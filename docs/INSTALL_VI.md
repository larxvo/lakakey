# Hướng dẫn cài Lakakey 0.14 — Windows x64

**Tác giả:** Larx Vo · https://vovinhlac.com

## Tải bộ cài

Mở [Windows CI Lakakey](https://github.com/larxvo/lakakey/actions/workflows/windows-desktop-beta.yml),
chọn một lần chạy **Success** của nhánh 0.14, rồi tải artifact
`Lakakey-0.14-Windows-x64-Dev`. Giải nén và chạy
`LakakeyDesktopSetup.exe`. Nếu có gói cài trực tiếp được chia sẻ trong
cuộc trò chuyện này, hãy kiểm tra SHA-256 đi kèm để đối chiếu.

Bộ cài cài ứng dụng Settings/tray cho tài khoản Windows hiện tại.
Không yêu cầu đăng nhập và không hiển thị các popup thông tin lặp lại.
Mục **Trạng thái native** trong Settings cho biết liệu hồ sơ thử nghiệm
được đăng ký hay chưa; chưa chứng minh việc nhập tiếng Việt trong ứng dụng.

## Gỡ cài đặt

Vào Windows Settings → Apps → Installed apps → Lakakey Desktop →
Uninstall. Thao tác này không cần chỉnh sửa các chương trình gõ khác.

## An toàn và trạng thái sản phẩm

Đây là bản **phát triển chưa ký số**, chưa được xác minh hoàn chỉnh trên
Windows 10/11. Chỉ chạy tệp có nguồn gốc rõ ràng và không cố bỏ qua cảnh báo
bảo mật của hệ điều hành nếu chưa xác nhận được nhà phát hành. Không cài đặt
DLL TSF trong gói nghiên cứu lên máy dùng hằng ngày.

Bộ cài này **chưa kích hoạt khả năng gõ tiếng Việt toàn hệ thống**.
TSF đang được kiểm thử trong môi trường tách biệt. Xem
[ma trận QA 0.14](WINDOWS-COMPAT-0.14.md) để biết các điều kiện trước khi
phát hành đầy đủ.

## Gửi báo cáo lỗi

Mở [GitHub Issues](https://github.com/larxvo/lakakey/issues),
ghi rõ Windows version, cách tái hiện lỗi và kết quả mong muốn. Không
đính kèm mật khẩu, văn bản nhạy cảm hoặc lịch sử phím đã gõ.

© 2026 Larx Vo — https://vovinhlac.com
