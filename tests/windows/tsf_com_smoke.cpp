// Windows CI smoke test: inspect TSF COM exports without registering a text service.
// It never installs input services, captures typing, or changes user input state.
// © 2026 Larx Vo — https://vovinhlac.com
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msctf.h>
#include <iostream>
#include <string>

namespace {
constexpr CLSID kClsid = {0xc67c6419,0x3d0f,0x4f56,{0x9c,0x4b,0x9d,0x8a,0x6e,0x74,0xa7,0x11}};
using FactoryExport = HRESULT (STDAPICALLTYPE*)(REFCLSID, REFIID, void**);
using CanUnloadExport = HRESULT (STDAPICALLTYPE*)();

int error(const wchar_t* message) {
    std::wcerr << L"Lakakey TSF COM smoke: FAIL — " << message << L"\n";
    return 1;
}
}
int wmain(int argc, wchar_t** argv) {
    if (argc != 2 || !argv[1] || !*argv[1])
        return error(L"expected exact path to the experimental TSF DLL");
    HMODULE module = LoadLibraryExW(argv[1], nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (!module) return error(L"cannot load the TSF DLL");
    auto factory_export = reinterpret_cast<FactoryExport>(
        GetProcAddress(module, "DllGetClassObject"));
    auto can_unload = reinterpret_cast<CanUnloadExport>(
        GetProcAddress(module, "DllCanUnloadNow"));
    if (!factory_export || !can_unload) {
        FreeLibrary(module);
        return error(L"required COM exports missing");
    }
    IClassFactory* factory = nullptr;
    const HRESULT found = factory_export(kClsid, IID_IClassFactory,
                                        reinterpret_cast<void**>(&factory));
    if (FAILED(found) || !factory) {
        FreeLibrary(module);
        return error(L"COM class factory unavailable");
    }
    ITfTextInputProcessor* service = nullptr;
    const HRESULT created = factory->CreateInstance(nullptr, IID_ITfTextInputProcessor,
                                                    reinterpret_cast<void**>(&service));
    if (FAILED(created) || !service) {
        factory->Release();
        FreeLibrary(module);
        return error(L"cannot instantiate text service");
    }
    ITfKeyEventSink* key_events = nullptr;
    const HRESULT keys = service->QueryInterface(IID_ITfKeyEventSink,
                                               reinterpret_cast<void**>(&key_events));
    ITfThreadMgrEventSink* thread_events = nullptr;
    const HRESULT thread = service->QueryInterface(IID_ITfThreadMgrEventSink,
                                                  reinterpret_cast<void**>(&thread_events));
    if (key_events) key_events->Release();
    if (thread_events) thread_events->Release();
    service->Release();
    factory->Release();
    const HRESULT unloaded = can_unload();
    FreeLibrary(module);
    if (FAILED(keys) || FAILED(thread))
        return error(L"mandatory TSF event sink interface missing");
    if (unloaded != S_OK)
        return error(L"COM object count remains nonzero after release");
    std::wcout << L"Lakakey TSF COM smoke: PASS (factory, interface and lifetime)\n";
    return 0;
}
