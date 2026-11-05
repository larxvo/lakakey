# Lakakey v0.6 — 3D Mascot Animation Runtime + TSF diagnostics

## Implemented
- 12 distinct **3D-rendered 2D sprite animation sequences**, 12 frames each (144 RGBA PNGs), including welcome, VN, EN, Unicode, Telex, VNI, typing, success, notice, follow, sleep and error.
- Character motion uses a generated 3D-rendered GIF plus 2D compositing/transforms and state-specific overlays; **not twelve independently rigged 3D character animations**. Character poses vary most in welcome/notice; other actions rely on transformations, overlay symbols, and timing. No 3D model/rig source is included.
- Win32 layered window draws event-specific animation sequences with per-pixel transparency; no keyboard hook and no keyboard-thread rendering.
- Full / Minimal / Off retained, including short auto-hide and click-through/no-activate behavior.
- Windows TSF diagnostic program `LakakeyTsfProbe` activates/deactivates TSF thread manager. This is **not** a TSF text service, is not registered as an IME, and cannot type Vietnamese into other applications.
- C++17 engine retained, with tests runnable on Linux.

## Build
Windows 10/11 with Visual Studio 2022 C++ desktop workload:
```
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Run `LakakeyMascot.exe` and send events with `LakakeyMascotEvent.exe welcome`, `... unicode`, etc. Run `LakakeyTsfProbe.exe` to check TSF activation. Source-only package: no precompiled Windows binaries.

## Important validation gaps
- The environment used to prepare this release is Linux; **Windows binaries were not compiled or executed**, so Windows runtime behavior, click-through and alpha blending are not verified on real Windows.
- The sprite asset size is approximately 6 MB on disk; further optimization may be required. The runtime now decodes only the active 12-frame sequence on demand and releases the previous one; further optimization and memory profiling on Windows are still needed.
- The v0.5 Settings and HTML Mascot Studio still require synchronization and Windows UX integration. The Windows TSF input processor, key event sink, composition lifecycle, language profile registration, installer, and signed builds remain future work.
- Source and assets are local-first, no telemetry or keylogging.

Copyright © 2026 Larx Vo · https://vovinhlac.com · Free to use.
