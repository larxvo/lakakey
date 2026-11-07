# Lakakey 0.14.1 — Branded one-click Windows installer

## Deliverable
A **single Windows x64 Setup EXE** opens the real Inno Setup wizard. The
official Lakakey logo is embedded in the Setup PE resource using
`SetupIconFile`; the desktop application uses a Go-linkable Windows resource
generated from the same branded ICO. Icons in Explorer, Start Menu,
Add/Remove Programs and installer UI are consistent.

### Brand asset
`ui/assets/brand_3d_reference.png` → `tools/build_icon.py` →
`assets/windows/lakakey.ico` (16–256 px).

### Build
`tools/windows/build_resources.ps1` invokes a pinned `rsrc` Go resource
compiler to embed the ICO into `desktop/windows/lakakey.syso`, which the
Go linker includes in `LakakeyDesktop.exe`. This file is generated, not
committed. The Inno Setup compiler also embeds the ICO into the Setup EXE.

The Windows workflow builds `LakakeyDesktop.exe`, installs Inno Setup 6,
compiles `installer/Lakakey.iss`, and emits:
`dist/windows/Lakakey-Setup-0.14.1-Windows-x64.exe`.

`tools/windows/verify_embedded_icon.py` checks x64 PE resources for
`RT_ICON` and `RT_GROUP_ICON` in both EXEs. GitHub Actions installs and
uninstalls the Setup EXE silently on a disposable Windows runner, then
uploads the installer with a checksum manifest and build identity.

### User flow
Double-click setup → welcome/installation → finish/optional launch.
Start Menu shortcut is created. Desktop shortcut is optional. Uninstall
is available from Windows Installed apps. The old desktop-only install
entry can be migrated without deleting user preferences.

### Safety boundary
This is **installer polish**, not system-wide Vietnamese input readiness.
The regular Setup EXE contains no experimental TSF DLL and never
registers an input service. Code signing and live text input QA still
remain before a stable public release.

© 2026 Larx Vo · https://vovinhlac.com.
