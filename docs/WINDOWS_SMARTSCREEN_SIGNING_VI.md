# Lakakey — phát hành Windows dưới danh nghĩa cá nhân Larx Vo

© 2026 Larx Vo · https://vovinhlac.com

> **Trạng thái:** đây là hướng dẫn thiết lập phát hành có ký số. Hiện chưa có
> chứng thư cá nhân được xác minh và chưa có file EXE mới ký hợp lệ.
> Bản v0.8 chỉ là Windows Settings preview, **không phải bộ gõ tiếng Việt**.
> Windows TSF Native Alpha chưa được kiểm thử gõ thật đầy đủ.

## 1. SmartScreen đang chặn điều gì?

Ảnh chụp của người dùng hiện `Windows protected your PC` và thông báo
`Microsoft Defender SmartScreen prevented an unrecognized app from starting`
khi mở `LakakeyPreviewSetup.exe`. Windows chưa đủ độ tin cậy đối với EXE hoặc
publisher. Cảnh báo **không tự động xác nhận có mã độc**, nhưng cũng **không xác
nhận phần mềm an toàn**. Không cần xuất bản qua Microsoft Store để có thể cài
ứng dụng Win32 độc lập. Không tắt SmartScreen hoặc khuyên người dùng
`Run anyway` cho bản IME chưa QA.

## 2. Cá nhân Larx Vo cần xin chứng thư gì?

Chọn **publicly trusted code-signing certificate** cho cá nhân, từ một CA
được Windows tin cậy, hoặc dịch vụ ký phần mềm có xác minh danh tính cá nhân
và hỗ trợ Windows Authenticode. Không mua chứng thư SSL/TLS cho website để
thay thế vì mục đích khác.

- Đăng ký bằng **tên pháp lý trùng với giấy tờ tùy thân**, cung cấp thông tin
  liên hệ và giấy tờ theo quy trình KYC của đơn vị cấp; có thể cần xác minh
  qua video/điện thoại. Không khai tên hiển thị `Larx Vo` thay tên pháp lý
  nếu đơn vị cấp không chấp nhận.
- Nhãn thương hiệu trong giao diện/README/website vẫn dùng **Larx Vo** và
  **vovinhlac.com**. Windows `Verified publisher` hiển thị danh tính trong
  chứng thư, không tự động hiển thị nickname `Larx Vo`.
- Trước khi thanh toán, hỏi trực tiếp nhà cung cấp: **có cấp chứng thư cho cá
  nhân cư trú tại Việt Nam không?** Loại xác minh danh tính, phí thực tế,
  hình thức giữ private key, tương thích SignTool/Authenticode, timestamp,
  quy trình gia hạn/thu hồi, và có phát sinh chi phí dịch vụ ký hay không.
- Khóa riêng nên là **non-exportable** qua token/HSM hoặc dịch vụ cloud ký
  được quản lý. Không gửi CCCD, mật khẩu, token PIN, khóa riêng hay PFX vào
  ChatGPT, GitHub Issues, commit, Actions variables/logs hoặc email công khai.
- Một số chứng thư cá nhân có thể không tương thích với mô hình runner; xác
  minh phương thức ký trước khi mua. Không thể bảo đảm giá/điều kiện hiện hành
  nếu chưa có xác nhận chính thức từ nhà cung cấp.

### Lưu ý về SmartScreen

Chữ ký Authenticode hợp lệ giúp Windows kiểm tra **nguồn gốc nhà phát hành
và tính toàn vẹn file**, **không bảo đảm SmartScreen hết cảnh báo ngay**.
Danh tiếng của publisher/file vẫn phải được Windows đánh giá qua thời gian.
Không thể tự tạo `Verified publisher` chỉ bằng sửa metadata EXE.

## 3. Đã triển khai trong GitHub

Nhánh `fix/trusted-windows-release` gồm:

- `scripts/windows/Sign-LakakeyRelease.ps1`: lấy chứng thư từ
  `Cert:\CurrentUser\My` trên Windows có private key sẵn, yêu cầu code-signing
  EKU, subject khớp danh tính pháp lý, kiểm tra certificate chain + revocation,
  ký SHA-256 và xác minh chữ ký/timestamp qua Windows SDK SignTool.
- `.github/workflows/windows-signed-candidate.yml`: workflow thủ công, chỉ ký
  source đã duyệt trên `main`, sử dụng runner Windows x64 gắn nhãn
  `lakakey-signing` và GitHub Environment `lakakey-production-signing`.
  **Không tự động đăng GitHub Releases.**
- `.github/workflows/windows-signing-lint.yml`: kiểm tra cú pháp script
  ký số và compile preview không cần certificate.
