//go:build windows

// Lakakey native Windows control panel and tray.
// Copyright (c) 2026 Larx Vo — https://vovinhlac.com
package main

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"syscall"
	"unsafe"
)

var (
	user32              = syscall.NewLazyDLL("user32.dll")
	shell32             = syscall.NewLazyDLL("shell32.dll")
	kernel32            = syscall.NewLazyDLL("kernel32.dll")
	gdi32               = syscall.NewLazyDLL("gdi32.dll")
	registerClass       = user32.NewProc("RegisterClassExW")
	createWindow        = user32.NewProc("CreateWindowExW")
	defWindow           = user32.NewProc("DefWindowProcW")
	getMessage          = user32.NewProc("GetMessageW")
	translateMessage    = user32.NewProc("TranslateMessage")
	dispatchMessage     = user32.NewProc("DispatchMessageW")
	sendMessage         = user32.NewProc("SendMessageW")
	showWindow          = user32.NewProc("ShowWindow")
	setWindowText       = user32.NewProc("SetWindowTextW")
	setWindowPos        = user32.NewProc("SetWindowPos")
	setForegroundWindow = user32.NewProc("SetForegroundWindow")
	postMessage         = user32.NewProc("PostMessageW")
	postQuitMessage     = user32.NewProc("PostQuitMessage")
	createPopup         = user32.NewProc("CreatePopupMenu")
	appendMenu          = user32.NewProc("AppendMenuW")
	trackPopup          = user32.NewProc("TrackPopupMenu")
	destroyMenu         = user32.NewProc("DestroyMenu")
	getCursorPos        = user32.NewProc("GetCursorPos")
	loadCursor          = user32.NewProc("LoadCursorW")
	setProcessDpiAware  = user32.NewProc("SetProcessDPIAware")
	findWindow          = user32.NewProc("FindWindowW")
	registerWindowMsg   = user32.NewProc("RegisterWindowMessageW")
	registerHotKey      = user32.NewProc("RegisterHotKey")
	setTimer            = user32.NewProc("SetTimer")
	killTimer           = user32.NewProc("KillTimer")
	unregisterHotKey    = user32.NewProc("UnregisterHotKey")
	createMutex         = kernel32.NewProc("CreateMutexW")
	getLastError        = kernel32.NewProc("GetLastError")
	enableWindow        = user32.NewProc("EnableWindow")
	notifyIcon          = shell32.NewProc("Shell_NotifyIconW")
	createIcon          = user32.NewProc("CreateIcon")
	destroyIcon         = user32.NewProc("DestroyIcon")
	getModuleHandle     = kernel32.NewProc("GetModuleHandleW")
	createFont          = gdi32.NewProc("CreateFontW")
	deleteObject        = gdi32.NewProc("DeleteObject")
)

