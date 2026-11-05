//go:build windows
// Lakakey Desktop PREVIEW: Settings demonstration, NOT a system Vietnamese IME.
// © 2026 Larx Vo — https://vovinhlac.com
package main

import (
 "fmt"
 "os"
 "path/filepath"
 "strings"
 "syscall"
 "unsafe"
)

var (
 user32 = syscall.NewLazyDLL("user32.dll")
 kernel32 = syscall.NewLazyDLL("kernel32.dll")
 shell32 = syscall.NewLazyDLL("shell32.dll")
 gdi32 = syscall.NewLazyDLL("gdi32.dll")
 pRegisterClassExW = user32.NewProc("RegisterClassExW")
 pCreateWindowExW = user32.NewProc("CreateWindowExW")
 pDefWindowProcW = user32.NewProc("DefWindowProcW")
 pGetMessageW = user32.NewProc("GetMessageW")
 pTranslateMessage = user32.NewProc("TranslateMessage")
 pDispatchMessageW = user32.NewProc("DispatchMessageW")
 pShowWindow = user32.NewProc("ShowWindow")
 pUpdateWindow = user32.NewProc("UpdateWindow")
 pPostQuitMessage = user32.NewProc("PostQuitMessage")
 pSendMessageW = user32.NewProc("SendMessageW")
 pSetWindowTextW = user32.NewProc("SetWindowTextW")
 pMessageBoxW = user32.NewProc("MessageBoxW")
 pLoadCursorW = user32.NewProc("LoadCursorW")
 pLoadImageW = user32.NewProc("LoadImageW")
 pFillRect = user32.NewProc("FillRect")
 pGetClientRect = user32.NewProc("GetClientRect")
 pSetTextColor = gdi32.NewProc("SetTextColor")
 pSetBkMode = gdi32.NewProc("SetBkMode")
 pCreateSolidBrush = gdi32.NewProc("CreateSolidBrush")
 pCreateFontW = gdi32.NewProc("CreateFontW")
 pDeleteObject = gdi32.NewProc("DeleteObject")
 pShellExecuteW = shell32.NewProc("ShellExecuteW")
 pGetModuleHandleW = kernel32.NewProc("GetModuleHandleW")
)