- `SHA256SUMS.txt`: manifest SHA-256 đi kèm artifact.

**Không đưa chứng thư cá nhân vào shared GitHub-hosted runner và không dùng
biến `LAKAKEY_SIGN_PFX_BASE64` hoặc `LAKAKEY_SIGN_PFX_PASSWORD` nữa.**

## 4. Cài đặt môi trường ký số trên GitHub

Chỉ triển khai khi đã có chứng thư hợp lệ:

1. Hoàn thiện Pull Request, chạy lint và review, merge vào `main`.
2. Trong GitHub repository → **Settings → Environments** → tạo
   `lakakey-production-signing`; bật required reviewers (nếu quyền repo/plan
   hỗ trợ), restrict deployment branches to `main`. Không cho PR từ fork
   chạy code trên runner được phép ký.
3. Chuẩn bị **Windows x64 runner chuyên dụng** mà chính mình kiểm soát;
   gán nhãn `lakakey-signing`. Bảo vệ máy, hạn chế người truy cập, dùng
   runner tạm thời/ephemeral khi phù hợp; không cho workflow bên ngoài
   truy cập khóa ký. Tối ưu hơn là dịch vụ cloud signing hỗ trợ ký từ GitHub
   với OIDC và quyền ký giới hạn; cần integration riêng theo nhà cung cấp.
4. Cài Windows SDK SignTool và Go. Lắp token / kết nối HSM của CA và
   cài chứng thư vào `Cert:\CurrentUser\My` cho tài khoản chạy runner.
   Đảm bảo provider cho phép ký không cần nhập PIN thủ công, hoặc chọn
   quy trình phát hành thủ công có người xác nhận trên máy ký.
5. Ở GitHub Environment variables (không chứa private key) cấu hình:
   - `LAKAKEY_SIGN_CERT_THUMBPRINT`: thumbprint thực tế của chứng thư.
   - `LAKAKEY_SIGN_CERT_SUBJECT`: **chính xác** giá trị Subject pháp lý trong
     chứng thư (ví dụ định dạng `CN=...`, không tự điền nickname).
6. Vào **Actions → Lakakey trusted Windows signing candidate → Run workflow**,
   chọn nhánh `main`. Runner ký ứng dụng con trước rồi ký bộ cài cuối cùng.
   Nếu không xác minh được chứng thư hoặc chữ ký, build dừng và không upload
   artifact signed candidate.
7. Tải candidate từ **Artifacts** chỉ để QA, so sánh `SHA256SUMS.txt`,
   kiểm tra SignTool và Microsoft Defender trên Windows 10/11 sạch.
8. Sau khi bộ gõ TSF gõ được thật, kiểm tra riêng cài / gỡ / cập nhật /
   trường mật khẩu / Word / Chrome / Edge / Notepad / VS Code; rồi mới
   công bố bản phát hành.

## 5. Tự kiểm tra chứng thư và chữ ký trên Windows

Mở PowerShell **trên máy ký**:

```powershell
Get-ChildItem Cert:\CurrentUser\My |
    Select-Object Subject, Thumbprint, NotAfter, HasPrivateKey

Get-AuthenticodeSignature .\LakakeyPreviewSetup.exe |
    Format-List Status, SignerCertificate, TimeStamperCertificate

Get-FileHash .\LakakeyPreviewSetup.exe -Algorithm SHA256
```

Kết quả kỳ vọng của `Get-AuthenticodeSignature` là `Status: Valid` và
`SignerCertificate.Subject` trùng tên pháp lý đã được CA xác minh.
Không công bố một bản mới khi `Status` là `NotSigned`, `UnknownError`
hoặc không có timestamp hợp lệ.

## 6. Nếu không có runner/thiết bị ký

**Không cần cấu hình runner ngay.** Có thể dùng máy Windows riêng của
người phát hành để ký và xác minh EXE *sau khi QA*, hoặc dịch vụ ký từ xa
được CA chấp nhận. Sau đó công bố đúng file đã ký và SHA-256 qua GitHub
Releases theo quy trình kiểm duyệt thủ công. Tuyệt đối không chuyển private
key sang runner tạm thời chỉ để tự động hóa.

## 7. Nơi cần xác minh tiếp

- Nhà cấp chứng thư hoặc Microsoft phải xác nhận chính sách, mức phí,
  và khả năng cấp cho cá nhân ở Việt Nam tại thời điểm mua.
- Cần kiểm thử ứng dụng thật trên Windows trước khi gọi Lakakey là bộ gõ
  thay UniKey.
- Microsoft Store **không bắt buộc** đối với EXE độc lập.
