# Lakakey — QA / SECURITY GATE

## Kiểm thử ký tự nền tảng

- Chữ thường/hoa/mixed case, các thanh sắc/huyền/hỏi/ngã/nặng, chữ ă â ê ô ơ ư đ.
- Dấu sau nguyên âm, sau chữ cuối, sau coda; `iê`, `uô`, `ươ`, `uyê`, `oa/oe/uy`, `qu/gi`.
- Đặt dấu kiểu mới/cũ và từ ghép thông dụng, từ miền Bắc/Trung/Nam trong bộ kiểm chứng.
- Dấu gõ trùng, bỏ dấu, sửa dấu, Backspace nhiều lần, Ctrl+Z, gõ tắt, Esc, chuyển app giữa chừng.
- Một ký tự, chuỗi dài, chữ số, dữ liệu có emoji/combining marks/Unicode ngoài BMP.
- URL/email/path/câu lệnh code, thuật ngữ tiếng Anh phổ biến, VIQR escape khi được triển khai.
- Chữ chưa chuẩn hóa NFD, surrogate pairs (Windows UTF-16), Unicode NFC trong file Office.

## Kiểm thử focus và tương thích

- Mỗi OS: text fields đa dòng/một dòng, password/OTP, trình duyệt, công cụ chat, MS Office/LibreOffice, phần mềm dev, terminal, ứng dụng bị lag.
- Chuyển focus nhanh bằng Alt+Tab/Cmd+Tab, click sang ô mới giữa lúc gõ, drag select, clipboard paste, IME khác, screen lock và log out.
- Ctrl+Shift/Alt+Z hoặc phím tự cấu hình; không phá shortcut hệ thống hoặc gõ trong password.
- High DPI, đa màn hình, remote desktop, network latency không làm mất phím.
- Nhiều tiến trình, native popup, long-running sessions, crash recovery, update in-use.

## Security/Privacy rules

1. Không ghi nội dung người dùng gõ vào file log, crash dump hoặc analytics.
2. Không đọc toàn bộ nội dung clipboard để gõ tắt; chỉ khi có thao tác chuyển mã chủ động.
3. Chỉ lưu settings, shortcuts, custom dictionary được người dùng chủ động khai báo; không lưu lịch sử đánh máy.
4. Secure fields pass-through. Nếu host không xác nhận được context và chức năng là nhạy cảm, fail safe.
5. Không yêu cầu quyền admin/root khi xử lý phím; cài đặt package dùng quyền phù hợp.
6. Không update các thư viện native từ URL không ký; tất cả binaries cần checksum/signature.
7. Nếu hỗ trợ crash report: opt-in, scrub nội dung và session text trước khi upload.
8. Có phương án khôi phục về system keyboard khi Lakakey crash/uninstall.
9. C API và Unicode parser được fuzz, không chấp nhận chuỗi UTF-8 malformed.
10. Bảo vệ settings permissions; validate import text-expansion chống thay đổi file đường dẫn tùy ý.

## Kiểm thử và ngưỡng release

| Gate | Đầu ra cần có trước public 1.0 |
|---|---|
| Engine | Bộ từ kiểm chứng rộng, unit + fuzz + property tests, code review |
| Unicode | Round-trip Unicode NFC với chữ VN, chữ hoa và biên ký tự |
| Real OS | Tối thiểu Windows 11, macOS hiện hành, GNOME/KDE + Wayland/X11 phù hợp |
| Integration | Không mất/dư chữ trong các app mục tiêu qua automation thủ công và automated tests |
| Privacy | Thử password/OTP, package permissions, no-network-at-keypress |
| Performance | Measure event p50/p95/p99, idle RAM/CPU, CPU high load, startup |
| Distribution | Packages signed/notarized as applicable, uninstall sạch, input source fallback |
| Access | Keyboard-only settings, screen readers, screen zoom/DPI |

**Trạng thái 0.1:** chỉ có unit tests cho core + C API build trên Linux x86_64. Chưa qua gates cho bản thương mại/hàng ngày.
