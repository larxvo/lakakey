#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cwchar>
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Ole32.lib")

// Native Win32 configuration companion. It is NOT a TSF/IME implementation.
static HWND windowHandle=nullptr;
static HWND modeButton=nullptr;
static HWND styleButton=nullptr;
static HWND colorButtons[6]{};
static HWND schemeBox=nullptr;
static HWND statusLabel=nullptr;
static HWND mascotBox=nullptr;
static int mascotMode=1; // 0 Full, 1 Minimal, 2 Off
static bool vietnamese=true, dark=false;
static int accent=0;
static const COLORREF colors[]={RGB(37,99,235),RGB(139,92,246),RGB(5,150,105),RGB(225,29,112),RGB(234,88,12),RGB(71,85,105)};
static const wchar_t* colorNames[]={L"Laka Blue",L"Lavender",L"Emerald",L"Rose",L"Orange",L"Graphite"};
static HBRUSH bgBrush=nullptr;
static const UINT WM_TRAY=WM_APP+1;
static const UINT TRAY_ID=1;
static std::filesystem::path configPath(){
 wchar_t path[MAX_PATH]{};
 if(SUCCEEDED(SHGetFolderPathW(nullptr,CSIDL_APPDATA,nullptr,SHGFP_TYPE_CURRENT,path))){
  auto dir=std::filesystem::path(path)/L"Lakakey";
  std::error_code ec;std::filesystem::create_directories(dir,ec);
  return dir/L"settings.ini";
 }
 return L"lakakey-settings.ini";
}
static void load(){
 std::wifstream f(configPath());std::wstring line;
 while(std::getline(f,line)){
  if(line.rfind(L"mascot_mode=",0)==0)mascotMode=std::clamp(_wtoi(line.c_str()+12),0,2);
  if(line.rfind(L"accent=",0)==0)accent=std::clamp(_wtoi(line.c_str()+7),0,5);
  if(line.rfind(L"dark=",0)==0)dark=_wtoi(line.c_str()+5)!=0;
  if(line.rfind(L"vietnamese=",0)==0)vietnamese=_wtoi(line.c_str()+11)!=0;
 }
}
static void save(){
 std::wofstream f(configPath(),std::ios::trunc);
 if(f)f<<L"accent="<<accent<<L"\ndark="<<(dark?1:0)<<L"\nvietnamese="<<(vietnamese?1:0)<<L"\nmascot_mode="<<mascotMode<<L"\n";
}
// Optional companion process; mascot is isolated from the input engine.
static constexpr wchar_t MASCOT_CLASS[]=L"LakakeyMascotRuntimeV05";
static HWND mascotWindow(){return FindWindowW(MASCOT_CLASS,nullptr);}
static void startMascot(){
 if(mascotWindow())return;
 wchar_t module[MAX_PATH]{};GetModuleFileNameW(nullptr,module,MAX_PATH);
 auto executable=std::filesystem::path(module).parent_path()/L"LakakeyMascot.exe";
 if(!std::filesystem::exists(executable))return;
 std::wstring command=L"\""+executable.wstring()+L"\"";
 STARTUPINFOW si{};si.cb=sizeof(si);PROCESS_INFORMATION pi{};
 if(CreateProcessW(executable.c_str(),command.data(),nullptr,nullptr,FALSE,
    CREATE_NO_WINDOW,nullptr,nullptr,&si,&pi)){
  CloseHandle(pi.hThread);CloseHandle(pi.hProcess);
 }
}
static void mascotEvent(DWORD event){
 if(mascotMode==2)return;
 startMascot();
 HWND target=mascotWindow();
 if(!target)return; // Never block the settings UI waiting for the mascot.
 COPYDATASTRUCT data{0x4C4B4D35,sizeof(event),&event};
 DWORD_PTR result=0;
 SendMessageTimeoutW(target,WM_COPYDATA,(WPARAM)windowHandle,(LPARAM)&data,
  SMTO_ABORTIFHUNG|SMTO_BLOCK,120,&result);
}
static void refresh(){
 SetWindowTextW(modeButton,vietnamese?L"Vietnamese · Preview":L"English · Preview");
 SetWindowTextW(styleButton,dark?L"Theme: Dark":L"Theme: Light");
 std::wstring status=L"Accent: ";status+=colorNames[accent];status+=L"  |  Windows Settings Preview";
 SetWindowTextW(statusLabel,status.c_str());
 if(mascotBox)SendMessageW(mascotBox,CB_SETCURSEL,mascotMode,0);
 for(int i=0;i<6;i++){
  std::wstring name=(i==accent?L"✓ ":L"");name+=colorNames[i];
  SetWindowTextW(colorButtons[i],name.c_str());
 }
 if(bgBrush)DeleteObject(bgBrush);
 bgBrush=CreateSolidBrush(dark?RGB(24,28,38):RGB(248,250,252));
 InvalidateRect(windowHandle,nullptr,TRUE);save();
}
static void trayAdd(){
 NOTIFYICONDATAW data{};data.cbSize=sizeof(data);data.hWnd=windowHandle;data.uID=TRAY_ID;
 data.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;data.uCallbackMessage=WM_TRAY;
 data.hIcon=LoadIconW(nullptr,IDI_APPLICATION);
 wcscpy_s(data.szTip,L"Lakakey — Larx Vo");Shell_NotifyIconW(NIM_ADD,&data);
}
static void trayRemove(){NOTIFYICONDATAW d{};d.cbSize=sizeof(d);d.hWnd=windowHandle;d.uID=TRAY_ID;Shell_NotifyIconW(NIM_DELETE,&d);}
static HWND makeControl(const wchar_t* klass,const wchar_t* label,DWORD style,int x,int y,int width,int height,int id){
 HWND h=CreateWindowExW(0,klass,label,WS_CHILD|WS_VISIBLE|style,x,y,width,height,windowHandle,(HMENU)(INT_PTR)id,GetModuleHandleW(nullptr),nullptr);
 SendMessageW(h,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),TRUE);return h;
}
static LRESULT CALLBACK proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 switch(msg){
 case WM_CREATE:{
  windowHandle=hwnd;
  makeControl(L"STATIC",L"Lakakey",0,28,18,390,43,101);
  makeControl(L"STATIC",L"Windows Desktop Preview - Input Method NOT implemented",0,30,61,500,25,102);
  makeControl(L"STATIC",L"Language toggle (UI only)",0,30,118,200,24,103);
  modeButton=makeControl(L"BUTTON",L"",BS_PUSHBUTTON,30,149,235,38,201);
  makeControl(L"STATIC",L"Appearance",0,30,215,200,25,104);
  styleButton=makeControl(L"BUTTON",L"",BS_PUSHBUTTON,30,247,235,38,202);
  makeControl(L"STATIC",L"Accent color",0,30,317,200,25,105);
  for(int i=0;i<6;i++)colorButtons[i]=makeControl(L"BUTTON",L"",BS_PUSHBUTTON,30+(i%3)*166,349+(i/3)*49,155,37,210+i);
  makeControl(L"STATIC",L"Mascot animation",0,30,451,200,22,109);
  mascotBox=makeControl(L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_VSCROLL,30,477,240,120,240);
  for(const auto* label:{L"Full — all interactions",L"Minimal — essential only",L"Off — no mascot"})SendMessageW(mascotBox,CB_ADDSTRING,0,(LPARAM)label);
  statusLabel=makeControl(L"STATIC",L"",0,30,522,500,28,106);
  makeControl(L"STATIC",L"Created & developed by Larx Vo  |  © 2026 Larx Vo",0,30,564,500,24,107);
  makeControl(L"BUTTON",L"vovinhlac.com  ↗",BS_PUSHBUTTON,30,594,210,36,230);
  makeControl(L"STATIC",L"Free software. No account. No advertising.",0,30,640,500,22,108);
  trayAdd();refresh();return 0;
 }
 case WM_COMMAND:{
  int id=LOWORD(wp);
  if(id==240){if(HIWORD(wp)==CBN_SELCHANGE)mascotMode=(int)SendMessageW(mascotBox,CB_GETCURSEL,0,0);else return 0;}
  else if(id==201)vietnamese=!vietnamese;
  else if(id==202)dark=!dark;
  else if(id>=210&&id<216)accent=id-210;
  else if(id==230){ShellExecuteW(hwnd,L"open",L"https://vovinhlac.com",nullptr,nullptr,SW_SHOWNORMAL);return 0;}
  else if(id==301){ShowWindow(hwnd,SW_SHOW);SetForegroundWindow(hwnd);return 0;}
  else if(id==302){vietnamese=!vietnamese;}
  else if(id==303){DestroyWindow(hwnd);return 0;}
  else return 0;
  refresh();
  if(id==240){if(mascotMode!=2)mascotEvent(1);else if(mascotWindow()){
    DWORD zero=0;COPYDATASTRUCT data{0x4C4B4D35,sizeof(zero),&zero};
    DWORD_PTR result=0;SendMessageTimeoutW(mascotWindow(),WM_COPYDATA,(WPARAM)hwnd,
      (LPARAM)&data,SMTO_ABORTIFHUNG,120,&result);
  }}
  if(id==201||id==302)mascotEvent(vietnamese?2:3);
  return 0;
 }
 case WM_CTLCOLORSTATIC:{
  HDC dc=(HDC)wp;SetBkMode(dc,TRANSPARENT);
  SetTextColor(dc,dark?RGB(237,241,248):RGB(35,43,58));return (LRESULT)bgBrush;
 }
 case WM_ERASEBKGND:{RECT r;GetClientRect(hwnd,&r);FillRect((HDC)wp,&r,bgBrush);return 1;}
 case WM_CLOSE:ShowWindow(hwnd,SW_HIDE);return 0;
 case WM_TRAY:{
  if(lp==WM_LBUTTONDBLCLK){ShowWindow(hwnd,SW_SHOW);SetForegroundWindow(hwnd);}
  if(lp==WM_RBUTTONUP){
   HMENU menu=CreatePopupMenu();AppendMenuW(menu,MF_STRING,301,L"Open Settings");
   AppendMenuW(menu,MF_STRING,302,vietnamese?L"Switch to English (preview)":L"Switch to Vietnamese (preview)");
   AppendMenuW(menu,MF_SEPARATOR,0,nullptr);AppendMenuW(menu,MF_STRING,303,L"Exit Lakakey Settings");
   POINT pos;GetCursorPos(&pos);SetForegroundWindow(hwnd);
   TrackPopupMenu(menu,TPM_RIGHTBUTTON,pos.x,pos.y,0,hwnd,nullptr);DestroyMenu(menu);
  }
  return 0;
 }
 case WM_DESTROY:trayRemove();if(bgBrush)DeleteObject(bgBrush);PostQuitMessage(0);return 0;
 }
 return DefWindowProcW(hwnd,msg,wp,lp);
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,LPWSTR,int show){
 load();WNDCLASSW wc{};wc.lpfnWndProc=proc;wc.hInstance=instance;wc.lpszClassName=L"LakakeySettingsV05";
 wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);RegisterClassW(&wc);
 HWND hwnd=CreateWindowExW(0,wc.lpszClassName,L"Lakakey Preview (not an IME) | Larx Vo",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE,
 CW_USEDEFAULT,CW_USEDEFAULT,550,750,nullptr,nullptr,instance,nullptr);
 if(!hwnd)return 1;ShowWindow(hwnd,show);MSG msg{};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}return (int)msg.wParam;
}