const (
	WS_VISIBLE           = 0x10000000
	WS_CHILD             = 0x40000000
	WS_OVERLAPPED        = 0x00CF0000
	WS_TABSTOP           = 0x00010000
	WS_GROUP             = 0x00020000
	BS_GROUPBOX          = 7
	BS_AUTORADIOBUTTON   = 9
	BS_AUTOCHECKBOX      = 3
	CBS_DROPDOWNLIST     = 3
	WS_VSCROLL           = 0x00200000
	WM_CREATE            = 1
	WM_DESTROY           = 2
	WM_CLOSE             = 0x10
	WM_COMMAND           = 0x111
	WM_SETFONT           = 0x30
	WM_APP               = 0x8000
	WM_LBUTTONUP         = 0x202
	WM_RBUTTONUP         = 0x205
	WM_LBUTTONDBLCLK     = 0x203
	WM_NULL              = 0
	WM_QUERYENDSESSION   = 0x11
	WM_HOTKEY            = 0x312
	WM_TIMER             = 0x113
	TRAY_CLICK_TIMER     = 0x4C4D
	HOTKEY_TOGGLE        = 0x4C4B
	MOD_ALT              = 0x0001
	VK_Z                 = 0x5A
	WM_SINGLEEXIT        = WM_APP + 21
	CB_ADDSTRING         = 0x143
	CB_SETCURSEL         = 0x14E
	CB_GETCURSEL         = 0x147
	BM_SETCHECK          = 0xF1
	BM_GETCHECK          = 0xF0
	BST_CHECKED          = 1
	SW_HIDE              = 0
	SW_SHOW              = 5
	SW_RESTORE           = 9
	SWP_NOMOVE           = 2
	SWP_NOZORDER         = 4
	SWP_NOACTIVATE       = 0x10
	NIM_ADD              = 0
	NIM_MODIFY           = 1
	NIM_DELETE           = 2
	NIF_MESSAGE          = 1
	NIF_ICON             = 2
	NIF_TIP              = 4
	TPM_RETURNCMD        = 0x100
	TPM_RIGHTBUTTON      = 2
	MF_STRING            = 0
	MF_SEPARATOR         = 0x800
	ID_ENCODING          = 101
	ID_LAYOUT            = 102
	ID_HOTKEY1           = 103
	ID_HOTKEY2           = 104
	ID_HIDE              = 105
	ID_EXIT              = 106
	ID_EXPAND            = 107
	ID_ABOUT             = 108
	ID_DEFAULT           = 109
	ID_GUIDE             = 110
	ID_VN                = 111
	ID_MASCOT            = 112
	ID_DIAGNOSTICS       = 113
	ID_SPELL             = 201
	ID_FREE              = 202
	ID_TONE              = 203
	ID_SHORTHAND         = 204
	ID_SHORTHAND_EN      = 205
	ID_SHOWSTART         = 206
	ID_STARTWIN          = 207
	ID_PERAPP            = 208
	TRAY_MESSAGE         = WM_APP + 7
	TRAY_UID             = 100
	ERROR_ALREADY_EXISTS = 183
)

type wndClass struct {
	Size, Style                        uint32
	WndProc                            uintptr
	ClassExtra, WindowExtra            int32
	Instance, Icon, Cursor, Background uintptr
	MenuName, ClassName                *uint16
	IconSm                             uintptr
}
type point struct{ X, Y int32 }
type winMsg struct {
	Hwnd           uintptr
	Message        uint32
	_              uint32
	WParam, LParam uintptr
	Time           uint32
	Pt             point
	Private        uint32
}
type notifyIconData struct {
	CbSize                      uint32
	_                           uint32
	Hwnd                        uintptr
	UID, Flags, CallbackMessage uint32
	_                           uint32
	HIcon                       uintptr
	Tip                         [128]uint16
	State, StateMask            uint32
	Info                        [256]uint16
	Timeout                     uint32
	InfoTitle                   [64]uint16
	InfoFlags                   uint32
	GUID                        [16]byte
	BalloonIcon                 uintptr
}

var (
	cfg                 Settings
	hwnd, font, appIcon uintptr
	handles             = map[uint16]uintptr{}
	expandedHandles     []uintptr
	controlsReady       bool
	quitRequested       bool
	taskbarCreated      uintptr
	instanceMutex       uintptr
	statusLabel         uintptr
	detailLabel         uintptr
	detailMode          string
	hotkeyRegistered    bool
	pendingTrayClick    bool
)

