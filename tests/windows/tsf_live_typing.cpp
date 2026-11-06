// Windows-only opt-in live typing experiment. DISPOSABLE CI RUNNER ONLY.
// This test sends fixed synthetic keys to its own native EDIT window.
// It never records user input, changes other applications or ships in Setup.
// Copyright (c) 2026 Larx Vo — https://vovinhlac.com
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msctf.h>
#include <string>
#include <iostream>
#include <iomanip>

namespace {
constexpr CLSID service_clsid = {0xc67c6419,0x3d0f,0x4f56,{0x9c,0x4b,0x9d,0x8a,0x6e,0x74,0xa7,0x11}};
constexpr GUID profile_id = {0x27fd617e,0x8b6c,0x4b12,{0x91,0x21,0x4b,0xc5,0x9e,0x10,0x77,0x31}};
constexpr LANGID vietnamese = MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT);
constexpr wchar_t input_key[] = L"Software\\LarxVo\\Lakakey\\Input";
constexpr wchar_t class_key[] =
    L"Software\\Classes\\CLSID\\{C67C6419-3D0F-4F56-9C4B-9D8A6E74A711}\\InprocServer32";
using DllFunc = HRESULT (STDAPICALLTYPE*)();
HWND field=nullptr;
HWND password_field=nullptr;
LRESULT CALLBACK on_window(HWND window, UINT msg, WPARAM w, LPARAM l) {
    if (msg == WM_CREATE) {
        HINSTANCE instance=reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(window,GWLP_HINSTANCE));
        field=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
            20,30,440,34,window,reinterpret_cast<HMENU>(110),instance,nullptr);
        password_field=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_PASSWORD | ES_AUTOHSCROLL,
            20,85,440,34,window,reinterpret_cast<HMENU>(111),instance,nullptr);
        return (field && password_field)?0:-1;
    }
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(window,msg,w,l);
}
bool has_registry_key(HKEY root,const wchar_t* path) {
    HKEY key=nullptr;
    const LONG code=RegOpenKeyExW(root,path,0,KEY_READ,&key);
    if(code!=ERROR_SUCCESS)return false;
    RegCloseKey(key);return true;
}
bool set_mode(DWORD enabled=1, DWORD scheme=0) {
    HKEY key=nullptr;
    if(RegCreateKeyExW(HKEY_CURRENT_USER,input_key,0,nullptr,0,KEY_WRITE,
                       nullptr,&key,nullptr)!=ERROR_SUCCESS)return false;
    const LONG a=RegSetValueExW(key,L"Enabled",0,REG_DWORD,
                      reinterpret_cast<const BYTE*>(&enabled),sizeof(enabled));
    const LONG b=RegSetValueExW(key,L"Scheme",0,REG_DWORD,
                      reinterpret_cast<const BYTE*>(&scheme),sizeof(scheme));
    RegCloseKey(key);
    return a==ERROR_SUCCESS && b==ERROR_SUCCESS;
}
void pump_for(DWORD milliseconds) {
    const ULONGLONG deadline=GetTickCount64()+milliseconds;
    MSG message{};
    while(GetTickCount64()<deadline) {
        while(PeekMessageW(&message,nullptr,0,0,PM_REMOVE)) {
            TranslateMessage(&message);DispatchMessageW(&message);
        }
        Sleep(10);
    }
}
bool send_letter(WORD vk) {
    INPUT key[2]{};
    for(auto& item:key){item.type=INPUT_KEYBOARD;item.ki.wVk=vk;}
    key[1].ki.dwFlags=KEYEVENTF_KEYUP;
    return SendInput(2,key,sizeof(INPUT))==2;
}
std::wstring read_field(HWND target) {
    if(!target)return {};
    const int size=GetWindowTextLengthW(target);
    std::wstring value(static_cast<size_t>(size)+1,L'\0');
    const int got=GetWindowTextW(target,&value[0],size+1);
    value.resize(static_cast<size_t>(got));
    return value;
}
bool send_shifted(WORD vk) {
    INPUT keys[4]{};
    for(auto& k:keys) k.type=INPUT_KEYBOARD;
    keys[0].ki.wVk=VK_SHIFT;
    keys[1].ki.wVk=vk;
    keys[2].ki.wVk=vk; keys[2].ki.dwFlags=KEYEVENTF_KEYUP;
    keys[3].ki.wVk=VK_SHIFT; keys[3].ki.dwFlags=KEYEVENTF_KEYUP;
    return SendInput(4,keys,sizeof(INPUT))==4;
}
bool run_case(HWND target, const wchar_t* name, const WORD* keys,
              size_t count, const std::wstring& expected, bool shifted_final=false) {
    SetWindowTextW(target,L"");
    SetFocus(target);
    pump_for(160);
    if(GetFocus()!=target) {
        std::wcerr << L"LIVE_INPUT_NOT_VERIFIED: cannot focus " << name << L"\\n";
        return false;
    }
    for(size_t i=0;i<count;++i) {
        bool ok=(shifted_final && i==count-1) ? send_shifted(keys[i]) : send_letter(keys[i]);
        if(!ok) return false;
        pump_for(55);
    }
    pump_for(250);
    const std::wstring actual=read_field(target);
    if(actual!=expected) {
        // Do not print actual text; this is synthetic data only.
        std::wcerr << L"LIVE_INPUT_NOT_VERIFIED: " << name
                   << L" (actual code units=" << actual.size() << L")\\n";
        return false;
    }
    std::wcout << L"LIVE_INPUT_CASE_PASS: " << name << L"\\n";
    return true;
}
void report_failure(const wchar_t* stage, HRESULT status=E_FAIL) {
    std::wcerr<<L"LIVE_INPUT_NOT_VERIFIED: "<<stage<<L" (0x"<<std::hex
              <<static_cast<unsigned>(status)<<L")\n";
}
}
int wmain(int argc,wchar_t** argv) {
    if(argc!=3 || lstrcmpW(argv[2],L"--ephemeral-runner")!=0) {
        report_failure(L"Requires DLL path and disposable Windows CI flag",E_INVALIDARG);
        return 2;
    }
    // Never alter an existing profile or input preference, including on a VM
    // that may have been reused for another experiment.
    if(has_registry_key(HKEY_CURRENT_USER,class_key) ||
       has_registry_key(HKEY_CURRENT_USER,input_key)) {
        report_failure(L"Existing registration or input preferences; refusing to modify");
        return 2;
    }
    if(!set_mode())return 2;
    const HRESULT init=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
    HMODULE dll=nullptr;DllFunc register_dll=nullptr,unregister_dll=nullptr;
    bool installed=false,thread_active=false,profile_active=false;
    ITfThreadMgr* mgr=nullptr;ITfInputProcessorProfiles* profiles=nullptr;
    TfClientId client=TF_CLIENTID_NULL;
    HWND window=nullptr; HRESULT status=S_OK;bool typed=false;
    do {
        if(FAILED(init)) {status=init;report_failure(L"COM initialization",status);break;}
        dll=LoadLibraryExW(argv[1],nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
                                            LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if(!dll) {report_failure(L"Cannot load experimental DLL");break;}
        register_dll=reinterpret_cast<DllFunc>(GetProcAddress(dll,"DllRegisterServer"));
        unregister_dll=reinterpret_cast<DllFunc>(GetProcAddress(dll,"DllUnregisterServer"));
        if(!register_dll || !unregister_dll) {report_failure(L"Registration exports missing");break;}
        status=register_dll();
        if(FAILED(status)) {report_failure(L"RegisterLanguageProfile",status);break;}
        installed=true;
        status=CoCreateInstance(CLSID_TF_ThreadMgr,nullptr,CLSCTX_INPROC_SERVER,
                 IID_ITfThreadMgr,reinterpret_cast<void**>(&mgr));
        if(FAILED(status)) {report_failure(L"ThreadMgr",status);break;}
        status=mgr->Activate(&client);
        if(FAILED(status)) {report_failure(L"ThreadMgr Activate",status);break;}
        thread_active=true;
        status=CoCreateInstance(CLSID_TF_InputProcessorProfiles,nullptr,
                CLSCTX_INPROC_SERVER,IID_ITfInputProcessorProfiles,
                reinterpret_cast<void**>(&profiles));
        if(FAILED(status)){report_failure(L"InputProfiles COM",status);break;}
        status=profiles->ActivateLanguageProfile(service_clsid,vietnamese,profile_id);
        if(FAILED(status)){report_failure(L"Profile activation",status);break;}
        profile_active=true;
        HINSTANCE module=GetModuleHandleW(nullptr);
        WNDCLASSW wc{};
        wc.lpfnWndProc=on_window;wc.hInstance=module;
        wc.lpszClassName=L"Lakakey014IsolatedTypingLab";
        wc.hbrBackground=reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);
        wc.hCursor=LoadCursorW(nullptr,IDC_IBEAM);
        if(!RegisterClassW(&wc)){report_failure(L"Window class");break;}
        window=CreateWindowExW(0,wc.lpszClassName,L"Lakakey Native Input Lab",
             WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,510,230,
             nullptr,nullptr,module,nullptr);
        if(!window||!field||!password_field){report_failure(L"Edit control not created");break;}
        ShowWindow(window,SW_SHOWNORMAL);
        SetForegroundWindow(window); SetFocus(field);
        pump_for(450);
        if(GetFocus()!=field){report_failure(L"Runner has no interactive keyboard focus");break;}
        const WORD telex[]={ 'T','I','E','E','N','G','S',VK_SPACE };
        if(!run_case(field,L"Telex basic EDIT",telex,8,L"tiếng "))break;

        const WORD punctuation[]={ 'C','H','A','O','F','1' };
        if(!run_case(field,L"Telex punctuation EDIT",punctuation,6,L"chào!",true))break;

        if(!set_mode(1,1)){report_failure(L"Switching to VNI");break;}
        pump_for(130);
        const WORD vni[]={ 'T','I','E','6','N','G','1',VK_SPACE };
        if(!run_case(field,L"VNI native EDIT",vni,8,L"tiếng "))break;

        if(!set_mode(0,0)){report_failure(L"Disabling native composition");break;}
        pump_for(130);
        if(!run_case(field,L"Disabled-mode pass-through",telex,8,L"tieengs "))break;

        if(!set_mode(1,0)){report_failure(L"Re-enabling Telex");break;}
        pump_for(130);
        if(!run_case(password_field,L"Password-field pass-through",telex,8,L"tieengs "))break;

        // A fresh field after leaving a password control must not inherit
        // that control's composition state. Punctuation is tested above.
        const WORD return_to_edit[]={ 'D','D','O','O','F',VK_SPACE };
        if(!run_case(field,L"Focus return native EDIT",return_to_edit,6,L"đồ "))break;
        typed=true;
    } while(false);
    if(window)DestroyWindow(window);
    if(profile_active && profiles)
        (void)profiles->EnableLanguageProfile(service_clsid,vietnamese,profile_id,FALSE);
    if(profiles)profiles->Release();
    if(thread_active && mgr)mgr->Deactivate();
    if(mgr)mgr->Release();
    if(unregister_dll) (void)unregister_dll();
    if(dll)FreeLibrary(dll);
    if(SUCCEEDED(init))CoUninitialize();
    (void)RegDeleteTreeW(HKEY_CURRENT_USER,input_key);
    if(!typed)return 2;
    std::wcout<<L"LIVE_INPUT_VERIFIED: native EDIT Telex/VNI, punctuation, focus, pass-through\n";
    return 0;
}
