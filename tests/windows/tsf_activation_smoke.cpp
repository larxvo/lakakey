// COM integration smoke: activate Lakakey TSF against a real thread manager.
// No registration, SendInput or user text capture. © 2026 Larx Vo.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msctf.h>
#include <iostream>

namespace {
constexpr CLSID kClsid = {0xc67c6419,0x3d0f,0x4f56,{0x9c,0x4b,0x9d,0x8a,0x6e,0x74,0xa7,0x11}};
using FactoryExport = HRESULT (STDAPICALLTYPE*)(REFCLSID, REFIID, void**);
using CanUnloadExport = HRESULT (STDAPICALLTYPE*)();
int fail(const wchar_t* stage, HRESULT code) {
    std::wcerr << L"TSF activation smoke FAILED: " << stage
               << L" (0x" << std::hex << static_cast<unsigned>(code) << L")\n";
    return 1;
}
}
int wmain(int argc, wchar_t** argv) {
    if (argc != 2) return fail(L"DLL argument missing", E_INVALIDARG);
    HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(init)) return fail(L"COM apartment initialization", init);
    HMODULE dll = LoadLibraryExW(argv[1], nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (!dll) { CoUninitialize(); return fail(L"DLL loading", HRESULT_FROM_WIN32(GetLastError())); }
    auto create = reinterpret_cast<FactoryExport>(GetProcAddress(dll, "DllGetClassObject"));
    auto can_unload = reinterpret_cast<CanUnloadExport>(GetProcAddress(dll, "DllCanUnloadNow"));
    if (!create || !can_unload) {
        FreeLibrary(dll); CoUninitialize();
        return fail(L"COM exports", E_NOINTERFACE);
    }
    ITfThreadMgr* mgr = nullptr;
    ITfTextInputProcessor* service = nullptr;
    ITfKeyEventSink* keys = nullptr;
    IClassFactory* factory = nullptr;
    TfClientId client = TF_CLIENTID_NULL;
    bool mgr_active = false;
    bool service_active = false;
    HRESULT result = CoCreateInstance(CLSID_TF_ThreadMgr, nullptr, CLSCTX_INPROC_SERVER,
                                      IID_ITfThreadMgr, reinterpret_cast<void**>(&mgr));
    if (SUCCEEDED(result))
        result = mgr->Activate(&client);
    if (SUCCEEDED(result)) mgr_active = true;
    if (SUCCEEDED(result))
        result = create(kClsid, IID_IClassFactory, reinterpret_cast<void**>(&factory));
    if (SUCCEEDED(result))
        result = factory->CreateInstance(nullptr, IID_ITfTextInputProcessor,
                                         reinterpret_cast<void**>(&service));
    if (SUCCEEDED(result))
        result = service->Activate(mgr, client);
    if (SUCCEEDED(result)) service_active = true;
    if (SUCCEEDED(result))
        result = service->QueryInterface(IID_ITfKeyEventSink, reinterpret_cast<void**>(&keys));
    if (SUCCEEDED(result)) {
        BOOL eaten = TRUE;
        result = keys->OnTestKeyDown(nullptr, 'A', 0, &eaten);
        if (SUCCEEDED(result) && eaten) result = E_UNEXPECTED;
        if (SUCCEEDED(result)) {
            eaten = TRUE;
            result = keys->OnKeyDown(nullptr, 'A', 0, &eaten);
            if (SUCCEEDED(result) && eaten) result = E_UNEXPECTED;
        }
    }
    if (keys) keys->Release();
    if (service_active) service->Deactivate();
    if (service) service->Release();
    if (factory) factory->Release();
    if (mgr_active) mgr->Deactivate();
    if (mgr) mgr->Release();
    HRESULT remaining = can_unload();
    FreeLibrary(dll);
    CoUninitialize();
    if (FAILED(result)) return fail(L"TSF manager activation or key sink", result);
    if (remaining != S_OK) return fail(L"COM references after cleanup", remaining);
    std::wcout << L"TSF activation smoke: PASS (thread manager, key sink, cleanup)\n";
    return 0;
}
