//go:build windows

// Lakakey Desktop Beta installer, per-user and reversible. No drivers or IME are installed.
// Copyright (c) 2026 Larx Vo — https://vovinhlac.com
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

//go:embed payload/LakakeyDesktop.exe payload/README.txt
var packed embed.FS

var messageBox = syscall.NewLazyDLL("user32.dll").NewProc("MessageBoxW")

const appVersion = "0.14.1"
const regUninstall = `HKCU\Software\Microsoft\Windows\CurrentVersion\Uninstall\LakakeyDesktopBeta`
const regStartup = `HKCU\Software\Microsoft\Windows\CurrentVersion\Run`

func tell(text string, flags uintptr) uintptr {
	x := syscall.StringToUTF16Ptr(text)
	y := syscall.StringToUTF16Ptr("Lakakey Setup — Larx Vo")
	r, _, _ := messageBox.Call(0, uintptr(unsafe.Pointer(x)), uintptr(unsafe.Pointer(y)), flags)
	return r
}
func programDir() string { return filepath.Join(os.Getenv("LOCALAPPDATA"), "Programs", "Lakakey") }
func startDir() string {
	return filepath.Join(os.Getenv("APPDATA"), "Microsoft", "Windows", "Start Menu", "Programs")
}
func runHidden(name string, args ...string) error {
	c := exec.Command(name, args...)
	c.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}
	return c.Run()
}
func setReg(name, value string) error {
	return runHidden("reg.exe", "add", regUninstall, "/v", name, "/t", "REG_SZ", "/d", value, "/f")
}
func shortcutPath() string { return filepath.Join(startDir(), "Lakakey.lnk") }
func shortcut(target string) error {
	// Arguments are escaped for the single-quoted PowerShell string literal.
	quote := func(s string) string { return "'" + strings.ReplaceAll(s, "'", "''") + "'" }
	script := "$s=(New-Object -ComObject WScript.Shell).CreateShortcut(" + quote(shortcutPath()) + ");$s.TargetPath=" + quote(target) + ";$s.WorkingDirectory=" + quote(filepath.Dir(target)) + ";$s.Description='Lakakey Desktop Beta';$s.Save()"
	return runHidden("powershell.exe", "-NoProfile", "-NonInteractive", "-Command", script)
}
func install() {
	dir := programDir()
	if _, err := os.Stat(filepath.Join(dir, "LakakeyDesktop.exe")); err == nil {
		// Ask the previous desktop process to quit gracefully before replacing it.
		_ = runHidden(filepath.Join(dir, "LakakeyDesktop.exe"), "--quit")
	}
	if err := os.MkdirAll(dir, 0755); err != nil {
		tell("Không thể tạo thư mục: "+err.Error(), 0x10)
		return
	}
	// Stage each file before atomic replacement. Update the EXE last.
	for _,name:=range []string{"README.txt","LakakeyDesktop.exe"} {
		data,err:=packed.ReadFile("payload/"+name)
		if err!=nil {tell("Bộ cài thiếu tệp: "+name,0x10);return}
		if err=installFile(filepath.Join(dir,name),data);err!=nil {
			tell("Không thể cập nhật: "+name+"\n"+err.Error(),0x10);return
		}
	}

	self, err := os.Open(os.Args[0])
	if err != nil {
		tell("Không thể tạo trình gỡ cài đặt.", 0x10)
		return
	}
	dst, err := os.Create(filepath.Join(dir, "UninstallLakakey.exe"))
	if err != nil {
		_ = self.Close()
		tell(err.Error(), 0x10)
		return
	}
	_, copyErr := io.Copy(dst, self)
	_ = dst.Close()
	_ = self.Close()
	if copyErr != nil {
		tell(copyErr.Error(), 0x10)
		return
	}
	uninstallCmd := "\"" + filepath.Join(dir, "UninstallLakakey.exe") + "\" --uninstall"
	entries := map[string]string{
		"DisplayName": "Lakakey", "DisplayVersion": appVersion,
		"Publisher": "Larx Vo", "URLInfoAbout": "https://vovinhlac.com",
		"InstallLocation": dir, "UninstallString": uninstallCmd,
		"DisplayIcon": filepath.Join(dir, "LakakeyDesktop.exe"),
	}
	for k, v := range entries {
		if e := setReg(k, v); e != nil {
			tell("Đã chép ứng dụng nhưng không tạo đủ uninstall registry: "+e.Error(), 0x30)
		}
	}
	if err := shortcut(filepath.Join(dir, "LakakeyDesktop.exe")); err != nil {
		tell("Đã cài chương trình nhưng không tạo được Start Menu shortcut.\nBạn vẫn có thể mở bằng file trong thư mục: "+dir, 0x30)
	}
	app := exec.Command(filepath.Join(dir, "LakakeyDesktop.exe"))
	app.SysProcAttr = &syscall.SysProcAttr{HideWindow: false}
	if err := app.Start(); err != nil {
		tell("Đã cài, nhưng không khởi động được ứng dụng: "+err.Error(), 0x30)
		return
	}
	// Successful install should be quiet. The application opens itself.
}
func uninstall() {
	// System Settings already confirmed removal. Avoid another blocking dialog.
	dir := programDir()
	if _, err := os.Stat(filepath.Join(dir, "LakakeyDesktop.exe")); err == nil {
		_ = runHidden(filepath.Join(dir, "LakakeyDesktop.exe"), "--quit")
	}
	// Prefer clean exit via tray before removal. Fail clearly for in-use files.
	for _, name := range []string{"LakakeyDesktop.exe", "README.txt"} {
		err := os.Remove(filepath.Join(dir, name))
		if err != nil && !os.IsNotExist(err) {
			tell("Vui lòng thoát Lakakey ở system tray trước khi gỡ.\n"+err.Error(), 0x30)
			return
		}
	}
	_ = os.Remove(shortcutPath())
	_ = runHidden("reg.exe", "delete", regUninstall, "/f")
	_ = runHidden("reg.exe", "delete", regStartup, "/v", "Lakakey", "/f")
	// No completion popup.
}
func main() {
	if len(os.Args) > 1 && strings.EqualFold(os.Args[1], "--uninstall") {
		uninstall()
		return
	}
	if os.Getenv("LOCALAPPDATA") == "" || os.Getenv("APPDATA") == "" {
		tell(fmt.Sprintf("Thiếu thư mục ứng dụng cho tài khoản Windows."), 0x10)
		return
	}
	install()
}
