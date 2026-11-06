// Ephemeral Windows CI registration check. Never run on daily-use machines.
// Explicitly registers and unregisters the experimental Lakakey TSF DLL in
// the current Windows CI account, with cleanup on every path.
// © 2026 Larx Vo — https://vovinhlac.com
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <iomanip>

namespace {
using ServerFunction = HRESULT (STDAPICALLTYPE*)();
constexpr wchar_t key_path[] =
    L"Software\\Classes\\CLSID\\{C67C6419-3D0F-4F56-9C4B-9D8A6E74A711}\\InprocServer32";
bool registered_key_exists() {
    HKEY key=nullptr;
    LONG code=RegOpenKeyExW(HKEY_CURRENT_USER,key_path,0,KEY_QUERY_VALUE,&key);
    if(code!=ERROR_SUCCESS) return false;
    RegCloseKey(key);
    return true;
}
int fail(const wchar_t* reason, HRESULT hr=S_OK) {
    std::wcerr << L"TSF profile smoke: FAIL: " << reason
               << L" (HRESULT 0x" << std::hex << static_cast<unsigned>(hr) << L")\n";
    return 1;
}
}
int wmain(int argc, wchar_t** argv) {
    if (argc != 3 || lstrcmpW(argv[2],L"--ephemeral-runner")!=0)
        return fail(L"requires DLL path and --ephemeral-runner; never run on a daily-use PC");
    if (registered_key_exists())
        return fail(L"service is already registered; refusing to alter existing state");
    HMODULE module=LoadLibraryExW(argv[1],nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if(!module) return fail(L"could not load TSF DLL");
    auto register_dll = reinterpret_cast<ServerFunction>(
        GetProcAddress(module,"DllRegisterServer"));
    auto unregister_dll = reinterpret_cast<ServerFunction>(
        GetProcAddress(module,"DllUnregisterServer"));
    if(!register_dll || !unregister_dll) {
        FreeLibrary(module);
        return fail(L"COM registration exports missing");
    }
    const HRESULT installed=register_dll();
    const bool entry_present=registered_key_exists();
    // Always attempt cleanup, even after a partial registration failure.
    const HRESULT removed=unregister_dll();
    const bool still_present=registered_key_exists();
    FreeLibrary(module);
    if(FAILED(installed)) return fail(L"registration rejected",installed);
    if(!entry_present) return fail(L"COM class key never appeared");
    if(FAILED(removed)) return fail(L"unregister rejected",removed);
    if(still_present) return fail(L"COM class key left after cleanup");
    std::wcout << L"TSF profile smoke: PASS (registered, found and cleaned)\n";
    return 0;
}
