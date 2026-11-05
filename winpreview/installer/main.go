//go:build windows
// Lakakey Desktop Preview Setup (not an IME) · © 2026 Larx Vo
package main
import (
 "embed"
 "fmt"
 "io"
 "os"
 "os/exec"
 "path/filepath"
 "strings"
 "syscall"
 "unsafe"
)
//go:embed payload/LakakeyPreview.exe payload/mascot.bmp payload/README_PREVIEW.txt
var files embed.FS
var mbox=syscall.NewLazyDLL("user32.dll").NewProc("MessageBoxW")
func msg(text string,flags uintptr)uintptr {a:=syscall.StringToUTF16Ptr(text);b:=syscall.StringToUTF16Ptr("Lakakey Setup — Larx Vo");r,_,_:=mbox.Call(0,uintptr(unsafe.Pointer(a)),uintptr(unsafe.Pointer(b)),flags);return r}
func base()string {return filepath.Join(os.Getenv("LOCALAPPDATA"),"Programs","LakakeyPreview")}
func registerUninstall(dir string){key:=`HKCU\Software\Microsoft\Windows\CurrentVersion\Uninstall\LakakeyDesktopPreview`;fields:=map[string]string{"DisplayName":"Lakakey Desktop Preview","DisplayVersion":"0.8.0-preview","Publisher":"Larx Vo","URLInfoAbout":"https://vovinhlac.com","UninstallString":"\""+filepath.Join(dir,"UninstallLakakeyPreview.exe")+"\" --uninstall","InstallLocation":dir};for k,v:=range fields {c:=exec.Command("reg.exe","add",key,"/v",k,"/t","REG_SZ","/d",v,"/f");c.SysProcAttr=&syscall.SysProcAttr{HideWindow:true};_ = c.Run()}}
func uninstall(){if msg("Remove Lakakey Desktop Preview?\nThis does not remove any other Vietnamese input method.",0x24)!=6{return};dir:=base();for _,name:=range []string{"LakakeyPreview.exe","mascot.bmp","README_PREVIEW.txt"}{_ = os.Remove(filepath.Join(dir,name))};key:=`HKCU\Software\Microsoft\Windows\CurrentVersion\Uninstall\LakakeyDesktopPreview`;c:=exec.Command("reg.exe","delete",key,"/f");c.SysProcAttr=&syscall.SysProcAttr{HideWindow:true};_ = c.Run();msg("The preview application has been removed. The uninstaller EXE can be deleted after it exits.",0x40)}
func main(){if len(os.Args)>1&&strings.EqualFold(os.Args[1],"--uninstall"){uninstall();return}
 if msg("Install Lakakey Desktop Preview?\n\nIMPORTANT: This is a working Settings preview ONLY. It cannot type Vietnamese in Chrome, Word, or other applications.\n\nContinue installing the preview?",0x24)!=6{return}
 dir:=base();if e:=os.MkdirAll(dir,0755);e!=nil {msg(fmt.Sprintf("Cannot create install directory: %v",e),0x10);return}
 for _,f:=range []string{"LakakeyPreview.exe","mascot.bmp","README_PREVIEW.txt"}{b,e:=files.ReadFile("payload/"+f);if e!=nil {msg(e.Error(),0x10);return};if e=os.WriteFile(filepath.Join(dir,f),b,0644);e!=nil{msg(e.Error(),0x10);return}}
 self,e:=os.Open(os.Args[0]);if e==nil{dst,e2:=os.Create(filepath.Join(dir,"UninstallLakakeyPreview.exe"));if e2==nil{_,_=io.Copy(dst,self);_ = dst.Close()};_ = self.Close()}
 registerUninstall(dir)
 app:=exec.Command(filepath.Join(dir,"LakakeyPreview.exe"));_ = app.Start()
 msg("Lakakey Desktop Preview installed and launched.\n\nNo Vietnamese system input has been installed.\n\n© Larx Vo — vovinhlac.com",0x40)
}
