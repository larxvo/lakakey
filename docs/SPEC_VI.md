# LAKAKEY — ĐẶC TẢ SẢN PHẨM (ĐỊNH HƯỚNG 1.0)

**Product:** Lakakey · Vietnamese Input Method  
**Tác giả:** Larx Vo · https://vovinhlac.com  
**Phân phối:** miễn phí hoàn toàn, sử dụng offline, không cần tài khoản.

## 1. Tuyên ngôn sản phẩm

Lakakey phải là bộ gõ tiếng Việt native có độ tin cậy cao để dùng cả ngày, không phải trình soạn thảo văn bản hay wrapper trang web. Giá trị của Lakakey đến từ trải nghiệm đa nền tảng thống nhất, khả năng kiểm soát tương thích theo ứng dụng, gõ tắt hiệu quả, quản lý lỗi tốt, quyền riêng tư rõ ràng và cấu hình đơn giản. Không phức tạp hóa giao diện nhập liệu.

### Nguyên tắc bắt buộc

1. **Đúng chữ trước tiên:** không làm mất ký tự, không chèn 2 lần, không nhảy con trỏ, không tự ý sửa chữ ngoài vùng đang gõ.
2. **Nhanh và nhẹ:** lõi no-dependencies, không tải web khi gõ, không AI online ở đường xử lý phím.
3. **Mặc định dễ hiểu:** bật tiếng Việt, Telex, Unicode NFC, bỏ dấu hiện đại; đầy đủ VNI và tùy chọn khác.
4. **Quyền riêng tư:** không lưu lại văn bản người dùng gõ, không log phím, bỏ qua ô mật khẩu và vùng nhập liệu nhạy cảm.
5. **Không cản trở hệ điều hành:** không chặn tổ hợp phím hệ thống, không giật focus, không gài keyboard hook toàn cục làm đường chính.
6. **Dễ thoát, dễ tắt:** khi tắt gõ Việt, phím được chuyển về hệ điều hành an toàn.
7. **Hoàn toàn miễn phí:** không gói Premium, không watermark, không yêu cầu đăng nhập hay website đang online.

## 2. Bảng tính năng mục tiêu

| Nhóm | Bản 1.0 phải có | Sau 1.0 |
| --- | --- | --- |
| Kiểu gõ | Telex, VNI, Plain, VIQR; cấu hình phím tắt | Custom keymap, Telex hybrid |
| Unicode | NFC chuẩn; chữ hoa/thường, dấu đầy đủ, độ dài từ | Cơ chế NFD mở rộng |
| Quy tắc dấu | Kiểu mới/cũ, dấu linh hoạt, undo đúng, dấu sau coda | Tùy chọn chính tả vùng miền |
| Tương thích | Windows TSF; macOS IMK; Linux Fcitx5/IBus | Native app-specific workarounds |
| Trạng thái | VN/EN trên tray/menu bar/status area | Popup cạnh caret tùy chọn |
| Gõ tắt | Từ viết tắt tự chọn, import/export, scope theo app | Biến động, template nâng cao |
| Từ tiếng Anh | Từ điển cơ bản, loại trừ URL/email/path/code tùy app | Personal offline dictionary |
| Chuyển bảng mã | Công cụ chuyển Unicode/TCVN3/VNI Windows | Chuyển hàng loạt trên tệp |
| Profiles | Mặc định chung và override theo ứng dụng | Workspace profile sync bằng tệp |
| Cài đặt | Hotkeys, khởi động cùng OS, auto-start tùy chọn | Portable Windows |
| Privacy | Zero telemetry by default, ẩn/không xử lý secure fields | Báo cáo lỗi tự nguyện có ẩn dữ liệu |
| Update | Gói ký số, kiểm tra update tùy chọn | Delta update ký số |
| Accessibility | UI keyboard-only, screen reader, high DPI | Thêm layout trợ năng |

### Những tính năng thật sự tạo khác biệt

