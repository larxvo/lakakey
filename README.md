## Windows desktop beta 0.10.1

The latest native Settings and tray changes are under review in [PR #1](https://github.com/larxvo/lakakey/pull/1). This beta has quiet installation, inline help/about, a compact/expanded panel and a configurable Alt+Z desktop-state switch. System-wide input integration remains in development and is not activated by this package. See the PR for the exact source and CI results.

# Lakakey 🐱 — Vietnamese Input Method

**Free Vietnamese typing software in development for Windows · macOS · Linux**

**Created by Larx Vo** · [vovinhlac.com](https://vovinhlac.com) · © 2026 Larx Vo

> **Important release status — v0.8 PREVIEW:** The downloadable installer launches a **Windows Settings demonstration** with VN/EN controls, six accents, and a mascot setting (Full / Minimal / Off). It **does not yet type Vietnamese system-wide** in Word, Notepad, Chrome, Edge or VS Code. Keep using các bộ gõ khác for typing until a real TSF IME is completed and validated. No claim of các bộ gõ khác replacement is made for this preview.

## 📥 Download & install on Windows

**[⬇ Download LakakeyPreviewSetup.exe — Windows x64 (GitHub prerelease)](https://github.com/larxvo/lakakey/releases/download/v0.8.0-preview/LakakeyPreviewSetup.exe)**

1. Download the EXE using the link above or open [Releases](https://github.com/larxvo/lakakey/releases/tag/v0.8.0-preview).
2. Double-click `LakakeyPreviewSetup.exe`.
3. Follow the installer instructions.
4. The Settings preview opens. To uninstall, open **Windows Settings → Apps → Installed apps → Lakakey Desktop Preview**.

For **source code**, see this repository; for CI artifacts, visit [Windows Preview Build](https://github.com/larxvo/lakakey/actions/workflows/windows-preview.yml).

The GitHub Actions **Windows build** and **Linux C++ core tests** passed for commit `7229b56a`. This verifies compilation and core unit tests, **not** working Vietnamese input or real-device UI compatibility. Binaries are **unsigned**. Code-signing certificates and Microsoft Store publication are **not required** to build or distribute a Windows EXE. SmartScreen can still warn for unfamiliar unsigned downloads; review the origin and SHA-256, and do not disable Windows protections. An optional future signing process does not block free development or GitHub distribution. This is a prerelease, **not a stable IME**.

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
