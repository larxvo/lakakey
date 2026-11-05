# Lakakey Desktop v0.9.0-beta — Windows native UI & tray

Author: **Larx Vo** — https://vovinhlac.com — © 2026.

## Delivered in this update

- A new Win32 Settings window with compact/expanded views, Unicode selection, Telex/VNI selection, shortcut preferences, local JSON config and an About panel.
- A native system tray utility with VN/EN **preview** state, menu, open Settings, and Exit. Icon is a temporary VN/EN badge; it is **not** the official 3D mascot.
- A one-click per-user `LakakeyDesktopSetup.exe` that installs an executable, adds a Start Menu shortcut, Windows Installed Apps metadata and a reversible uninstaller.
- Optional real Windows startup for the UI/tray utility, managed under HKCU Run.
- Windows x64 GitHub Actions build and unit tests. No network calls, no keyboard hook, no password-field monitoring.

## Important limitations

**This is a desktop settings/tray beta, NOT a working system-wide Vietnamese input method.** Telex/VNI are only selectable settings until the Windows TSF adapter is complete. Other listed input methods/encodings are placeholders that cannot be selected. Mascot runtime and hotkey handling are unfinished.

The native TSF composition code lives under `platform/windows/tsf/` and must be reviewed, fuzzed and tested on real Windows installations before it is included in any installer. Never register experimental TSF code for daily use.

## Build (Windows 10/11 x64 or Linux cross-compile)

Requires Go 1.23+. In the repository root:

```powershell
go test ./desktop/windows
go build -trimpath -ldflags '-s -w -H=windowsgui' -o desktop/installer/payload/LakakeyDesktop.exe ./desktop/windows
go build -trimpath -ldflags '-s -w -H=windowsgui' -o LakakeyDesktopSetup.exe ./desktop/installer
```

For cross-compilation from Linux, set `GOOS=windows GOARCH=amd64 CGO_ENABLED=0` for each build. The complete installer embeds the app and `desktop/installer/payload/README.txt`.

## Install/uninstall

1. Download **LakakeyDesktopSetup.exe** from the release or Actions artifact.
2. Run the installer and read the preview warning. It installs under `%LOCALAPPDATA%\Programs\Lakakey`.
3. Start from Windows Start Menu (`Lakakey`) or the tray icon. Closing Settings hides the window and leaves the tray running.
4. Right click the tray icon to exit the app; uninstall from Windows Settings → Installed apps → Lakakey Desktop Beta.
5. Installer is unsigned and has not been exercised on a physical Windows machine. Avoid bypassing Windows security warnings without verifying file provenance.

For development source changes see `.github/workflows/windows-desktop-beta.yml`. The existing v0.8 settings-only preview remains separate.