**A. Smart App Profiles**
- Quy định bộ gõ cho từng ứng dụng theo application identity thực, không giả định có thể xem mọi URL đang mở trong trình duyệt.
- Ví dụ: Word bật Telex; VS Code ưu tiên tiếng Anh; game không bắt phím; terminal tắt thay thế gõ tắt theo mặc định.
- Người dùng có thể override ngay từ tray, không cần đi sâu menu.

**B. Smart English Protection — offline, không tự ý sửa**
- Tách token trong URL/email/path/code; tránh biến tên biến `className`, `password`, `access` thành chữ có dấu.
- Từ điển ngoại lệ được lưu cục bộ, mặc định ưu tiên không can thiệp với token không chắc chắn.
- Không tự động gửi văn bản cho AI/cloud để đoán ngôn ngữ.

**C. Text Expansion**
- `@@mail` thành email đã khai báo; `;dc` thành đoạn văn mẫu; cho phép bật/tắt theo app.
- Chỉ hoạt động ở ô văn bản thường, không chạy trong password/OTP/chat bảo mật khi policy cấm.
- Hỗ trợ xem trước, hoàn tác, nhập/xuất JSON không chứa phím gõ lịch sử.
- Có kiểm tra xung đột và câu kích hoạt quá ngắn.

**D. Khôi phục an toàn / Recovery**
- Phím tắt hoàn tác lần chuyển cuối trong vùng preedit, không đụng đến văn bản ngoài quyền của IME.
- Khi app không hỗ trợ preedit, dùng cơ chế fallback được kiểm thử riêng, không tự động bơm backspace ngẫu nhiên.
- Lỗi plugin không được làm mất bàn phím; OS input source khác vẫn dùng được.

**E. Công cụ chuyển mã riêng**
- Giao diện chuyển Unicode NFC, Unicode tổ hợp, TCVN3 (ABC) và VNI Windows khi có thư viện mapping được kiểm thử.
- Tách bạch `encoding conversion` với bộ gõ thời gian thực; không đổi bảng mã nội dung tùy tiện.
- Không tự động đọc clipboard nền; chỉ thao tác khi người dùng bấm.

## 3. Thiết kế UI/UX

### System tray / menu bar

- Icon nhỏ `VN` hoặc `EN`, trạng thái rõ ràng, có tooltip.
- Click trái: bật/tắt tiếng Việt (khi nền tảng cho phép); click phải: menu nhỏ.
- Menu: `Tiếng Việt ✓`, `Tiếng Anh`, `Telex`, `VNI`, `VIQR`, `Tạm dừng`, `Cài đặt...`, `Giới thiệu Lakakey`, `Thoát`.
- Phím tắt để chuyển trạng thái tùy chỉnh và hiển thị cạnh menu; tránh mặc định chiếm tổ hợp phím hệ thống.
- Khi đang gõ, tuyệt đối không hiện quảng cáo, banner, trợ lý AI hoặc toast gây che chữ.

### Cửa sổ Settings (native, mở khi cần)

1. **Tổng quan:** công tắc VN/EN, kiểu gõ, trạng thái khởi động cùng OS, phiên bản và cập nhật.
2. **Bộ gõ:** Telex/VNI/VIQR, kiểu bỏ dấu cũ/mới, cấu hình xử lý từ ngoại ngữ, phương thức hoàn tác.
3. **Gõ tắt:** danh sách, search, thêm/sửa/xóa, phạm vi theo app, import/export, cảnh báo xung đột.
4. **Ứng dụng:** danh sách app detect được, bật/tắt từng profile, lựa chọn mặc định nhanh.
5. **Phím tắt:** cấu hình, kiểm tra trùng OS/app, nút Khôi phục mặc định.
6. **Giới thiệu:** `Lakakey`, phiên bản, `Created by Larx Vo`, liên kết `vovinhlac.com`, bản quyền, chính sách riêng tư, giấy phép.

Giao diện kiểu native, sạch, font hệ thống, icon đủ lớn, label rõ, các nút `Apply` chỉ dùng khi có thay đổi thực sự. Dark/light theo OS; keyboard navigation; hỗ trợ DPI 100–300%, screen reader; không ép font tải qua mạng.

