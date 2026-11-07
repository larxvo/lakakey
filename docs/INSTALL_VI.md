# Cài đặt Lakakey 0.14.1 trên Windows

Lakakey dùng **một tệp cài đặt EXE có icon thương hiệu**. Không cần giải nén
một thư mục source để chạy chương trình.

## Cài đặt

1. Tải **Lakakey-Setup-0.14.1-Windows-x64.exe** từ gói Windows CI được đánh
   dấu **Success** trên [GitHub Actions](https://github.com/larxvo/lakakey/actions/workflows/windows-desktop-beta.yml)
   hoặc đường dẫn EXE do dự án cung cấp. Khi tải artifact từ GitHub Actions,
   phải giải nén artifact ZIP **một lần** để lấy file EXE bên trong; chính file
   EXE là trình cài đặt duy nhất.
2. Xác minh nguồn tệp và SHA-256 trong `SHA256SUMS.txt`.
3. Nhấp đúp **Lakakey-Setup-0.14.1-Windows-x64.exe** có icon Lakakey.
4. Làm theo cửa sổ cài đặt Windows (Inno Setup). Chọn tạo shortcut Desktop
   nếu muốn; mặc định có shortcut Start Menu.
5. Ở trang cuối, chọn **Launch Lakakey** nếu muốn mở Settings sau khi cài.
   Sau đó Lakakey có thể chạy nền ở system tray.

Ứng dụng mặc định được cài cho **tài khoản Windows hiện tại**; không yêu cầu
quyền Administrator. Có thể gỡ trong **Settings → Apps → Installed apps**.

## Biểu tượng

Logo chữ **L kèm dấu chân màu trắng trên nền xanh** được lấy từ tài sản brand
có sẵn tại `ui/assets/brand_3d_reference.png`, chuyển thành ICO nhiều kích
thước rồi nhúng trực tiếp vào cả `LakakeyDesktop.exe` và tệp Setup EXE.
Không phụ thuộc một tệp ICO rời để Windows Explorer hiển thị icon.

## Trạng thái kỹ thuật

Bộ cài Lakakey 0.14.1 chỉ cài **Settings và system tray**. Nó không tự cài
hoặc đăng ký thành phần TSF thử nghiệm. Việc gõ tiếng Việt toàn hệ thống
trong Word, Chrome, Notepad và VS Code vẫn cần kiểm chứng riêng.

Tệp cài đặt thử nghiệm chưa ký số. Không bỏ qua cảnh báo Windows chỉ để
cài một tệp không xác minh được nguồn gốc.

Không có popup quảng bá hoặc thông báo so sánh phần mềm khác.
Dữ liệu đã gõ không được ghi lại hay tải lên server.

© 2026 Larx Vo · https://vovinhlac.com.
