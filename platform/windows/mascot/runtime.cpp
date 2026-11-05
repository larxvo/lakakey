// Lakakey Mascot Runtime v0.6 — native, separate process; no keyboard hooks.
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shlobj.h>
#include <gdiplus.h>
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <cwchar>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "shell32.lib")
using namespace Gdiplus;
namespace {
constexpr UINT kTick=WM_APP+12;
constexpr int kW=220,kH=260;
constexpr wchar_t kClass[]=L"LakakeyMascotRuntimeV06";
HWND window=nullptr;
ULONG_PTR gdipToken=0;
std::array<Bitmap*,12> frames{};
std::filesystem::path assetDirectory;
int loadedAction=-1;
constexpr const wchar_t* kActions[]={L"welcome",L"vn",L"en",L"unicode",L"telex",L"vni",L"typing",L"success",L"notice",L"follow",L"sleep",L"error"};
int mode=1; // 0 Full, 1 Minimal, 2 Off
int frame=0, ticksRemaining=0, action=0;
bool following=false;
POINT anchor{};
std::filesystem::path settingsFile(){
 wchar_t path[MAX_PATH]{};
 if(SUCCEEDED(SHGetFolderPathW(nullptr,CSIDL_APPDATA,nullptr,SHGFP_TYPE_CURRENT,path)))
  return std::filesystem::path(path)/L"Lakakey"/L"settings.ini";
 return L"settings.ini";
}
void readMode(){
 std::wifstream file(settingsFile()); std::wstring line;
 while(std::getline(file,line)) if(line.rfind(L"mascot_mode=",0)==0)
  mode=std::clamp(_wtoi(line.c_str()+12),0,2);
}
void position(){
 POINT p{};GetCursorPos(&p);
 HMONITOR mon=MonitorFromPoint(p,MONITOR_DEFAULTTONEAREST);
 MONITORINFO info{};info.cbSize=sizeof(info);GetMonitorInfoW(mon,&info);
 anchor={info.rcWork.right-kW-28,info.rcWork.bottom-kH-18};
}
bool loadAction(int event){
 int index=event-1;
 if(index==loadedAction)return true;
 for(auto*& bitmap:frames){delete bitmap;bitmap=nullptr;}
 loadedAction=-1;
 for(int i=0;i<12;i++){
  wchar_t name[64]{};swprintf_s(name,L"%s_%02d.png",kActions[index],i);
  frames[i]=Bitmap::FromFile((assetDirectory/name).c_str(),FALSE);
  if(!frames[i]||frames[i]->GetLastStatus()!=Ok)return false;
 }
 loadedAction=index;return true;
}
void render(){
 if(!window||mode==2||!frames[0])return;
 POINT cursor{};GetCursorPos(&cursor);
 int offsetX=0,offsetY=0;
 if(following&&mode==0){
  offsetX=std::clamp((cursor.x-(anchor.x+kW/2))/25,-38,38);
  offsetY=std::clamp((cursor.y-(anchor.y+kH/2))/35,-22,22);
 }
 HDC screen=GetDC(nullptr),memory=CreateCompatibleDC(screen);
 BITMAPINFO bmi{};bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
 bmi.bmiHeader.biWidth=kW;bmi.bmiHeader.biHeight=-kH;
 bmi.bmiHeader.biPlanes=1;bmi.bmiHeader.biBitCount=32;
 bmi.bmiHeader.biCompression=BI_RGB;
 void* pixels=nullptr;
 HBITMAP dib=CreateDIBSection(screen,&bmi,DIB_RGB_COLORS,&pixels,nullptr,0);
 if(!dib){DeleteDC(memory);ReleaseDC(nullptr,screen);return;}
 HGDIOBJ previous=SelectObject(memory,dib);
 {
  Graphics graphics(memory);
  graphics.SetCompositingMode(CompositingModeSourceCopy);
  graphics.Clear(Color(0,0,0,0));
  graphics.SetCompositingMode(CompositingModeSourceOver);
  graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
  if(frames[frame]) graphics.DrawImage(frames[frame],Rect(0,0,kW,kH));
 }
 POINT source{0,0},destination{anchor.x+offsetX,anchor.y+offsetY};
 SIZE size{kW,kH};BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
 UpdateLayeredWindow(window,screen,&destination,&size,memory,&source,0,&blend,ULW_ALPHA);
 SelectObject(memory,previous);DeleteObject(dib);DeleteDC(memory);ReleaseDC(nullptr,screen);
}
// Event IDs: 1 welcome, 2 VN, 3 EN, 4 Unicode, 5 Telex, 6 VNI,
// 7 typing, 8 success, 9 notice, 10 follow, 11 sleep, 12 error.
bool essential(int event){return event==1||event==2||event==3||event==8||event==9||event==12;}
void play(int event){
 readMode();
 if(mode==2){ShowWindow(window,SW_HIDE);ticksRemaining=0;return;}
 if(mode==1&&!essential(event)){ShowWindow(window,SW_HIDE);ticksRemaining=0;return;}
 if(!loadAction(event))return;
 following=(mode==0&&event==10);
 action=event;
 frame=0;
 ticksRemaining=(mode==1?16:32); // 1.28s / 2.56s
 position();render();ShowWindow(window,SW_SHOWNOACTIVATE);
}
LRESULT CALLBACK windowProc(HWND hwnd,UINT message,WPARAM wp,LPARAM lp){
 switch(message){
 case WM_CREATE:{
  window=hwnd;
  GdiplusStartupInput input{};if(GdiplusStartup(&gdipToken,&input,nullptr)!=Ok)return -1;
  wchar_t module[MAX_PATH]{};GetModuleFileNameW(nullptr,module,MAX_PATH);
  assetDirectory=std::filesystem::path(module).parent_path()/L"assets";
  readMode();position();SetTimer(hwnd,1,80,nullptr);return 0;
 }
 case WM_COPYDATA:{
  auto* data=reinterpret_cast<COPYDATASTRUCT*>(lp);
  if(data&&data->dwData==0x4C4B4D35&&data->cbData==sizeof(DWORD)&&data->lpData){
   DWORD event=*reinterpret_cast<DWORD*>(data->lpData);
   if(event==0){readMode();ShowWindow(hwnd,SW_HIDE);ticksRemaining=0;}
   else if(event<=12)play(static_cast<int>(event));
   return TRUE;
  }return FALSE;
 }
 case WM_TIMER:{
  if(wp!=1)return 0;
  static int configTicks=0;
  if(++configTicks>=12){configTicks=0;int previous=mode;readMode();if(mode==2&&previous!=2){ShowWindow(hwnd,SW_HIDE);ticksRemaining=0;}}
  if(ticksRemaining>0){
   --ticksRemaining;
   frame=(frame+1)%12;
   render();
   if(ticksRemaining==0){ShowWindow(hwnd,SW_HIDE);following=false;}
  }
  return 0;
 }
 case WM_DISPLAYCHANGE:position();return 0;
 case WM_DESTROY:{
  KillTimer(hwnd,1);
  for(auto*& bitmap:frames){delete bitmap;bitmap=nullptr;}
  if(gdipToken)GdiplusShutdown(gdipToken);
  PostQuitMessage(0);return 0;
 }
 }
 return DefWindowProcW(hwnd,message,wp,lp);
}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,LPWSTR,int){
 if(FindWindowW(kClass,nullptr))return 0;
 WNDCLASSW wc{};wc.hInstance=instance;wc.lpfnWndProc=windowProc;wc.lpszClassName=kClass;
 if(!RegisterClassW(&wc))return 1;
 HWND hwnd=CreateWindowExW(WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,
  kClass,L"Lakakey Mascot",WS_POPUP,0,0,kW,kH,nullptr,nullptr,instance,nullptr);
 if(!hwnd)return 2;
 // Deliberately never call ShowWindow on startup: no intrusive startup animation.
 MSG message{};while(GetMessageW(&message,nullptr,0,0)>0){TranslateMessage(&message);DispatchMessageW(&message);}
 return static_cast<int>(message.wParam);
}