func wide(s string) *uint16   { return syscall.StringToUTF16Ptr(s) }
func strPtr(s string) uintptr { return uintptr(unsafe.Pointer(wide(s))) }
func callSend(h uintptr, msg uintptr, w, l uintptr) uintptr {
	r, _, _ := sendMessage.Call(h, msg, w, l)
	return r
}
func setText(h uintptr, text string) {
	if h != 0 {
		setWindowText.Call(h, strPtr(text))
	}
}
func clientControl(class, text string, style uint32, id uint16, x, y, w, h int32) uintptr {
	inst, _, _ := getModuleHandle.Call(0)
	handle, _, _ := createWindow.Call(0, strPtr(class), strPtr(text), uintptr(WS_CHILD|WS_VISIBLE|style), uintptr(x), uintptr(y), uintptr(w), uintptr(h), hwnd, uintptr(id), inst, 0)
	if handle == 0 {
		return 0
	}
	if font != 0 {
		callSend(handle, WM_SETFONT, font, 1)
	}
	if id != 0 {
		handles[id] = handle
	}
	return handle
}
func label(t string, x, y, w int32) uintptr { return clientControl("STATIC", t, 0, 0, x, y, w, 24) }
func group(t string, x, y, w, h int32) uintptr {
	return clientControl("BUTTON", t, BS_GROUPBOX, 0, x, y, w, h)
}
func button(t string, id uint16, x, y, w int32) uintptr {
	return clientControl("BUTTON", t, WS_TABSTOP, id, x, y, w, 30)
}
func check(t string, id uint16, x, y, w int32) uintptr {
	return clientControl("BUTTON", t, WS_TABSTOP|BS_AUTOCHECKBOX, id, x, y, w, 24)
}
func radio(t string, id uint16, x, y, w int32, first bool) uintptr {
	var s uint32 = WS_TABSTOP | BS_AUTORADIOBUTTON
	if first {
		s |= WS_GROUP
	}
	return clientControl("BUTTON", t, s, id, x, y, w, 24)
}
func combo(id uint16, x, y, w int32, values []string) uintptr {
	h := clientControl("COMBOBOX", "", WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWNLIST, id, x, y, w, 320)
	for _, v := range values {
		callSend(h, CB_ADDSTRING, 0, strPtr(v))
	}
	return h
}
func idx(list []string, item string) uintptr {
	for n, s := range list {
		if s == item {
			return uintptr(n)
		}
	}
	return 0
}
func readCombo(id uint16, list []string) string {
	n := callSend(handles[id], CB_GETCURSEL, 0, 0)
	if n >= uintptr(len(list)) {
		return ""
	}
	return list[n]
}
func checked(id uint16) bool { return callSend(handles[id], BM_GETCHECK, 0, 0) == BST_CHECKED }
func mark(id uint16, on bool) {
	var v uintptr
	if on {
		v = BST_CHECKED
	}
	callSend(handles[id], BM_SETCHECK, v, 0)
}

