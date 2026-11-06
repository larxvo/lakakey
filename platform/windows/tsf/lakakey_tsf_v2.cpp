// Lakakey TSF composition prototype — Windows only, experimental.
// Copyright (c) 2026 Larx Vo — https://vovinhlac.com
// This is an opt-in development DLL. Never auto-install it on end-user machines.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msctf.h>
#include <atomic>
#include <string>
#include <utility>
#include "lakakey_engine.h"
#include "lakakey_tsf_policy.h"

namespace {
constexpr CLSID kClsid = {0xc67c6419,0x3d0f,0x4f56,{0x9c,0x4b,0x9d,0x8a,0x6e,0x74,0xa7,0x11}};
constexpr GUID kProfile = {0x27fd617e,0x8b6c,0x4b12,{0x91,0x21,0x4b,0xc5,0x9e,0x10,0x77,0x31}};
constexpr wchar_t kClsidText[] = L"{C67C6419-3D0F-4F56-9C4B-9D8A6E74A711}";
constexpr wchar_t kProfileName[] = L"Lakakey Vietnamese (experimental TSF)";
HINSTANCE g_module = nullptr;
std::atomic<long> g_objects{0};

template<class T> void release(T*& p) { if (p) { p->Release(); p = nullptr; } }

std::wstring to_utf16(const std::string& utf8) {
    std::wstring out;
    for (char32_t cp : lakakey::utf8_decode(utf8)) {
        if (cp <= 0xffff) out.push_back(static_cast<wchar_t>(cp));
        else {
            cp -= 0x10000;
            out.push_back(static_cast<wchar_t>(0xd800 + (cp >> 10)));
            out.push_back(static_cast<wchar_t>(0xdc00 + (cp & 0x3ff)));
        }
    }
    return out;
}
bool blocked_modifiers() {
    return (GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_MENU) & 0x8000) ||
           (GetKeyState(VK_LWIN) & 0x8000) || (GetKeyState(VK_RWIN) & 0x8000);
}
bool is_secure(ITfContext* ctx) {
    if (!ctx) return true;
    TF_STATUS status{};
    if (FAILED(ctx->GetStatus(&status))) return true;
    // These are SDK status constants, not preprocessor feature macros.
    // Treat hidden/transitory contexts conservatively. This remains a
    // supplementary check, not proof of protection across browser frameworks.
    if (status.dwStaticFlags & (TF_SS_TRANSITORY | TF_SS_NOHIDDENTEXT))
        return true;
    // A conventional Win32 EDIT field exposes the password style.
    // Do not inspect its contents; only the window style is queried.
    HWND focus = GetFocus();
    if (focus) {
        wchar_t class_name[48]{};
        if (GetClassNameW(focus, class_name, 48) &&
            CompareStringOrdinal(class_name, -1, L"Edit", -1, TRUE) == CSTR_EQUAL &&
            (GetWindowLongPtrW(focus, GWL_STYLE) & ES_PASSWORD))
            return true;
    }
    return false;
}
char32_t ascii_from_vk(WPARAM vk) {
    if (vk >= 'A' && vk <= 'Z') {
        const bool upper = ((GetKeyState(VK_SHIFT) & 0x8000) != 0) !=
                           ((GetKeyState(VK_CAPITAL) & 1) != 0);
        return static_cast<char32_t>(upper ? vk : vk + 32);
    }
    if (vk >= '0' && vk <= '9') {
        if (GetKeyState(VK_SHIFT) & 0x8000) return 0;
        return static_cast<char32_t>(vk);
    }
    if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9)
        return static_cast<char32_t>('0' + vk - VK_NUMPAD0);
    return 0; // Unknown layouts: let the application handle the key.
}

