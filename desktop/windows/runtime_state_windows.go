//go:build windows
package main
import ("fmt";"os/exec";"syscall")
// Per-user preference bridge; only Enabled and Scheme DWORDs are published.
// Never store typed contents or input history.
const runtimeRegistryKey = `HKCU\Software\LarxVo\Lakakey\Input`
func runtimeValues(s Settings) [2]string {
 enabled:="0";if s.Vietnamese{enabled="1"}
 scheme:="0";if s.Layout=="VNI"{scheme="1"}
 return [2]string{enabled,scheme}
}
func publishRuntimeMode(s Settings) error {
 values:=runtimeValues(s)
 for i,name:=range [2]string{"Enabled","Scheme"} {
  cmd:=exec.Command("reg.exe","add",runtimeRegistryKey,
     "/v",name,"/t","REG_DWORD","/d",values[i],"/f")
  cmd.SysProcAttr=&syscall.SysProcAttr{HideWindow:true}
  if err:=cmd.Run();err!=nil{return fmt.Errorf("cannot publish %s: %w",name,err)}
 }
 return nil
}
