# Lakakey — trạng thái kỹ thuật 0.14

Cập nhật ngày 06/11/2026 · Tác giả: Larx Vo · https://vovinhlac.com

| Thành phần | Trạng thái mã nguồn | Bằng chứng |
| --- | --- | --- |
| Atomic Space commit (TSF) | Hoàn thiện mã commit một phiên cho từ và dấu cách | Mô phỏng pipeline C++ và kiểm thử policy; chưa kiểm tra trong Word/Chrome |
| Core Telex/VNI/Unicode | Đã triển khai nền tảng | C++ engine/session/contract tests đạt |
| Windows Desktop Settings + tray | Đã triển khai | Go tests và Windows x64 build đạt |
| Per-user EXE installer | Đã tạo | GitHub Actions Windows x64 build đạt; QA cập nhật/gỡ trên máy thật còn thiếu |
| Cấu hình an toàn | Có ghi file thay thế, backup/migration | Go unit + race tests đạt |
| VN/EN preference bridge | Sử dụng Registry DWORD, không lưu ký tự | Kiểm thử mode và Windows compile đạt |
| TSF text-service prototype | Có COM class, composition và key sink | MSVC build, COM smoke, Windows per-user register/unregister smoke đạt |
| Keyboard layout translation | Dùng ToUnicodeEx không thay đổi dead-key state | Windows layout smoke đạt |
| Focus/composition cleanup | Có đường xử lý best-effort | Chưa có kiểm thử nhập liệu end-to-end |
| Trường mật khẩu | Có kiểm tra bảo thủ với native EDIT | Browser/Office/Electron cần QA thực tế |
| Tương thích Word/Chrome/Edge/VS Code | Chưa xác minh | Chưa đủ điều kiện dùng làm bộ gõ chính |
| Mascot animation | Có nghiên cứu riêng | Chưa tích hợp vào desktop 0.13 |
| macOS / Linux adapters | Chưa triển khai phiên bản sử dụng được | Chưa kiểm thử |
| Chữ ký và phát hành ổn định | Chưa có chữ ký số | Chưa production |

## Kết quả CI đã xác minh

Tại 0.13, Windows CTest đạt **6/6**; 0.14 bổ sung một bài kiểm thử giao dịch nhập liệu: engine, session, contract, TSF policy,
keyboard layout, và COM smoke. Kiểm thử đăng ký/kiểm tra/gỡ đăng ký COM
trong môi trường runner dùng tạm cũng đạt. Điều này xác nhận xây dựng và
đăng ký TSF cơ bản, **không xác nhận nhập liệu hoạt động trong các ứng dụng thực**.

## Hàng rào phát hành

Bộ cài desktop thông thường không chứa hoặc đăng ký TSF thử nghiệm.
Cần kiểm thử trên Windows 10/11 thật: Notepad, trình duyệt, Office,
IDE, vùng mật khẩu, con trỏ, focus, tổ hợp phím, đóng/mở tiến trình,
tính năng rollback và gỡ cài đặt. Không được đánh đồng việc build thành công
với một sản phẩm nhập liệu toàn hệ thống ổn định.

Theo dõi: https://github.com/larxvo/lakakey/pull/4

© 2026 Larx Vo.
