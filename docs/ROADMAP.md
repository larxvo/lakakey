# Lakakey — Lộ trình kỹ thuật

### Giai đoạn A — Core xác minh (foundation 0.1, đang có)

Engine C++17 không phụ thuộc package, Telex/VNI cơ bản, Unicode UTF-8/NFC precomposed, C ABI, lab + unit tests. Tồn tại: nhiều ngoại lệ chính tả, chưa bảo vệ tiếng Anh, chưa kiểm thử các OS khác.

### Giai đoạn B — Core production-ready

- Bộ dữ liệu 5.000–20.000 ca test có kỳ vọng dấu đúng từ corpus tiếng Việt, kể cả typo/English mixing.
- Sửa rules diacritic theo vần, dấu khi đặt cuối từ, các dấu bật/tắt và khả năng undo.
- Normalize NFC/NFD với Unicode segmentation đúng, bổ sung VIQR và custom Telex mapping.
- Token detection URLs/email/codes, dictionary tiếng Anh offline + exceptions.
- Fuzz/performance/stress tests trên input events và các chuỗi Unicode đối kháng.

### Giai đoạn C — Windows usable beta

- Viết native TSF IME + session manager và settings/tray.
- Installer, đăng ký input source, setup VN/EN, hotkeys, profiles.
- Tương thích Notepad, Office, browsers, Terminal/VSCode; password bypass.
- Kiểm thử trên Windows thật, build x64 và ARM64 nếu khả thi.

### Giai đoạn D — macOS usable beta

- InputMethodKit wrapper và macOS menu bar/settings Swift.
- Packaging, ký số/notarize, thử Intel/ARM64; xử lý security mode.

### Giai đoạn E — Linux usable beta

- Fcitx5 engine & IBus implementation; settings native; distro packaging.
- Thử GNOME/KDE, X11/Wayland, Chromium/GTK/Qt.

### Giai đoạn F — Lakakey 1.0

- Text expansion + offline English protection bảo thủ.
- App profiles, onboarding, import/export settings, bộ chuyển bảng mã offline.
- QA, bảo mật, performance metrics theo từng OS, tài liệu sử dụng, signed release.

### Mức ưu tiên

- **P0:** Unicode đúng, không mất chữ, native IME, privacy, fallback keyboard.
- **P1:** app profiles, shortcuts, undo, easy Settings, high DPI.
- **P2:** converter, advanced snippets, custom dictionary, portable distribution.
- **Không làm trước 1.0:** AI cloud, browser extension, sync online, bộ điều khiển ngữ cảnh intrusive.
