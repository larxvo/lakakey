# Lakakey v0.5 — Windows Native Mascot Runtime

© 2026 Larx Vo — https://vovinhlac.com — free to use.

## Implemented in source

- `LakakeyMascot.exe`: separate Win32 native process, GDI+ PNG compositor with a per-pixel-alpha layered window. `WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW`: does not take focus, is click-through, does not appear on taskbar.
- Animated 12-frame 3D-rendered waving sprite; native 80 ms frame timer (only active while event is visible), automatically hides after 1.28 s (Minimal) or 2.56 s (Full).
- Full/Minimal/Off mode loaded from `%APPDATA%\Lakakey\settings.ini`; Off never displays and hides current animation, Minimal allows essential events only, Full allows all.
- Events delivered via `WM_COPYDATA` from Windows Settings or `LakakeyMascotEvent.exe` smoke-test utility. Does not intercept keyboard, screen content or global mouse hooks. Cursor-follow uses a periodic `GetCursorPos` sample **only during an explicit Full-mode follow event**, and ends automatically.
- GDI+ loads RGBA PNG assets from `assets` alongside the runtime. Assets generated from existing 3D-style sprite artwork, not a rigged 3D mesh.
- Windows Settings starts companion when available and sends language toggle/mascot preview events; the VN/EN toggle is **still a Settings preview**, not a system input method.
- Separate Windows executable version metadata with copyright and author URL.

## Build (Windows 10/11, Visual Studio 2022)

```powershell
cmake -S . -B build-win -G "Visual Studio 17 2022" -A x64
cmake --build build-win --config Release
```

Run `build-win\Release\LakakeyMascot.exe`, then `build-win\Release\LakakeySettings.exe`. In a separate terminal:

```powershell
build-win\Release\LakakeyMascotEvent.exe welcome
build-win\Release\LakakeyMascotEvent.exe unicode
build-win\Release\LakakeyMascotEvent.exe follow
```

If the companion does not start, verify `assets\wave_00.png` through `wave_11.png` exist next to `LakakeyMascot.exe`.

## Engineering boundaries / next milestones

- Native Windows binaries **not compiled or visually tested on Windows in this environment**; source is provided for Windows build and QA.
- No actual TSF/IME Windows input integration yet. The input core remains independent.
- This is an actual Win32 overlay runtime in source, **not** a fully rigged 3D animator: only the wave event has distinct 3D-rendered frames; other event IDs currently reuse static/wave frames. Build separate 3D-rendered frame sequences for Unicode/Telex/VNI, typing, sleep, jump, hover, etc., and load them through a frame-manifest before claiming complete animation coverage.
- No global cursor tracking while hidden. No keylogging. No online service. No installer, startup task, or autoupdate.
- Verify alpha edges, multi-monitor DPI, high-contrast accessibility, secure desktops, event spoofing boundary, process lifetime, GPU/CPU usage and application focus on Windows 10/11 before release.
