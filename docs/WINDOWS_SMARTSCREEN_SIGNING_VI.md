# Lakakey — xử lý Microsoft Defender SmartScreen đúng cách

© 2026 Larx Vo — https://vovinhlac.com

## Chẩn đoán hiện tại

Ảnh người dùng cho thấy **Windows protected your PC** và dòng
**Microsoft Defender SmartScreen prevented an unrecognized app from starting**
khi mở `LakakeyPreviewSetup.exe`.

Đây là cảnh báo về mức độ tin cậy / danh tiếng của file và nhà phát hành,
**không phải bằng chứng malware**, cũng không chứng minh file an toàn.
Không cần đăng ứng dụng lên Microsoft Store để có thể phát hành bộ cài EXE độc lập.

**Bản v0.8 chỉ là Settings preview, chưa gõ tiếng Việt toàn hệ thống.**
Không nên vô hiệu hóa SmartScreen hoặc khuyên người dùng bấm Run anyway.
TSF native alpha là bản thử nghiệm kỹ thuật riêng, không phải bản ổn định.

## Đường phát hành được đề xuất

1. Hoàn thiện và QA Windows TSF IME trên Windows 10/11 trước khi public.
2. Chọn phương án ký số có xác minh danh tính nhà phát hành:
   - Chứng thư code-signing được CA đáng tin cậy cấp cho cá nhân/doanh nghiệp,
     hoặc hệ thống cloud/HSM signing được hỗ trợ bởi công cụ phát hành.
   - Nếu dùng PFX có thể xuất được: cấu hình bí mật `LAKAKEY_SIGN_PFX_BASE64`
     và `LAKAKEY_SIGN_PFX_PASSWORD` trong GitHub Actions secrets.
   - Nếu CA không cho xuất private key thành PFX, dùng integration cloud/HSM phù hợp.
     **Tuyệt đối không xuất private key lên GitHub hay commit certificate/password.**
3. Chạy thủ công `Windows signed preview — manual candidate only`.
   Workflow fail-closed nếu không có chứng thư hợp lệ, và không tự phát hành công khai.
4. Xem `SHA256SUMS.txt`, `RELEASE_INFO.txt`, chữ ký Authenticode và scan bảo mật.
5. Kiểm thử sạch trên VM, cả cài / update / uninstall, UI, IME, Word/Notepad/Chrome/Edge.
6. Chỉ sau khi vượt QA, mới xuất bản GitHub Releases với hướng dẫn rõ chức năng.

## Thiết lập secrets (chỉ khi đã có PFX hợp lệ)

Trong repository GitHub:

Settings → Secrets and variables → Actions → New repository secret.

- `LAKAKEY_SIGN_PFX_BASE64`: nội dung PFX được mã hóa base64.
- `LAKAKEY_SIGN_PFX_PASSWORD`: mật khẩu chứng thư.

Không đưa các giá trị bí mật vào tin nhắn, Issues, logs hay source code.
Không tạo chứng thư tự ký rồi tuyên bố là publisher đã xác minh.

Workflow được thiết kế cho **PFX xuất được**, không áp dụng cho chứng thư
khóa riêng trong USB/HSM không xuất được. Khi dùng giải pháp cloud signing,
phải thêm adapter tương ứng và chính sách phân quyền ký chỉ với nhánh/release tin cậy.

## Xác minh file sau khi nhận bản ký

Mở PowerShell trên Windows:

```powershell
Get-AuthenticodeSignature .\LakakeyPreviewSetup.exe | Format-List Status,SignerCertificate,TimeStamperCertificate
Get-FileHash .\LakakeyPreviewSetup.exe -Algorithm SHA256
```

Chữ ký hợp lệ nghĩa là nhận dạng nhà phát hành và nội dung file được bảo vệ
theo quy tắc Authenticode. **Không bảo đảm SmartScreen hết cảnh báo ngay**:
Windows còn xem danh tiếng file/certificate, chính sách doanh nghiệp,
Smart App Control và các tín hiệu bảo mật khác.

Nếu vẫn bị cảnh báo dù đã ký hợp lệ, có thể gửi bản build và thông tin
chính xác qua kênh **Microsoft Security Intelligence file submission**
để Microsoft đánh giá. Không thể tự khẳng định được gỡ cảnh báo;
phản hồi thuộc Microsoft.

## Vì sao không chuyển ngay lên Microsoft Store?

Microsoft Store là một lựa chọn phân phối, không phải điều kiện bắt buộc
để tải và cài phần mềm Windows. Với bộ gõ TSF/COM, chính sách đóng gói
và khả năng Store chấp nhận cần được kiểm tra riêng. Không nên đăng một
bản IME chưa QA chỉ để né SmartScreen.

## Trạng thái thực tế

- Workflow build v0.8 preview hiện tại tạo EXE **chưa ký**.
- Workflow mới cho signed preview mới là **đường chuẩn bị**, chưa chạy với
  chứng thư của Larx Vo; vì thế chưa có file `LakakeyPreviewSetup.exe`
  được ký và xác minh danh tính nhà phát hành.
- Windows TSF Native Alpha đã build, được kiểm thử đăng ký/gỡ COM trong CI,
  nhưng vẫn chưa đạt kiểm thử gõ hệ thống trong máy Windows thực.
