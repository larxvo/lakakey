//go:build windows
package main
import ("syscall";"unsafe")
var moveFileExW=syscall.NewLazyDLL("kernel32.dll").NewProc("MoveFileExW")
func replaceFileAtomic(from,to string) error {
 src,err:=syscall.UTF16PtrFromString(from);if err!=nil{return err}
 dst,err:=syscall.UTF16PtrFromString(to);if err!=nil{return err}
 result,_,callErr:=moveFileExW.Call(uintptr(unsafe.Pointer(src)),uintptr(unsafe.Pointer(dst)),0x1|0x8)
 if result==0{return callErr}
 return nil
}