// Per-user preference bridge from LakakeyDesktop. Only two DWORD values are
// read. This service does not read or write user keystroke history.
void read_input_preferences(lakakey::Settings& settings) {
    HKEY key=nullptr;
    constexpr wchar_t path[]=L"Software\\LarxVo\\Lakakey\\Input";
    if (RegOpenKeyExW(HKEY_CURRENT_USER,path,0,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS) return;
    auto query_dword=[&](const wchar_t* name, DWORD* value)->bool {
        DWORD kind=0, size=sizeof(DWORD), temp=0;
        const LONG code=RegQueryValueExW(key,name,nullptr,&kind,
                           reinterpret_cast<LPBYTE>(&temp),&size);
        if(code!=ERROR_SUCCESS || kind!=REG_DWORD || size!=sizeof(DWORD))return false;
        *value=temp;return true;
    };
    DWORD enabled=1;
    DWORD scheme=0;
    if(query_dword(L"Enabled",&enabled))settings.enabled=(enabled!=0);
    if(query_dword(L"Scheme",&scheme)) {
        if(scheme==0)settings.scheme=lakakey::Scheme::Telex;
        else if(scheme==1)settings.scheme=lakakey::Scheme::Vni;
    }
    RegCloseKey(key);
}

class Service;
class KeyEdit final : public ITfEditSession {
    std::atomic<ULONG> refs_{1};
    Service* service_;
    ITfContext* context_;
    lakakey::KeyAction action_;
    char32_t cp_;
public:
    KeyEdit(Service* service, ITfContext* ctx, char32_t cp, lakakey::KeyAction action);
    ~KeyEdit();
    STDMETHODIMP QueryInterface(REFIID id, void** obj) override;
    STDMETHODIMP_(ULONG) AddRef() override { return ++refs_; }
    STDMETHODIMP_(ULONG) Release() override { ULONG n = --refs_; if (!n) delete this; return n; }
    STDMETHODIMP DoEditSession(TfEditCookie cookie) override;
};

class Service final : public ITfTextInputProcessor,
                      public ITfKeyEventSink,
                      public ITfThreadMgrEventSink,
                      public ITfCompositionSink {
    std::atomic<ULONG> refs_{1};
    ITfThreadMgr* mgr_ = nullptr;
    ITfSource* source_ = nullptr;
    ITfComposition* composition_ = nullptr;
    ITfContext* composition_context_ = nullptr;
    DWORD sink_cookie_ = TF_INVALID_COOKIE;
    TfClientId client_ = TF_CLIENTID_NULL;
    lakakey::Engine engine_;
public:
    Service() { ++g_objects; }
    ~Service() { Deactivate(); --g_objects; }
    STDMETHODIMP QueryInterface(REFIID id, void** obj) override {
        if (!obj) return E_POINTER;
        *obj = nullptr;
        if (id == IID_IUnknown || id == IID_ITfTextInputProcessor)
            *obj = static_cast<ITfTextInputProcessor*>(this);
        else if (id == IID_ITfKeyEventSink)
            *obj = static_cast<ITfKeyEventSink*>(this);
        else if (id == IID_ITfThreadMgrEventSink)
            *obj = static_cast<ITfThreadMgrEventSink*>(this);
        else if (id == IID_ITfCompositionSink)
            *obj = static_cast<ITfCompositionSink*>(this);
        else return E_NOINTERFACE;
        AddRef(); return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++refs_; }
    STDMETHODIMP_(ULONG) Release() override { ULONG n = --refs_; if (!n) delete this; return n; }

    void reset_local() {
        // The host owns the actual composition lifecycle. A focus change
        // must never replay stale keystrokes in the next application's context.
        engine_.reset();
        release(composition_);
        release(composition_context_);
    }
    void refresh_input_preferences() {
        // Never discard an in-progress composition to apply a preference.
        if (composition_) return;
        lakakey::Settings requested=engine_.settings();
        read_input_preferences(requested);
        const auto current=engine_.settings();
        if (current.enabled!=requested.enabled || current.scheme!=requested.scheme)
            engine_.set_settings(requested);
    }
    STDMETHODIMP Activate(ITfThreadMgr* mgr, TfClientId client) override {
        if (!mgr) return E_POINTER;
        Deactivate();
        mgr_ = mgr; mgr_->AddRef(); client_ = client;
        ITfKeystrokeMgr* keys = nullptr;
        HRESULT hr = mgr_->QueryInterface(IID_ITfKeystrokeMgr, reinterpret_cast<void**>(&keys));
        if (SUCCEEDED(hr)) {
            hr = keys->AdviseKeyEventSink(client_, this, TRUE);
            keys->Release();
        }
        if (SUCCEEDED(hr)) {
            hr = mgr_->QueryInterface(IID_ITfSource, reinterpret_cast<void**>(&source_));
            if (SUCCEEDED(hr)) {
                hr = source_->AdviseSink(IID_ITfThreadMgrEventSink,
                                        static_cast<ITfThreadMgrEventSink*>(this), &sink_cookie_);
            }
        }
        if (FAILED(hr)) Deactivate();
        return hr;
    }
    STDMETHODIMP Deactivate() override {
        if (source_ && sink_cookie_ != TF_INVALID_COOKIE)
            source_->UnadviseSink(sink_cookie_);
        sink_cookie_ = TF_INVALID_COOKIE;
        release(source_);
        if (mgr_) {
            ITfKeystrokeMgr* keys = nullptr;
            if (SUCCEEDED(mgr_->QueryInterface(IID_ITfKeystrokeMgr,
                                               reinterpret_cast<void**>(&keys)))) {
                keys->UnadviseKeyEventSink(client_);
                keys->Release();
            }
        }
        reset_local();
        release(mgr_);
        client_ = TF_CLIENTID_NULL;
        return S_OK;
    }
    STDMETHODIMP OnSetFocus(BOOL foreground) override {
        if (!foreground) reset_local();
        return S_OK;
    }
    STDMETHODIMP OnInitDocumentMgr(ITfDocumentMgr*) override { return S_OK; }
    STDMETHODIMP OnUninitDocumentMgr(ITfDocumentMgr*) override { reset_local(); return S_OK; }
    STDMETHODIMP OnSetFocus(ITfDocumentMgr* current, ITfDocumentMgr* previous) override {
        if (current != previous) reset_local();
        return S_OK;
    }
    STDMETHODIMP OnPushContext(ITfContext*) override { reset_local(); return S_OK; }
    STDMETHODIMP OnPopContext(ITfContext*) override { reset_local(); return S_OK; }
    STDMETHODIMP OnCompositionTerminated(TfEditCookie, ITfComposition* ended) override {
        if (ended == composition_) {
            // Do not release the same composition twice from an EndComposition callback.
            release(composition_);
            release(composition_context_);
            engine_.reset();
        }
        return S_OK;
    }
    STDMETHODIMP OnPreservedKey(ITfContext*, REFGUID, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        *eaten = FALSE; return S_OK;
    }
    // Route both TSF probes and real key events through the exact same policy.
    // The host always receives navigation, punctuation and shortcuts.
    static bool is_boundary_key(WPARAM vk) noexcept {
        return vk == VK_SPACE || vk == VK_RETURN || vk == VK_TAB ||
            vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN ||
            vk == VK_HOME || vk == VK_END || vk == VK_DELETE || vk == VK_ESCAPE ||
            vk == VK_DECIMAL || (vk >= VK_OEM_1 && vk <= VK_OEM_3) ||
            (vk >= VK_OEM_4 && vk <= VK_OEM_8);
    }
    lakakey::KeyPlan route(ITfContext* ctx, WPARAM vk) {
        refresh_input_preferences();
        const bool active = composition_ != nullptr;
        return lakakey::plan_key({
            engine_.settings().enabled,
            is_secure(ctx),
            blocked_modifiers(),
            active,
            !active || composition_context_ == ctx,
            !engine_.preedit().empty(),
            is_boundary_key(vk),
            vk == VK_BACK,
            ascii_from_vk(vk)
        });
    }
    STDMETHODIMP OnTestKeyDown(ITfContext* ctx, WPARAM vk, LPARAM, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        const auto decision = route(ctx, vk);
        *eaten = decision.eat_on_success ? TRUE : FALSE;
        return S_OK;
    }
    STDMETHODIMP OnTestKeyUp(ITfContext*, WPARAM, LPARAM, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        *eaten = FALSE; return S_OK;
    }
    STDMETHODIMP OnKeyUp(ITfContext*, WPARAM, LPARAM, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        *eaten = FALSE; return S_OK;
    }
    STDMETHODIMP OnKeyDown(ITfContext* ctx, WPARAM vk, LPARAM, BOOL* eaten) override {
        if (!eaten) return E_POINTER;
        *eaten = FALSE;
        const auto decision = route(ctx, vk);
        if (decision.action == lakakey::KeyAction::Pass) return S_OK;
        KeyEdit* edit = new KeyEdit(this, ctx, ascii_from_vk(vk), decision.action);
        HRESULT session_result = E_FAIL;
        const HRESULT request = ctx->RequestEditSession(
            client_, edit, TF_ES_SYNC | TF_ES_READWRITE, &session_result);
        edit->Release();
        // Fail open on unsuccessful edit sessions. Never eat the original key
        // unless text was successfully committed to the application's TSF context.
        *eaten = decision.eat_on_success && SUCCEEDED(request) &&
                 session_result == S_OK ? TRUE : FALSE;
        return S_OK;
    }
    HRESULT ensure_composition(TfEditCookie cookie, ITfContext* ctx) {
        if (composition_) return composition_context_ == ctx ? S_OK : E_FAIL;
        ITfInsertAtSelection* insert = nullptr;
        HRESULT hr = ctx->QueryInterface(IID_ITfInsertAtSelection,
                                          reinterpret_cast<void**>(&insert));
        if (FAILED(hr)) return hr;
        ITfRange* selection = nullptr;
        hr = insert->InsertTextAtSelection(cookie, TF_IAS_QUERYONLY, L"", 0, &selection);
        insert->Release();
        if (FAILED(hr)) return hr;
        ITfContextComposition* host = nullptr;
        hr = ctx->QueryInterface(IID_ITfContextComposition,
                                  reinterpret_cast<void**>(&host));
        if (SUCCEEDED(hr)) {
            hr = host->StartComposition(cookie, selection,
                 static_cast<ITfCompositionSink*>(this), &composition_);
            host->Release();
        }
        release(selection);
        if (SUCCEEDED(hr)) {
            composition_context_ = ctx; composition_context_->AddRef();
        }
        return hr;
    }
    HRESULT update_text(TfEditCookie cookie, ITfContext* ctx, const std::string& utf8) {
        HRESULT hr = ensure_composition(cookie, ctx);
        if (FAILED(hr)) return hr;
        ITfRange* range = nullptr;
        hr = composition_->GetRange(&range);
        if (FAILED(hr)) return hr;
        const std::wstring text = to_utf16(utf8);
        hr = range->SetText(cookie, 0, text.c_str(), static_cast<LONG>(text.size()));
        if (SUCCEEDED(hr)) {
            ITfRange* caret = nullptr;
            if (SUCCEEDED(range->Clone(&caret))) {
                if (SUCCEEDED(caret->Collapse(cookie, TF_ANCHOR_END))) {
                    TF_SELECTION selected{};
                    selected.range = caret;
                    selected.style.ase = TF_AE_END;
                    selected.style.fInterimChar = FALSE;
                    (void)ctx->SetSelection(cookie, 1, &selected);
                }
                caret->Release();
            }
        }
        range->Release();
        return hr;
    }
    HRESULT finish(TfEditCookie cookie) {
        if (!composition_) { engine_.reset(); return S_OK; }
        ITfComposition* comp = composition_;
        comp->AddRef(); // callbacks may release the service's reference
        HRESULT hr = comp->EndComposition(cookie);
        comp->Release();
        if (SUCCEEDED(hr)) reset_local();
        return hr;
    }
    friend class KeyEdit;
};

KeyEdit::KeyEdit(Service* s, ITfContext* c, char32_t cp, lakakey::KeyAction action)
    : service_(s), context_(c), action_(action), cp_(cp) {
    service_->AddRef(); context_->AddRef();
}
KeyEdit::~KeyEdit() { context_->Release(); service_->Release(); }
STDMETHODIMP KeyEdit::QueryInterface(REFIID id, void** obj) {
    if (!obj) return E_POINTER;
    *obj = nullptr;
    if (id == IID_IUnknown || id == IID_ITfEditSession) {
        *obj = static_cast<ITfEditSession*>(this); AddRef(); return S_OK;
    }
    return E_NOINTERFACE;
}
STDMETHODIMP KeyEdit::DoEditSession(TfEditCookie cookie) {
    if (action_ == lakakey::KeyAction::Finalize) return service_->finish(cookie);
    lakakey::Engine next = service_->engine_;
    lakakey::Result result = action_ == lakakey::KeyAction::Erase ? next.backspace() : next.feed(cp_);
    if (!result.consumed) return S_FALSE;
    // The core emits committed text only on word boundaries / safety limits.
    // In that rare overflow case end the old composition before starting another.
    if (!result.commit.empty()) {
        HRESULT hr = service_->update_text(cookie, context_, result.commit);
        if (FAILED(hr)) return hr;
        hr = service_->finish(cookie);
        if (FAILED(hr)) return hr;
    }
    if (!result.preedit.empty()) {
        HRESULT hr = service_->update_text(cookie, context_, result.preedit);
        if (FAILED(hr)) return hr;
    } else if (service_->composition_) {
        HRESULT hr = service_->update_text(cookie, context_, "");
        if (FAILED(hr)) return hr;
        hr = service_->finish(cookie);
        if (FAILED(hr)) return hr;
    }
    service_->engine_ = std::move(next);
    return S_OK;
}

class Factory final : public IClassFactory {
    std::atomic<ULONG> refs_{1};
public:
    STDMETHODIMP QueryInterface(REFIID id, void** obj) override {
        if (!obj) return E_POINTER;
        *obj = nullptr;
        if (id == IID_IUnknown || id == IID_IClassFactory) {
            *obj = static_cast<IClassFactory*>(this); AddRef(); return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++refs_; }
    STDMETHODIMP_(ULONG) Release() override { ULONG n=--refs_; if (!n) delete this; return n; }
    STDMETHODIMP CreateInstance(IUnknown* outer, REFIID id, void** obj) override {
        if (outer) return CLASS_E_NOAGGREGATION;
        auto* service = new Service;
        HRESULT hr = service->QueryInterface(id, obj);
        service->Release();
        return hr;
    }
    STDMETHODIMP LockServer(BOOL lock) override { g_objects += lock ? 1 : -1; return S_OK; }
};

HRESULT register_service(bool enable) {
    wchar_t dll[MAX_PATH]{};
    if (!GetModuleFileNameW(g_module, dll, MAX_PATH))
        return HRESULT_FROM_WIN32(GetLastError());
    const std::wstring clsid_key = std::wstring(L"Software\\Classes\\CLSID\\") +
                                   kClsidText;
    if (enable) {
        HKEY key = nullptr;
        LONG err = RegCreateKeyExW(HKEY_CURRENT_USER,
                (clsid_key + L"\\InprocServer32").c_str(), 0, nullptr, 0,
                KEY_WRITE, nullptr, &key, nullptr);
        if (err != ERROR_SUCCESS) return HRESULT_FROM_WIN32(err);
        RegSetValueExW(key, nullptr, 0, REG_SZ,
                reinterpret_cast<const BYTE*>(dll),
                static_cast<DWORD>((wcslen(dll) + 1) * sizeof(wchar_t)));
        constexpr wchar_t threading[] = L"Apartment";
        RegSetValueExW(key, L"ThreadingModel", 0, REG_SZ,
                reinterpret_cast<const BYTE*>(threading), sizeof(threading));
        RegCloseKey(key);
    }
    ITfInputProcessorProfiles* profiles = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, nullptr,
                   CLSCTX_INPROC_SERVER, IID_ITfInputProcessorProfiles,
                   reinterpret_cast<void**>(&profiles));
    if (FAILED(hr)) return hr;
    if (enable) {
        hr = profiles->Register(kClsid);
        if (SUCCEEDED(hr)) {
            hr = profiles->AddLanguageProfile(kClsid, MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT),
                    kProfile, kProfileName, static_cast<ULONG>(wcslen(kProfileName)),
                    dll, static_cast<ULONG>(wcslen(dll)), 0);
        }
    } else {
        // Release the language profile before unregistering the text service.
        // Registration can fail mid-way, so cleanup should be idempotent.
        const HRESULT remove_hr = profiles->RemoveLanguageProfile(
            kClsid, MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT), kProfile);
        (void)remove_hr;
        hr = profiles->Unregister(kClsid);
    }
    profiles->Release();
    if (!enable) {
        const LONG erased = RegDeleteTreeW(HKEY_CURRENT_USER, clsid_key.c_str());
        if (erased != ERROR_SUCCESS && erased != ERROR_FILE_NOT_FOUND &&
            erased != ERROR_PATH_NOT_FOUND)
            return HRESULT_FROM_WIN32(erased);
    }
    return hr;
}
} // namespace

