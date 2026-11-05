# Lakakey 🐱 — Vietnamese Input Method

**Free Vietnamese typing software in development for Windows · macOS · Linux**

**Created by Larx Vo** · [vovinhlac.com](https://vovinhlac.com) · © 2026 Larx Vo

> **Important release status — v0.8 PREVIEW:** The downloadable installer launches a **Windows Settings demonstration** with VN/EN controls, six accents, and a mascot setting (Full / Minimal / Off). It **does not yet type Vietnamese system-wide** in Word, Notepad, Chrome, Edge or VS Code. Keep using UniKey for typing until a real TSF IME is completed and validated. No claim of UniKey replacement is made for this preview.

## 📥 Download & install on Windows

1. Open **[Actions → Windows Preview Build](https://github.com/larxvo/lakakey/actions/workflows/windows-preview.yml)**.
2. Open the latest **successful** workflow run; under **Artifacts**, download `LakakeyPreview-Windows-x64`.
3. Unzip the downloaded artifact, then double-click `LakakeyPreviewSetup.exe`.
4. Read the preview warning and choose **Yes** to install to your user profile (`%LOCALAPPDATA%\Programs\LakakeyPreview`); no administrator permission is intended.
5. The Settings preview opens automatically. Use **Windows Settings → Apps → Installed apps** to uninstall **Lakakey Desktop Preview**.

**There is no verified public stable release yet.** Action artifacts may require a GitHub login. Binaries are presently unsigned; Windows may show a SmartScreen warning. Only run installers from sources you trust, and do not bypass security warnings without verifying origin.

A prebuilt preview executable has also been provided as a direct chat attachment for testing; the GitHub workflow creates new installer artifacts from the checked-in Go source. These are independent builds.

## Current capabilities

| Module | Status |
|---|---|
| Portable C++17 Vietnamese engine (Telex, VNI baseline) | Core implemented; tests still require platform QA |
| Windows native Settings preview | Source provided; builds to .exe on Windows Actions |
| Mascot branding | UI concept and Full / Minimal / Off configuration; not a complete rigged 3D runtime |
| Windows TSF native IME (system-wide typing) | **Not complete** |
| macOS InputMethodKit | Planned |
| Linux Fcitx5 / IBus | Planned |
| Installer signing, QA on Windows 10/11 | **Not verified** |

## Repository layout

- `winpreview/app/main.go` — native Win32 Settings preview (Go standard library; no Electron).
- `winpreview/installer/main.go` — one-click self-contained per-user setup/uninstaller.
- `winpreview/installer/payload/README_PREVIEW.txt` — clear installer warning.
- `.github/workflows/windows-preview.yml` — Windows x64 build and downloadable artifacts.
- `docs/INSTALL_VI.md` — Vietnamese install/uninstall guide.
- `docs/STATUS.md` — implementation truth table and limitations.
- `docs/ROADMAP.md` — path toward real multi-platform IMEs.
- `COPYRIGHT.md` — authorship and usage statement.

## Building the Windows preview locally

Install **Go 1.23+**. From the repo root, in PowerShell on Windows:

```powershell
New-Item -ItemType Directory -Force winpreview/installer/payload | Out-Null
go build -trimpath -ldflags '-s -w -H=windowsgui' -o winpreview/installer/payload/LakakeyPreview.exe ./winpreview/app
go build -trimpath -ldflags '-s -w -H=windowsgui' -o LakakeyPreviewSetup.exe ./winpreview/installer
```

The build must include `winpreview/installer/payload/mascot.bmp` and `README_PREVIEW.txt`. A fallback image is generated automatically in the CI workflow. The preview executable itself does not rely on a network connection.

## Security & privacy

The intended product is offline-first, free to use, with no account or ads. This preview does not install a keyboard hook or intercept input. The buttons marked VN/EN only update demonstration UI state. The mascot preview is optional. Never enter passwords into experimental input engines.

## Contributors and license

This is a **public development repository**, not a blanket grant of open-source redistribution rights. The author has **not yet selected an open-source license**; source may be viewed, but copying/redistribution is not automatically licensed. The compiled preview is offered free to use for testing. Do not remove the Larx Vo attribution.

For issues, please use GitHub Issues and include Windows version, exact steps, and screenshots without sensitive content.

© 2026 **Larx Vo** — [vovinhlac.com](https://vovinhlac.com). Free software, clearly attributed.
