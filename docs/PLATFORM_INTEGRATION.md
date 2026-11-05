# Lakakey — Platform Integration Contract (production implementation pending)

## Giao diện core hiện có

- `lk_create(scheme, tone_style)` -> IME session handle.
- `lk_feed(handle, unicode_scalar)` -> `{commit, preedit, consumed}`.
- `lk_backspace(handle)` -> cập nhật preedit hoặc pass-through khi rỗng.
- `lk_commit(handle)` -> commit pending text và clear buffer.
- `lk_reset(handle)` -> reset khi abandon composition, focus thay đổi.
- `lk_configure(handle, scheme, tone_style, enabled)` -> đổi cấu hình (host commit trước khi gọi nếu muốn giữ chữ).

`preedit` là **full replacement**, không phải diff. `commit` là chuỗi UTF-8 phải được chèn một lần. C API result pointers có hiệu lực tới lần gọi tiếp theo trên cùng handle. Host phải giữ handle theo đúng text context và không chia sẻ across threads mà không đồng bộ.

## Windows (TSF)

- Windows desktop IME chính nên viết bằng C++ native Text Services Framework (TSF), COM, `ITfTextInputProcessorEx`, `ITfKeyEventSink`, composition/edit sessions.
- Đăng ký profile/CLSID và input source, giữ input context per document/input field, đồng bộ `start/update/end composition` với core.
- Dùng các API host để thay thế `preedit` và `commit`, không liên tục gửi `SendInput` và `Backspace` vào cửa sổ bất kỳ.
- Khi focus chuyển, selection đổi, mouse click giữa preedit: commit hoặc cancel theo policy đã test; không ghép chữ giữa hai ô.
- Xác định secure/password context khi host cho biết; không giữ event; không hook dùng để lấy văn bản ngoài composition.
- Kiểm thử Text Services support: Notepad, Word, Excel, Edge/Chrome/Firefox, VS Code, Windows Terminal, RDP, app Win32 cũ, app với UAC/elevation. Có thể cần compatibility fallback đặc thù, không bảo đảm 100% app.
- Tray+Settings có thể triển khai WinUI/Win32 gọn, không phụ thuộc quá nhiều framework.
- Package: x64 + arm64; ký số file EXE/MSI nếu phát hành chính thức. Cần QA installer/uninstaller và rollback.

## macOS (InputMethodKit)

- InputMethodKit (`IMKServer`, `IMKInputController`) đưa input source thật vào macOS; Swift/Objective-C++ bridge gọi `lakakey_c`.
- Trên key event, convert thành Unicode scalar đúng keyboard layout; cập nhật `setMarkedText` nếu đang composition và `insertText` khi commit; tôn trọng selectionRange/replacementRange.
- Secure input mode hoặc secure text entry phải pass-through theo OS; không cố override cơ chế an toàn macOS.
- Menu bar Settings bằng SwiftUI/AppKit native; không cần renderer web để xử lý phím.
- Đóng gói `.app` input method + installer hợp lệ theo OS; cần Apple Developer signing và notarization cho phân phối mượt, có thể còn tương thích chính sách OS phiên bản.
- Test cả Intel/Apple Silicon; Chrome, Safari, Pages, Word, Terminal, Xcode, password fields, Japanese/Chinese IME switching.

## Linux (Fcitx5 và IBus)

- Hỗ trợ cả **Fcitx5** và **IBus** là mục tiêu, bởi distro/desktop environment không đồng nhất.
- Fcitx5: viết native addon/engine wrapper, dùng `InputContext` để nhận phím và cập nhật preedit/commit. IBus: engine C/GObject sử dụng `ibus_engine_update_preedit_text` và `ibus_engine_commit_text`, có thể gọi C ABI từ C/C++.
- Kết nối theo cơ chế input method của phiên đăng nhập, không dùng X11 global hook giả định chạy trên Wayland.
- Test GNOME Wayland, KDE Plasma Wayland, X11, XWayland, GTK, Qt, Electron, terminal, Flatpak/Snap apps theo khả năng IME của từng host.
- Packaging dự kiến `.deb`, `.rpm` và cách tích hợp phù hợp từng distro; AppImage chỉ phù hợp phần Settings, thường không đủ để đăng ký system IME.
- Không hứa hỗ trợ mọi Wayland compositor nếu chưa thử thực tế.

## Các event adapter cần xử lý ngoài `feed()`

1. `text-input` có Unicode scalar do OS cung cấp -> `lk_feed`.
2. `Backspace` trong preedit -> `lk_backspace`; ngoài preedit -> OS xử lý.
3. `Space`, punctuation -> `lk_feed` để commit token + dấu phân cách.
4. `Tab/Enter`, arrow, Home/End, mouse select, focus lost -> commit/cancel policy trước khi forward nguyên event.
5. `Ctrl/Cmd` shortcuts, OS hotkeys, non-text modifiers -> OS xử lý, không gọi engine.
6. Paste / clipboard / drag-drop -> không re-key thành từng ký tự; OS chịu trách nhiệm.
7. Autofill/password/OTP -> bypass, `lk_reset`.
8. Multi-cursor / collaborative editors -> chỉ composition tại active context nếu supported.
9. Dynamic layout/keymaps và input sources khác -> không cướp xử lý phím không thuộc Lakakey.
10. Composition undo trong core -> host cập nhật marked text; Ctrl+Z của host chỉ tác động text đã commit.

## Sự thật về bản foundation

Thư mục `platform/` hiện là **đặc tả tích hợp**, không có native adapters đã hoàn thiện. Việc biên dịch `lakakey_lab` trên Windows/macOS **không** tự đăng ký nó là một input method. Cần phát triển, ký/đóng gói và kiểm tra chính xác trên từng OS trước khi công bố phiên bản dùng hàng ngày.
