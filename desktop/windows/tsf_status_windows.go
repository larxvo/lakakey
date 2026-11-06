//go:build windows

package main

import (
    "syscall"
    "unsafe"
)

const tsfRegistryKey = `Software\Classes\CLSID\{C67C6419-3D0F-4F56-9C4B-9D8A6E74A711}\InprocServer32`
const keyQueryValue = 0x0001

func experimentalTSFRegistered() bool {
    var handle uintptr
    open := advapiRegistry.NewProc("RegOpenKeyExW")
    result, _, _ := open.Call(uintptr(syscall.HKEY_CURRENT_USER),
        uintptr(unsafe.Pointer(syscall.StringToUTF16Ptr(tsfRegistryKey))),
        0, keyQueryValue, uintptr(unsafe.Pointer(&handle)))
    if result != 0 { return false }
    regCloseKey.Call(handle)
    return true
}

func nativeStatusText() string {
    registered := "Chưa đăng ký"
    if experimentalTSFRegistered() {
        registered = "Đã đăng ký thử nghiệm (chưa xác minh đang hoạt động)"
    }
    return "LAKAKEY 0.13  ·  CHẨN ĐOÁN\r\n\r\n" +
        "Ứng dụng cài đặt: Đang chạy\r\n" +
        "Hồ sơ Windows TSF: " + registered + "\r\n\r\n" +
        "Trạng thái VN/EN ở đây là cấu hình ứng dụng.\r\n" +
        "Nhập liệu toàn hệ thống cần kích hoạt và kiểm thử TSF riêng.\r\n\r\n" +
        "Không ghi lại nội dung gõ. Không gửi dữ liệu ra ngoài."
}
