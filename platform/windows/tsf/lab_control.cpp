// Lakakey TSF developer control — isolated Windows QA only.
// Never bundled with the normal Windows installer.
// © 2026 Larx Vo · https://vovinhlac.com
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <iostream>
#include <cwchar>

namespace {
constexpr wchar_t kInproc[] =
    L"Software\\Classes\\CLSID\\{C67C6419-3D0F-4F56-9C4B-9D8A6E74A711}\\InprocServer32";
using ServerFn = HRESULT (STDAPICALLTYPE*)();

std::wstring registration_path() {
    wchar_t buffer[32768]{};
    DWORD bytes = sizeof(buffer);
    DWORD type = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, kInproc, nullptr,
                     RRF_RT_REG_SZ, &type, buffer, &bytes) != ERROR_SUCCESS)
        return {};
    return buffer;
}
int error(const wchar_t* msg) {
    std::wcerr << L"Lakakey TSF Lab: " << msg << L"\n";
    return 1;
}
HRESULT invoke(const std::wstring& dll_path, const char* export_name) {
    HMODULE dll = LoadLibraryExW(dll_path.c_str(), nullptr,
                LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (!dll) return HRESULT_FROM_WIN32(GetLastError());
    auto fn = reinterpret_cast<ServerFn>(GetProcAddress(dll, export_name));
    HRESULT result = fn ? fn() : HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    FreeLibrary(dll);
    return result;
}
bool absolute_dll(const wchar_t* original, std::wstring& resolved) {
    if (!original || !*original) return false;
    wchar_t path[32768]{};
    DWORD len = GetFullPathNameW(original, 32768, path, nullptr);
    if (!len || len >= 32768) return false;
    resolved.assign(path, len);
    if (resolved.size() < 4 ||
        _wcsicmp(resolved.c_str() + resolved.size() - 4, L".dll") != 0)
        return false;
    DWORD attrs = GetFileAttributesW(resolved.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}
void usage() {
    std::wcout <<
        L"Lakakey TSF Lab — isolated test VM only\n"
        L"  --status\n"
        L"  --register \"C:\\path\\LakakeyTsfV2.dll\" --dev-only\n"
        L"  --unregister --dev-only\n\n"
        L"Only explicit commands change TSF registration. This is not a stable input release.\n";
}
} // namespace
int wmain(int argc, wchar_t** argv) {
    if (argc == 2 && wcscmp(argv[1], L"--status") == 0) {
        const auto path = registration_path();
        std::wcout << (path.empty() ? L"Not registered" : L"Registered test DLL: " + path)
                   << L"\n";
        return 0;
    }
    if (argc == 4 && wcscmp(argv[1], L"--register") == 0 &&
        wcscmp(argv[3], L"--dev-only") == 0) {
        if (!registration_path().empty())
            return error(L"Existing TSF registration found; remove it explicitly first.");
        std::wstring path;
        if (!absolute_dll(argv[2], path))
            return error(L"Provide an existing absolute path to LakakeyTsfV2.dll.");
        HRESULT result = invoke(path, "DllRegisterServer");
        if (FAILED(result)) {
            (void)invoke(path, "DllUnregisterServer");
            return error(L"Registration failed; cleanup attempted.");
        }
        const auto stored = registration_path();
        if (stored.empty() || _wcsicmp(stored.c_str(), path.c_str()) != 0) {
            (void)invoke(path, "DllUnregisterServer");
            return error(L"Registered DLL path does not match; cleanup attempted.");
        }
        std::wcout << L"Developer TSF profile registered: " << path << L"\n";
        return 0;
    }
    if (argc == 3 && wcscmp(argv[1], L"--unregister") == 0 &&
        wcscmp(argv[2], L"--dev-only") == 0) {
        const std::wstring path = registration_path();
        if (path.empty()) {
            std::wcout << L"No developer registration found.\n";
            return 0;
        }
        const HRESULT result = invoke(path, "DllUnregisterServer");
        if (FAILED(result) || !registration_path().empty())
            return error(L"Unregistration did not finish; inspect Windows test VM.");
        std::wcout << L"Developer TSF registration removed.\n";
        return 0;
    }
    usage();
    return 2;
}
