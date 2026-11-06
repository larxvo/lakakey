//go:build windows
package main
import ("os";"path/filepath";"syscall";"time";"unsafe")
var moveFileExW=syscall.NewLazyDLL("kernel32.dll").NewProc("MoveFileExW")
func installFile(target string,contents []byte) error {
 dir:=filepath.Dir(target)
 if err:=os.MkdirAll(dir,0755);err!=nil{return err}
 f,err:=os.CreateTemp(dir,".lakakey-install-*.tmp");if err!=nil{return err}
 tmp:=f.Name();defer os.Remove(tmp)
 if _,err=f.Write(contents);err!=nil{f.Close();return err}
 if err=f.Sync();err!=nil{f.Close();return err}
 if err=f.Close();err!=nil{return err}
 src,err:=syscall.UTF16PtrFromString(tmp);if err!=nil{return err}
 dst,err:=syscall.UTF16PtrFromString(target);if err!=nil{return err}
 var lastErr error
 for i:=0;i<20;i++ {
   result,_,callErr:=moveFileExW.Call(uintptr(unsafe.Pointer(src)),uintptr(unsafe.Pointer(dst)),0x1|0x8)
   if result!=0{return nil}
   lastErr=callErr
   time.Sleep(100*time.Millisecond)
 }
 return lastErr
}