### Luồng cơ bản

`Cài đặt -> chọn input source Lakakey -> VN -> chọn Telex -> gõ văn bản -> đổi EN theo app -> quay lại VN`.

`Đổi tiêu điểm -> commit/huỷ preedit đúng ngữ cảnh -> reset bộ đệm -> gắn session cho ô nhập mới`.

`Password/secure field -> tắt thành phần thông minh -> OS pass-through an toàn`.

## 4. Kiến trúc module

```
Lakakey Native Desktop
  ├── core/                 C++17 Unicode Vietnamese IME
  │    ├── composition     event -> text preedit
  │    ├── tone-rules      modern/traditional syllabic accent
  │    ├── unicode         normalize NFC & grapheme processing
  │    ├── dictionary      optional offline English protection
  │    └── shortcuts       offline snippets and policy
  ├── platform/windows/    Microsoft TSF IME + settings/tray Win32
  ├── platform/macos/      InputMethodKit bundle + Swift menu/settings
  ├── platform/linux/      Fcitx5 addon & IBus engine + settings
  ├── shared/config/       versioned JSON config, migration
  ├── shared/diagnostics/  opt-in local diagnostics without keystrokes
  ├── tests/               unicode regressions + app integration + fuzz
  └── packaging/           signed MSI/EXE, notarized macOS package, deb/rpm
```

**Nguyên tắc:** UI có thể khởi động riêng hoặc đóng; phím gõ vẫn xử lý bởi IME. Không viết network I/O, file I/O hoặc gọi tới UI/blocking IPC trong luồng xử lý phím. Giữ mỗi session theo từng context; phiên này không được làm sai vị trí con trỏ phiên khác.

## 5. Cấu hình dự kiến

```json
{
  "schema_version": 1,
  "input": {
    "enabled": true,
    "scheme": "telex",
    "unicode_normalization": "NFC",
    "tone_placement": "modern",
    "english_protection": "conservative"
  },
  "hotkeys": {
    "toggle_vietnamese": null
  },
  "privacy": {
    "telemetry": false,
    "store_typed_text": false,
    "online_ai": false
  },
  "launch_at_login": false,
  "profiles": [],
  "text_expansions": []
}
```

`null` hotkey nghĩa là tránh tự ý chiếm shortcut hệ thống; đề xuất các tổ hợp an toàn theo OS ở lần cài đặt đầu.

## 6. Mục tiêu định lượng (chưa được xác minh là số đo trên OS thật)

- Lõi xử lý một phím: p95 < 1 ms ở các âm tiết thông thường, đo ở cả x86_64 và ARM64.
- CPU idle của thành phần resident: tiệm cận 0%, mục tiêu < 0.2% khi chờ không gõ.
- Mức RAM cho resident utility ở cấu hình cơ bản: **mục tiêu** dưới ~20 MB, chưa tính process host/IME và tài nguyên frameworks OS.
- Không có network khi gõ, crash-free sessions >= 99.99% trong test có thu thập được số liệu phù hợp.
- Không mất/dư ký tự trong bộ kiểm thử hành vi chuẩn, kể cả phím tắt, paste, xóa, composition, focus change.
- Gói ứng dụng và startup tối ưu theo từng OS; tránh đưa Chromium/Electron vào runtime.

Mọi chỉ số chỉ được công bố là số thực đo sau khi benchmark trên máy Windows/macOS/Linux. Không lấy kích thước `lakakey_lab` làm RAM của bản sản phẩm cuối.

## 7. Điều không làm

- Không chạy AI cloud thường trực trên phím người dùng.
- Không ghi keylogging/debug logs có nội dung gõ.
- Không truy cập clipboard vô điều kiện.
- Không tự động chuyển toàn bộ text sang font/bảng mã cũ.
- Không dùng global injection hook làm con đường gõ chính nếu OS có input framework phù hợp.
- Không cài background service đặc quyền cao nếu không có lý do bắt buộc.
- Không yêu cầu phải truy cập website của tác giả khi chạy app.