func createControls() {
	group("Điều khiển", 16, 12, 407, 171)
	label("Bảng mã", 30, 48, 91)
	combo(ID_ENCODING, 122, 43, 285, encodings)
	label("Kiểu gõ", 30, 83, 91)
	combo(ID_LAYOUT, 122, 78, 285, inputLayouts)
	label("Phím chuyển", 30, 128, 91)
	radio("Ctrl + Shift", ID_HOTKEY1, 124, 124, 137, true)
	radio("Alt + Z", ID_HOTKEY2, 270, 124, 130, false)
	button("Đóng", ID_HIDE, 439, 28, 119)
	button("Kết thúc", ID_EXIT, 439, 68, 119)
	button("Mở rộng  ▾", ID_EXPAND, 439, 108, 119)
	// These explain the actual implementation stage, rather than mimicking a system IME.
	statusLabel = label("Lakakey · Thiết lập và điều khiển", 18, 192, 535)
	button("Hướng dẫn", ID_GUIDE, 16, 226, 105)
	button("Thông tin", ID_ABOUT, 130, 226, 105)
	button("Mặc định", ID_DEFAULT, 244, 226, 105)
	button("VN / EN", ID_VN, 359, 226, 200)

	expandedHandles = append(expandedHandles, group("Tùy chọn khác", 16, 279, 355, 138))
	expandedHandles = append(expandedHandles, check("Cho phép gõ tự do", ID_FREE, 28, 309, 305))
	expandedHandles = append(expandedHandles, check("Đặt dấu òa, úy (kiểu mới)", ID_TONE, 28, 336, 305))
	expandedHandles = append(expandedHandles, check("Kiểm tra chính tả", ID_SPELL, 28, 363, 318))
	expandedHandles = append(expandedHandles, group("Tùy chọn gõ tắt", 16, 428, 355, 108))
	expandedHandles = append(expandedHandles, check("Cho phép gõ tắt", ID_SHORTHAND, 28, 458, 320))
	expandedHandles = append(expandedHandles, check("Cho phép gõ tắt khi ở chế độ EN", ID_SHORTHAND_EN, 28, 484, 330))
	expandedHandles = append(expandedHandles, group("Hệ thống", 383, 279, 176, 257))
	expandedHandles = append(expandedHandles, check("Hiện khi khởi động", ID_SHOWSTART, 396, 311, 161))
	expandedHandles = append(expandedHandles, check("Cùng Windows", ID_STARTWIN, 396, 341, 153))
	expandedHandles = append(expandedHandles, check("Theo từng ứng dụng", ID_PERAPP, 396, 373, 163))
	expandedHandles = append(expandedHandles, label("Mascot", 396, 411, 154))
	expandedHandles = append(expandedHandles, combo(ID_MASCOT, 396, 439, 145, []string{"Off", "Minimal", "Full"}))
	expandedHandles = append(expandedHandles, button("Trạng thái native", ID_DIAGNOSTICS, 396, 480, 145))
	// Inline guide / about: no disruptive modal dialogs.
	detailLabel = clientControl("STATIC", "", 0, 0, 27, 303, 518, 212)
	// Hide unfinished controls instead of offering options that have no effect.
	for _, id := range []uint16{ID_HOTKEY1, ID_FREE, ID_TONE, ID_SPELL, ID_SHORTHAND, ID_SHORTHAND_EN, ID_PERAPP, ID_MASCOT} {
		enableWindow.Call(handles[id], 0)
	}
	controlsReady = true
	refresh()
}
func refresh() {
	if !controlsReady {
		return
	}
	callSend(handles[ID_LAYOUT], CB_SETCURSEL, idx(inputLayouts, cfg.Layout), 0)
	callSend(handles[ID_ENCODING], CB_SETCURSEL, idx(encodings, cfg.Encoding), 0)
	callSend(handles[ID_MASCOT], CB_SETCURSEL, idx([]string{"Off", "Minimal", "Full"}, cfg.Mascot), 0)
	mark(ID_HOTKEY1, cfg.ToggleKey == "Ctrl+Shift")
	mark(ID_HOTKEY2, cfg.ToggleKey == "Alt+Z")
	for _, pair := range []struct {
		id uint16
		v  bool
	}{{ID_FREE, cfg.FreeTyping}, {ID_TONE, cfg.ModernTone}, {ID_SPELL, cfg.Spellcheck}, {ID_SHORTHAND, cfg.Shorthand}, {ID_SHORTHAND_EN, cfg.ShorthandEnglish}, {ID_SHOWSTART, cfg.ShowAtStartup}, {ID_STARTWIN, cfg.StartWithWindows}, {ID_PERAPP, cfg.PerAppMode}} {
		mark(pair.id, pair.v)
	}
	if cfg.Vietnamese {
		setText(handles[ID_VN], "Chế độ: VN")
	} else {
		setText(handles[ID_VN], "Chế độ: EN")
	}
	if cfg.Expanded {
		if detailMode != "" {
			setText(handles[ID_EXPAND], "Tùy chọn  ▴")
		} else {
			setText(handles[ID_EXPAND], "Thu nhỏ  ▴")
		}
	} else {
		setText(handles[ID_EXPAND], "Mở rộng  ▾")
	}
	for _, h := range expandedHandles {
		if cfg.Expanded && detailMode == "" {
			showWindow.Call(h, SW_SHOW)
		} else {
			showWindow.Call(h, SW_HIDE)
		}
	}
	if detailLabel != 0 {
		if cfg.Expanded && detailMode != "" {
			showWindow.Call(detailLabel, SW_SHOW)
		} else {
			showWindow.Call(detailLabel, SW_HIDE)
		}
	}
	height := int32(320)
	if cfg.Expanded {
		height = 590
	}
	setWindowPos.Call(hwnd, 0, 0, 0, 592, uintptr(height), SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE)
	if appIcon != 0 {
		updateTray()
	}
}
func save() {
	if err := writeSettings(cfg); err != nil {
		setText(statusLabel, "Không lưu được cấu hình: "+err.Error())
		return
	}
	if err := publishRuntimeMode(cfg); err != nil {
		setText(statusLabel, "Chưa đồng bộ được chế độ nhập liệu")
	}
}
func startupRegistration(on bool) {
	exe, err := os.Executable()
	if err != nil {
		setText(statusLabel, "Không xác định được đường dẫn ứng dụng")
		return
	}
	exe, _ = filepath.Abs(exe)
	const key = `HKCU\Software\Microsoft\Windows\CurrentVersion\Run`
	var cmd *exec.Cmd
	if on {
		mode := "--tray"
		if cfg.ShowAtStartup {mode="--show"}
		cmd = exec.Command("reg.exe", "add", key, "/v", "Lakakey", "/t", "REG_SZ", "/d", fmt.Sprintf("\"%s\" %s",exe,mode), "/f")
	} else {
		cmd = exec.Command("reg.exe", "delete", key, "/v", "Lakakey", "/f")
	}
	cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}
	if err := cmd.Run(); err != nil {
		if !on {
			return
		} // deleting an absent startup value is harmless
		cfg.StartWithWindows = !on
		setText(statusLabel, "Chưa thể cập nhật khởi động cùng Windows")
	}
}
func onCommand(wp uintptr) {
	id := uint16(wp & 0xffff)
	changed := true
	switch id {
	case ID_HIDE:
		showWindow.Call(hwnd, SW_HIDE)
		changed = false
	case ID_EXIT:
		quitRequested = true
		user32.NewProc("DestroyWindow").Call(hwnd)
		changed = false
	case ID_EXPAND:
		if detailMode != "" {
			detailMode = ""
			cfg.Expanded = true
		} else {
			cfg.Expanded = !cfg.Expanded
		}
	case ID_DIAGNOSTICS:
        detailMode = "diagnostics"
        cfg.Expanded = true
        setText(detailLabel, nativeStatusText())
        changed = false
    case ID_ABOUT:
		detailMode = "about"
		cfg.Expanded = true
		setText(detailLabel, "LAKAKEY  ·  Windows Desktop\r\n\r\nỨng dụng nhập liệu tiếng Việt\r\nPhiên bản 0.13.0\r\n\r\nTác giả: Larx Vo\r\nWebsite: https://vovinhlac.com\r\n\r\nMiễn phí · Hoạt động cục bộ · Không tài khoản")
		changed = false
	case ID_GUIDE:
		detailMode = "guide"
		cfg.Expanded = true
		setText(detailLabel, "HƯỚNG DẪN SỬ DỤNG\r\n\r\n1. Chọn Telex hoặc VNI và Unicode.\r\n2. Chuyển chế độ ở khay hệ thống hoặc Alt + Z khi được chọn.\r\n3. Nhấn Đóng để ẩn cửa sổ xuống khay.\r\n4. Nhấn Mở rộng để xem các thiết lập nâng cao.\r\n\r\nTùy chọn chưa khả dụng được khóa trong giao diện.")
		changed = false
	case ID_DEFAULT:
		detailMode = ""
		wasStartup := cfg.StartWithWindows
		cfg = defaultSettings()
		if wasStartup {
			startupRegistration(false)
		}
		updateHotkey()
	case ID_VN:
		cfg.Vietnamese = !cfg.Vietnamese
	case ID_LAYOUT:
		selected := readCombo(id, inputLayouts)
		if selected != "Telex" && selected != "VNI" {
			setText(statusLabel, "Kiểu gõ này chưa khả dụng")
			changed = false
		} else {
			cfg.Layout = selected
		}
	case ID_ENCODING:
		selected := readCombo(id, encodings)
		if selected != "Unicode" {
			setText(statusLabel, "Bảng mã này chưa khả dụng")
			changed = false
		}
	case ID_HOTKEY1:
		cfg.ToggleKey = "Ctrl+Shift"
		updateHotkey()
	case ID_HOTKEY2:
		cfg.ToggleKey = "Alt+Z"
		updateHotkey()
	case ID_FREE:
		cfg.FreeTyping = checked(id)
	case ID_TONE:
		cfg.ModernTone = checked(id)
	case ID_SPELL, ID_SHORTHAND, ID_SHORTHAND_EN, ID_PERAPP:
		setText(statusLabel, "Tùy chọn này đang được phát triển")
		changed = false
	case ID_SHOWSTART:
		cfg.ShowAtStartup = checked(id)
		if cfg.StartWithWindows { startupRegistration(true) }
	case ID_STARTWIN:
		cfg.StartWithWindows = checked(id)
		startupRegistration(cfg.StartWithWindows)
	case ID_MASCOT:
		cfg.Mascot = readCombo(id, []string{"Off", "Minimal", "Full"})
	default:
		changed = false
	}
	if changed {
		save()
	}
	refresh()
}

