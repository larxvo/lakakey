// Lakakey experimental TSF text service. © 2026 Larx Vo
// This is an early COM/TSF prototype; not a production-ready IME.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msctf.h>
#include <atomic>
#include <string>
#include "lakakey_engine.h"
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "msctf.lib")
namespace {
HINSTANCE module_handle = nullptr;
std::atomic<long> object_count{0};
constexpr wchar_t clsid_string[] = L"{C67C6419-3D0F-4F56-9C4B-9D8A6E74A711}";
const CLSID CLSID_Lakakey = {0xc67c6419,0x3d0f,0x4f56,{0x9c,0x4b,0x9d,0x8a,0x6e,0x74,0xa7,0x11}};
const GUID GUID_LakakeyProfile = {0x27fd617e,0x8b6c,0x4b12,{0x91,0x21,0x4b,0xc5,0x9e,0x10,0x77,0x31}};
std::wstring widen(const std::u32string& s) {
    std::wstring out;
    for(char32_t c:s) {
        if(c<=0xffff) out.push_back(static_cast<wchar_t>(c));
        else {c-=0x10000;out.push_back(static_cast<wchar_t>(0xd800+(c>>10)));out.push_back(static_cast<wchar_t>(0xdc00+(c&1023)));}
    }
    return out;
}
class TextService;
class Edit final : public ITfEditSession {
    std::atomic<ULONG> refs{1};
    ITfContext* context;
    std::wstring text;
public:
    Edit(ITfContext* c,std::wstring t):context(c),text(std::move(t)){context->AddRef();}
    ~Edit(){context->Release();}
    STDMETHODIMP QueryInterface(REFIID riid,void** out) override {
        if(!out)return E_POINTER;*out=nullptr;
        if(riid==IID_IUnknown||riid==IID_ITfEditSession){*out=static_cast<ITfEditSession*>(this);AddRef();return S_OK;}return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override{return ++refs;}
    STDMETHODIMP_(ULONG) Release() override{auto n=--refs;if(!n)delete this;return n;}
    STDMETHODIMP DoEditSession(TfEditCookie cookie) override {
        ITfInsertAtSelection* insert=nullptr;
        HRESULT hr=context->QueryInterface(IID_ITfInsertAtSelection,reinterpret_cast<void**>(&insert));
        if(SUCCEEDED(hr)){
            ITfRange* range=nullptr;
            hr=insert->InsertTextAtSelection(cookie,TF_IAS_NOQUERY,text.c_str(),static_cast<LONG>(text.size()),&range);
            if(range)range->Release();insert->Release();
        }
        return hr;
    }
};
class TextService final : public ITfTextInputProcessor, public ITfKeyEventSink {
    std::atomic<ULONG> refs{1};
    ITfThreadMgr* mgr=nullptr;
    TfClientId client=TF_CLIENTID_NULL;
    std::u32string pending;
    lakakey::Engine engine;
public:
    TextService(){++object_count;}
    ~TextService(){Deactivate();--object_count;}
    STDMETHODIMP QueryInterface(REFIID riid,void** out) override {
        if(!out)return E_POINTER;*out=nullptr;
        if(riid==IID_IUnknown||riid==IID_ITfTextInputProcessor)*out=static_cast<ITfTextInputProcessor*>(this);
        else if(riid==IID_ITfKeyEventSink)*out=static_cast<ITfKeyEventSink*>(this);
        else return E_NOINTERFACE;
        AddRef();return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override{return ++refs;}
    STDMETHODIMP_(ULONG) Release() override{auto n=--refs;if(!n)delete this;return n;}
    STDMETHODIMP Activate(ITfThreadMgr* tm,TfClientId id) override {
        if(!tm)return E_POINTER;
        Deactivate();mgr=tm;mgr->AddRef();client=id;
        ITfKeystrokeMgr* keys=nullptr;
        HRESULT hr=mgr->QueryInterface(IID_ITfKeystrokeMgr,reinterpret_cast<void**>(&keys));
        if(SUCCEEDED(hr)){hr=keys->AdviseKeyEventSink(client,this,TRUE);keys->Release();}
        return hr;
    }
    STDMETHODIMP Deactivate() override {
        if(mgr){ITfKeystrokeMgr* keys=nullptr;
            if(SUCCEEDED(mgr->QueryInterface(IID_ITfKeystrokeMgr,reinterpret_cast<void**>(&keys)))){keys->UnadviseKeyEventSink(client);keys->Release();}
            mgr->Release();mgr=nullptr;
        }pending.clear();return S_OK;
    }
    STDMETHODIMP OnSetFocus(BOOL) override {pending.clear();return S_OK;}
    STDMETHODIMP OnTestKeyDown(ITfContext*,WPARAM w,LPARAM,BOOL* eaten) override {
        if(!eaten)return E_POINTER;
        *eaten=(w>=L'A'&&w<=L'Z')||(w>=L'0'&&w<=L'9');
        return S_OK;
    }
    STDMETHODIMP OnTestKeyUp(ITfContext*,WPARAM,LPARAM,BOOL* eaten) override {if(!eaten)return E_POINTER;*eaten=FALSE;return S_OK;}
    STDMETHODIMP OnKeyUp(ITfContext*,WPARAM,LPARAM,BOOL* eaten) override {if(!eaten)return E_POINTER;*eaten=FALSE;return S_OK;}
    STDMETHODIMP OnKeyDown(ITfContext* context,WPARAM w,LPARAM,BOOL* eaten) override {
        if(!eaten)return E_POINTER;*eaten=FALSE;
        if(!context||!((w>=L'A'&&w<=L'Z')||(w>=L'0'&&w<=L'9')))return S_OK;
        // Conservative: prototype does not swallow keys until composition editing is implemented.
        // Prevents dropping characters in applications that do not support synchronous edit sessions.
        return S_OK;
    }
};
class Factory final : public IClassFactory {
    std::atomic<ULONG> refs{1};
public:
    STDMETHODIMP QueryInterface(REFIID riid,void** out) override {
        if(!out)return E_POINTER;*out=nullptr;
        if(riid==IID_IUnknown||riid==IID_IClassFactory){*out=static_cast<IClassFactory*>(this);AddRef();return S_OK;}return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override{return ++refs;}
    STDMETHODIMP_(ULONG) Release() override{auto n=--refs;if(!n)delete this;return n;}
    STDMETHODIMP CreateInstance(IUnknown* outer,REFIID riid,void** out) override {
        if(outer)return CLASS_E_NOAGGREGATION;
        auto* service=new TextService;HRESULT hr=service->QueryInterface(riid,out);service->Release();return hr;
    }
    STDMETHODIMP LockServer(BOOL lock) override {object_count+=(lock?1:-1);return S_OK;}
};
HRESULT register_server(bool add) {
    wchar_t path[MAX_PATH]{};
    if(!GetModuleFileNameW(module_handle,path,MAX_PATH))return HRESULT_FROM_WIN32(GetLastError());
    std::wstring key=L"Software\\Classes\\CLSID\\"+std::wstring(clsid_string)+L"\\InprocServer32";
    if(add){
        HKEY h=nullptr;LONG err=RegCreateKeyExW(HKEY_CURRENT_USER,key.c_str(),0,nullptr,0,KEY_WRITE,nullptr,&h,nullptr);
        if(err!=ERROR_SUCCESS)return HRESULT_FROM_WIN32(err);
        RegSetValueExW(h,nullptr,0,REG_SZ,reinterpret_cast<const BYTE*>(path),static_cast<DWORD>((wcslen(path)+1)*sizeof(wchar_t)));
        constexpr wchar_t model[]=L"Apartment";
        RegSetValueExW(h,L"ThreadingModel",0,REG_SZ,reinterpret_cast<const BYTE*>(model),sizeof(model));
        RegCloseKey(h);
    }else{
        RegDeleteTreeW(HKEY_CURRENT_USER,(L"Software\\Classes\\CLSID\\"+std::wstring(clsid_string)).c_str());
    }
    ITfInputProcessorProfiles* profiles=nullptr;
    HRESULT hr=CoCreateInstance(CLSID_TF_InputProcessorProfiles,nullptr,CLSCTX_INPROC_SERVER,IID_ITfInputProcessorProfiles,reinterpret_cast<void**>(&profiles));
    if(FAILED(hr))return hr;
    if(add){
        hr=profiles->Register(CLSID_Lakakey);
        if(SUCCEEDED(hr))hr=profiles->AddLanguageProfile(CLSID_Lakakey,0x042a,GUID_LakakeyProfile,L"Lakakey Vietnamese (experimental)",static_cast<ULONG>(wcslen(L"Lakakey Vietnamese (experimental)")),path,0);
    }else hr=profiles->Unregister(CLSID_Lakakey);
    profiles->Release();return hr;
}
}
extern "C" BOOL WINAPI DllMain(HINSTANCE h,DWORD why,LPVOID){if(why==DLL_PROCESS_ATTACH){module_handle=h;DisableThreadLibraryCalls(h);}return TRUE;}
extern "C" __declspec(dllexport) HRESULT __stdcall DllCanUnloadNow(){return object_count==0?S_OK:S_FALSE;}
extern "C" __declspec(dllexport) HRESULT __stdcall DllGetClassObject(REFCLSID cls,REFIID iid,void** out){
    if(cls!=CLSID_Lakakey)return CLASS_E_CLASSNOTAVAILABLE;
    auto* factory=new Factory;HRESULT hr=factory->QueryInterface(iid,out);factory->Release();return hr;
}
extern "C" __declspec(dllexport) HRESULT __stdcall DllRegisterServer(){HRESULT hr=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);bool initialized=SUCCEEDED(hr);if(hr==RPC_E_CHANGED_MODE)hr=S_OK;if(SUCCEEDED(hr))hr=register_server(true);if(initialized)CoUninitialize();return hr;}
extern "C" __declspec(dllexport) HRESULT __stdcall DllUnregisterServer(){HRESULT hr=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);bool initialized=SUCCEEDED(hr);if(hr==RPC_E_CHANGED_MODE)hr=S_OK;if(SUCCEEDED(hr))hr=register_server(false);if(initialized)CoUninitialize();return hr;}