extern "C" BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_module = instance; DisableThreadLibraryCalls(instance);
    }
    return TRUE;
}
extern "C" __declspec(dllexport) HRESULT __stdcall LakakeyDllCanUnloadNow() {
    return g_objects == 0 ? S_OK : S_FALSE;
}
extern "C" __declspec(dllexport) HRESULT __stdcall LakakeyDllGetClassObject(
        REFCLSID clsid, REFIID iid, void** object) {
    if (clsid != kClsid) return CLASS_E_CLASSNOTAVAILABLE;
    auto* factory = new Factory;
    HRESULT hr = factory->QueryInterface(iid, object);
    factory->Release();
    return hr;
}
extern "C" __declspec(dllexport) HRESULT __stdcall LakakeyDllRegisterServer() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool initialized = SUCCEEDED(hr);
    if (hr == RPC_E_CHANGED_MODE) hr = S_OK;
    if (SUCCEEDED(hr)) hr = register_service(true);
    if (initialized) CoUninitialize();
    return hr;
}
extern "C" __declspec(dllexport) HRESULT __stdcall LakakeyDllUnregisterServer() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool initialized = SUCCEEDED(hr);
    if (hr == RPC_E_CHANGED_MODE) hr = S_OK;
    if (SUCCEEDED(hr)) hr = register_service(false);
    if (initialized) CoUninitialize();
    return hr;
}