func updateHotkey() {
	if hwnd == 0 {
		return
	}
	if hotkeyRegistered {
		unregisterHotKey.Call(hwnd, HOTKEY_TOGGLE)
		hotkeyRegistered = false
	}
	if cfg.ToggleKey == "Alt+Z" {
		r, _, _ := registerHotKey.Call(hwnd, HOTKEY_TOGGLE, MOD_ALT, VK_Z)
		hotkeyRegistered = r != 0
		if !hotkeyRegistered {
			setText(statusLabel, "Alt + Z đang được ứng dụng khác sử dụng")
		}
	}
}
func toggleMode() {
	cfg.Vietnamese = !cfg.Vietnamese
	save()
	refresh()
}
func iconForMode(vietnamese bool) uintptr {
	const size = 32
	andMask := make([]byte, size*size/8)
	colorBits := make([]byte, size*size*4)
	var r, g, b byte = 66, 89, 156
	if vietnamese {
		r, g, b = 23, 138, 101
	}
	for y := 0; y < size; y++ {
		for x := 0; x < size; x++ {
			index := y*size + x
			rounded := (x < 4 && y < 4) || (x > 27 && y < 4) || (x < 4 && y > 27) || (x > 27 && y > 27)
			if rounded {
				andMask[index/8] |= 1 << uint(7-index%8)
				continue
			}
			offset := (size-1-y)*size*4 + x*4
			colorBits[offset], colorBits[offset+1], colorBits[offset+2], colorBits[offset+3] = b, g, r, 255
			// Minimal letter mark, avoids imitating a non-existent mascot asset.
			if y >= 10 && y <= 23 {
				white := false
				if vietnamese {
					white = (x >= 6 && x <= 9 && y <= 19) || (x >= 20 && x <= 23 && y <= 19) || (x >= 10 && x <= 19 && y == 20+int32Abs(x-14)/3)
				} else {
					white = (x >= 7 && x <= 10) || (y == 10 || y == 16 || y == 23) && x >= 11 && x <= 23
				}
				if white {
					colorBits[offset], colorBits[offset+1], colorBits[offset+2] = 255, 255, 255
				}
			}
		}
	}
	inst, _, _ := getModuleHandle.Call(0)
	icon, _, _ := createIcon.Call(inst, size, size, 1, 32, uintptr(unsafe.Pointer(&andMask[0])), uintptr(unsafe.Pointer(&colorBits[0])))
	return icon
}
func int32Abs(i int) int {
	if i < 0 {
		return -i
	}
	return i
}
func makeNID() notifyIconData {
	data := notifyIconData{Hwnd: hwnd, UID: TRAY_UID, Flags: NIF_ICON | NIF_MESSAGE | NIF_TIP, CallbackMessage: TRAY_MESSAGE, HIcon: appIcon}
	data.CbSize = uint32(unsafe.Sizeof(data))
	tip := "Lakakey · EN"
	if cfg.Vietnamese {
		tip = "Lakakey · VN"
	}
	copy(data.Tip[:], syscall.StringToUTF16(tip))
	return data
}
func addTray() {
	appIcon = iconForMode(cfg.Vietnamese)
	d := makeNID()
	notifyIcon.Call(NIM_ADD, uintptr(unsafe.Pointer(&d)))
}
func updateTray() {
	previous := appIcon
	appIcon = iconForMode(cfg.Vietnamese)
	d := makeNID()
	notifyIcon.Call(NIM_MODIFY, uintptr(unsafe.Pointer(&d)))
	if previous != 0 {
		destroyIcon.Call(previous)
	}
}
func removeTray() {
	d := makeNID()
	notifyIcon.Call(NIM_DELETE, uintptr(unsafe.Pointer(&d)))
	if appIcon != 0 {
		destroyIcon.Call(appIcon)
		appIcon = 0
	}
}
func openSettings() { showWindow.Call(hwnd, SW_RESTORE); setForegroundWindow.Call(hwnd) }
func trayMenu() {
	menu, _, _ := createPopup.Call()
	if menu == 0 {
		return
	}
	state := "Chuyển sang EN"
	if !cfg.Vietnamese {
		state = "Chuyển sang VN"
	}
	appendMenu.Call(menu, MF_STRING, 1, strPtr(state))
	appendMenu.Call(menu, MF_STRING, 2, strPtr("Mở cài đặt Lakakey"))
	appendMenu.Call(menu, MF_STRING, 3, strPtr("Thông tin"))
	appendMenu.Call(menu, MF_SEPARATOR, 0, 0)
	appendMenu.Call(menu, MF_STRING, 4, strPtr("Thoát Lakakey"))
	var pt point
	getCursorPos.Call(uintptr(unsafe.Pointer(&pt)))
	setForegroundWindow.Call(hwnd)
	result, _, _ := trackPopup.Call(menu, TPM_RIGHTBUTTON|TPM_RETURNCMD, uintptr(pt.X), uintptr(pt.Y), 0, hwnd, 0)
	destroyMenu.Call(menu)
	postMessage.Call(hwnd, WM_NULL, 0, 0)
	switch result {
	case 1:
		toggleMode()
	case 2:
		openSettings()
	case 3:
		onCommand(ID_ABOUT)
	case 4:
		onCommand(ID_EXIT)
	}
}
func windowProc(h, msg, wp, lp uintptr) uintptr {
	if taskbarCreated != 0 && msg == taskbarCreated {
		addTray() // Explorer restarted: restore icon without opening a window.
		return 0
	}
	switch msg {
	case WM_CREATE:
		hwnd = h
		inst, _, _ := getModuleHandle.Call(0)
		f, _, _ := createFont.Call(^uintptr(16)+1, 0, 0, 0, 400, 0, 0, 0, 1, 0, 0, 0, 0, strPtr("Segoe UI"))
		font = f
		_ = inst
		createControls()
		updateHotkey()
		addTray()
		return 0
	case WM_COMMAND:
		// Only selection changes and button clicks are user actions.
		if wp>>16 == 0 || wp>>16 == 1 {
			onCommand(wp)
		}
		return 0
	case WM_CLOSE:
		if quitRequested {
			user32.NewProc("DestroyWindow").Call(h)
		} else {
			showWindow.Call(h, SW_HIDE)
		}
		return 0
	case WM_HOTKEY:
		if wp == HOTKEY_TOGGLE && cfg.ToggleKey == "Alt+Z" {
			toggleMode()
		}
		return 0
	case WM_TIMER:
		if wp==TRAY_CLICK_TIMER && pendingTrayClick { pendingTrayClick=false;killTimer.Call(h,TRAY_CLICK_TIMER);toggleMode() }
		return 0
	case WM_QUERYENDSESSION:
		_ = writeSettings(cfg)
		return 1
	case TRAY_MESSAGE:
		switch lp & 0xffff {
		case WM_LBUTTONUP:
			pendingTrayClick=true
			if id,_,_:=setTimer.Call(h,TRAY_CLICK_TIMER,260,0);id==0 {pendingTrayClick=false;toggleMode()}
		case WM_LBUTTONDBLCLK:
			pendingTrayClick=false
			killTimer.Call(h,TRAY_CLICK_TIMER)
			openSettings()
		case WM_RBUTTONUP:
			trayMenu()
		}
		return 0
	case WM_SINGLEEXIT:
		quitRequested = true
		user32.NewProc("DestroyWindow").Call(h)
		return 0
	case WM_DESTROY:
		killTimer.Call(h,TRAY_CLICK_TIMER)
		if hotkeyRegistered {
			unregisterHotKey.Call(hwnd, HOTKEY_TOGGLE)
		}
		removeTray()
		if font != 0 {
			deleteObject.Call(font)
		}
		postQuitMessage.Call(0)
		return 0
	}
	ret, _, _ := defWindow.Call(h, msg, wp, lp)
	return ret
}
func main() {
	runtime.LockOSThread()
	instanceMutex, _, _ = createMutex.Call(0, 0, strPtr("Local\\LakakeyDesktopSingleInstance"))
	if instanceMutex != 0 {
		if last, _, _ := getLastError.Call(); last == ERROR_ALREADY_EXISTS {
			existing, _, _ := findWindow.Call(strPtr("LakakeyNativeDesktop2026"), 0)
			if existing != 0 {
				if len(os.Args) > 1 && strings.EqualFold(os.Args[1], "--quit") {
					postMessage.Call(existing, WM_SINGLEEXIT, 0, 0)
				} else if !(len(os.Args) > 1 && strings.EqualFold(os.Args[1], "--tray")) {
					showWindow.Call(existing, SW_RESTORE)
					setForegroundWindow.Call(existing)
				}
			}
			return
		}
	}
	if len(os.Args) > 1 && strings.EqualFold(os.Args[1], "--quit") {
		return
	}
	taskbarCreated, _, _ = registerWindowMsg.Call(strPtr("TaskbarCreated"))
	cfg = readSettings()
	_ = publishRuntimeMode(cfg)
	if err := writeSettings(cfg); err != nil {
		fmt.Fprintln(os.Stderr, err)
	}
	setProcessDpiAware.Call()
	instance, _, _ := getModuleHandle.Call(0)
	class := wide("LakakeyNativeDesktop2026")
	wc := wndClass{Size: uint32(unsafe.Sizeof(wndClass{})), WndProc: syscall.NewCallback(windowProc), Instance: instance, ClassName: class}
	wc.Cursor, _, _ = loadCursor.Call(0, 32512)
	reg, _, _ := registerClass.Call(uintptr(unsafe.Pointer(&wc)))
	if reg == 0 {
		fmt.Fprintln(os.Stderr, "Lakakey window initialization failed")
		return
	}
	window, _, _ := createWindow.Call(0, uintptr(unsafe.Pointer(class)), strPtr("Lakakey · Cài đặt"), 0x00CA0000, 130, 100, 592, 320, 0, 0, instance, 0)
	if window == 0 {
		return
	}
	if !(len(os.Args) > 1 && strings.EqualFold(os.Args[1], "--tray")) {
		showWindow.Call(window, SW_SHOW)
	}
	var message winMsg
	for {
		got, _, _ := getMessage.Call(uintptr(unsafe.Pointer(&message)), 0, 0, 0)
		if int32(got) <= 0 {
			break
		}
		translateMessage.Call(uintptr(unsafe.Pointer(&message)))
		dispatchMessage.Call(uintptr(unsafe.Pointer(&message)))
	}
}
