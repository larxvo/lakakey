//go:build windows

package main

import (
    "fmt"
    "syscall"
    "unsafe"
)

// Only input mode and scheme are shared. No typed text or process activity is stored.
const runtimeRegistryPath = `Software\LarxVo\Lakakey\Input`
const regSetValue = 0x0002
const regDword = 4

var (
    advapiRegistry = syscall.NewLazyDLL("advapi32.dll")
    regCreateKeyEx = advapiRegistry.NewProc("RegCreateKeyExW")
    regSetValueEx = advapiRegistry.NewProc("RegSetValueExW")
    regCloseKey = advapiRegistry.NewProc("RegCloseKey")
)

func runtimeValues(s Settings) [2]uint32 {
    var enabled, scheme uint32
    if s.Vietnamese { enabled = 1 }
    if s.Layout == "VNI" { scheme = 1 }
    return [2]uint32{enabled, scheme}
}

func publishRuntimeMode(s Settings) error {
    var key uintptr
    hkcu := uintptr(syscall.HKEY_CURRENT_USER)
    result, _, _ := regCreateKeyEx.Call(
        hkcu, uintptr(unsafe.Pointer(syscall.StringToUTF16Ptr(runtimeRegistryPath))),
        0, 0, 0, regSetValue, 0, uintptr(unsafe.Pointer(&key)), 0)
    if result != 0 { return fmt.Errorf("cannot open input preferences: Windows error %d", result) }
    defer regCloseKey.Call(key)
    values := runtimeValues(s)
    for i, name := range [2]string{"Enabled", "Scheme"} {
        v := values[i]
        result, _, _ = regSetValueEx.Call(key,
            uintptr(unsafe.Pointer(syscall.StringToUTF16Ptr(name))),
            0, regDword, uintptr(unsafe.Pointer(&v)), unsafe.Sizeof(v))
        if result != 0 { return fmt.Errorf("cannot update input preference %s: Windows error %d", name, result) }
    }
    return nil
}