const (
 WM_CREATE=1; WM_DESTROY=2; WM_COMMAND=0x111; WM_CLOSE=0x10; WM_SETFONT=0x30
 WM_CTLCOLORSTATIC=0x138; WM_ERASEBKGND=0x14; STM_SETIMAGE=0x172
 WS_OVERLAPPEDWINDOW=0x00CF0000; WS_VISIBLE=0x10000000; WS_CHILD=0x40000000
 SS_BITMAP=0x0E; SW_SHOW=5; MB_OK=0; IMAGE_BITMAP=0; LR_LOADFROMFILE=0x10
 ID_VN=201; ID_MODE=202; ID_THEME=203; ID_ABOUT=204
)
type WndClassEx struct { Size uint32; Style uint32; Proc uintptr; ClassExtra int32; WindowExtra int32; Instance uintptr; Icon uintptr; Cursor uintptr; Background uintptr; MenuName *uint16; ClassName *uint16; IconSm uintptr }
type Pt struct {X,Y int32}
type Msg struct { Hwnd uintptr; Message uint32; _ uint32; WParam uintptr; LParam uintptr; Time uint32; Point Pt; Private uint32 }
type Rect struct{ Left,Top,Right,Bottom int32 }
var (
 hwnd, modeButton, vnButton, themeButton, statusLabel, mascotStatic uintptr
 bgBrush, font, imageBitmap uintptr
 mode=1 // 0 Full, 1 Minimal, 2 Off
 color=0
 vn=true
 names=[6]string{"Laka Blue","Lavender","Emerald","Rose","Orange","Graphite"}
)
func wp(s string)*uint16{return syscall.StringToUTF16Ptr(s)}
func setText(h uintptr,s string){pSetWindowTextW.Call(h,uintptr(unsafe.Pointer(wp(s))))}
func send(h uintptr,msg uintptr,a uintptr,b uintptr) uintptr {r,_,_:=pSendMessageW.Call(h,msg,a,b);return r}
func configFile()string{return filepath.Join(os.Getenv("APPDATA"),"Lakakey","preview.ini")}
func save(){_ = os.MkdirAll(filepath.Dir(configFile()),0700);val:=0;if vn{val=1};_ = os.WriteFile(configFile(),[]byte(fmt.Sprintf("mascot_mode=%d\naccent=%d\nvietnamese=%d\n",mode,color,val)),0600)}
func load(){b,e:=os.ReadFile(configFile());if e!=nil{return};for _,line:=range strings.Split(string(b),"\n"){var n int;if _,e=fmt.Sscanf(line,"mascot_mode=%d",&n);e==nil&&n>=0&&n<=2{mode=n};if _,e=fmt.Sscanf(line,"accent=%d",&n);e==nil&&n>=0&&n<=5{color=n};if _,e=fmt.Sscanf(line,"vietnamese=%d",&n);e==nil{vn=n!=0}}}
func button(parent uintptr,text string,id int32,x,y,w,h int32)uintptr{return ctrl(parent,"BUTTON",text,0,id,x,y,w,h)}
func label(parent uintptr,text string,x,y,w,h int32)uintptr{return ctrl(parent,"STATIC",text,0,0,x,y,w,h)}
func ctrl(parent uintptr,class,text string,style uint32,id,x,y,w,h int32)uintptr{
 ins,_,_:=pGetModuleHandleW.Call(0)
 handle,_,_:=pCreateWindowExW.Call(0,uintptr(unsafe.Pointer(wp(class))),uintptr(unsafe.Pointer(wp(text))),uintptr(WS_VISIBLE|WS_CHILD|style),uintptr(x),uintptr(y),uintptr(w),uintptr(h),parent,uintptr(id),ins,0)
 if font!=0 {send(handle,WM_SETFONT,font,1)}
 return handle
}
func refresh(){if mascotStatic!=0 {if mode==2 {pShowWindow.Call(mascotStatic,0)}else {pShowWindow.Call(mascotStatic,SW_SHOW)}};modes:=[]string{"Full","Minimal","Off"};setText(modeButton,"Mascot: "+modes[mode]);setText(themeButton,"Accent: "+names[color]);if vn{setText(vnButton,"Vietnamese (preview only)")}else{setText(vnButton,"English (preview only)")};setText(statusLabel,"Settings preview — typing Vietnamese system-wide is not available.");save()}
func wmProc(h,msg,wp,lp uintptr)uintptr{
 switch msg{
 case WM_CREATE:
  hwnd=h
  bgBrush,_,_=pCreateSolidBrush.Call(0xFFFFFF)
  face:=wpStr("Segoe UI")
  font,_,_=pCreateFontW.Call(^uintptr(18)+1,0,0,0,400,0,0,0,0,0,0,0,0,uintptr(unsafe.Pointer(face)))
  label(h,"Lakakey",28,18,440,42)
  label(h,"Windows Desktop Preview  |  © 2026 Larx Vo",28,62,510,28)
  label(h,"This is NOT yet a Vietnamese input method.",28,99,510,28)
  label(h,"Input language (UI demo)",28,152,310,27)
  vnButton=button(h,"",ID_VN,28,182,270,37)
  label(h,"Mascot appearance",28,245,300,24)
  modeButton=button(h,"",ID_MODE,28,273,270,37)
  label(h,"Theme accent",28,330,300,24)
  themeButton=button(h,"",ID_THEME,28,360,270,37)
  statusLabel=label(h,"",28,418,520,32)
  button(h,"About author · vovinhlac.com",ID_ABOUT,28,464,280,38)
  label(h,"Free software concept · No account · No advertising",28,525,530,27)
  mascotStatic=ctrl(h,"STATIC","",SS_BITMAP,0,368,172,182,220)
  exe,_:=os.Executable()
  bmp:=filepath.Join(filepath.Dir(exe),"mascot.bmp")
  imageBitmap,_,_=pLoadImageW.Call(0,uintptr(unsafe.Pointer(wpStr(bmp))),IMAGE_BITMAP,0,0,LR_LOADFROMFILE)
  if imageBitmap!=0{send(mascotStatic,STM_SETIMAGE,IMAGE_BITMAP,imageBitmap)}
  refresh(); return 0
 case WM_COMMAND:
  id:=wp & 0xFFFF
  switch id {case ID_VN:vn=!vn;refresh();case ID_MODE:mode=(mode+1)%3;refresh();case ID_THEME:color=(color+1)%6;refresh();case ID_ABOUT:pShellExecuteW.Call(h,uintptr(unsafe.Pointer(wpStr("open"))),uintptr(unsafe.Pointer(wpStr("https://vovinhlac.com"))),0,0,SW_SHOW)}
  return 0
 case WM_CTLCOLORSTATIC:
  pSetBkMode.Call(wp,1);pSetTextColor.Call(wp,0x28354A)
  return bgBrush
 case WM_ERASEBKGND:
  var r Rect;pGetClientRect.Call(h,uintptr(unsafe.Pointer(&r)));pFillRect.Call(wp,uintptr(unsafe.Pointer(&r)),bgBrush);return 1
 case WM_CLOSE:
  pMessageBoxW.Call(h,uintptr(unsafe.Pointer(wpStr("Lakakey is currently a settings preview, not an IME. Closing preview."))),uintptr(unsafe.Pointer(wpStr("Lakakey Preview"))),MB_OK)
  // let DefWindowProc handle destruction
 case WM_DESTROY:
  if imageBitmap!=0{pDeleteObject.Call(imageBitmap)};if font!=0{pDeleteObject.Call(font)};if bgBrush!=0{pDeleteObject.Call(bgBrush)}
  pPostQuitMessage.Call(0);return 0
 }
 r,_,_:=pDefWindowProcW.Call(h,msg,wp,lp);return r
}
func wpStr(s string)*uint16{return wp(s)}
func main(){
 load()
 ins,_,_:=pGetModuleHandleW.Call(0)
 cn:=wp("LakakeyDesktopPreviewWin32")
 wc:=WndClassEx{Size:uint32(unsafe.Sizeof(WndClassEx{})),Proc:syscall.NewCallback(wmProc),Instance:ins,ClassName:cn}
 wc.Cursor,_,_=pLoadCursorW.Call(0,32512)
 if r,_,_:=pRegisterClassExW.Call(uintptr(unsafe.Pointer(&wc)));r==0 {pMessageBoxW.Call(0,uintptr(unsafe.Pointer(wp("Unable to initialize Windows preview."))),uintptr(unsafe.Pointer(wp("Lakakey"))),0);return}
 window,_,_:=pCreateWindowExW.Call(0,uintptr(unsafe.Pointer(cn)),uintptr(unsafe.Pointer(wp("Lakakey Preview — Created by Larx Vo"))),WS_OVERLAPPEDWINDOW|WS_VISIBLE,150,110,610,635,0,0,ins,0)
 if window==0{return}
 pShowWindow.Call(window,SW_SHOW);pUpdateWindow.Call(window)
 var m Msg
 for {n,_,_:=pGetMessageW.Call(uintptr(unsafe.Pointer(&m)),0,0,0);if int32(n)<=0{break};pTranslateMessage.Call(uintptr(unsafe.Pointer(&m)));pDispatchMessageW.Call(uintptr(unsafe.Pointer(&m)))}
}
