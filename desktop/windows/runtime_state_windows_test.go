//go:build windows
package main
import "testing"
func TestRuntimeModeSerialization(t *testing.T) {
 s:=defaultSettings()
 if got:=runtimeValues(s);got!=[2]string{"1","0"}{t.Fatalf("default bridge: %v",got)}
 s.Vietnamese=false;s.Layout="VNI"
 if got:=runtimeValues(s);got!=[2]string{"0","1"}{t.Fatalf("mode bridge: %v",got)}
}
