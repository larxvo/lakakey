# Lakakey v0.7 — Cross-platform input groundwork

Copyright © 2026 Larx Vo. Author: https://vovinhlac.com. Free software project.

## Implemented
- Portable `lakakey::InputSession` adapter contract with focus reset, secure-field bypass, Telex/VNI processing, preedit/commit semantics.
- New session tests; existing engine tests preserved.
- Windows experimental TSF COM DLL source (`LakakeyTsf`): class factory, text-service activation, keystroke sink wiring, per-user registration hooks, edit-session helper scaffold.
- macOS InputMethodKit and Linux Fcitx5/IBus implementation requirements documented.
- Existing 3D-rendered mascot assets and Windows mascot source preserved from v0.6.

## Critical status
**NOT YET A FUNCTIONAL SYSTEM-WIDE INPUT METHOD.** The TSF key sink intentionally does not consume keys or commit text because complete TSF composition/context lifecycle and host compatibility are not implemented. Windows TSF code has NOT been compiled or executed on Windows in this environment. Do not install this DLL as a daily-use input method; experimental registration may affect input settings. The macOS/Linux frontends are NOT implemented. No universal app compatibility claim is made. Mascot assets are image-based animation, not independently rigged 3D models.

## Build & test (portable engine)
`cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure`

## Required before a usable release
1. Implement Windows TSF edit sessions, composition start/update/end, correct keyboard layout and modifier handling, per-context engine state, focus reset, language bar and secure-field policy.
2. Test Windows 10/11 x64/ARM64 in Notepad, Office, Chrome, Edge, Firefox, VS Code, Electron apps, browsers' password fields, RDP, elevated apps and accessibility tools.
3. Implement native InputMethodKit bundle on macOS, Fcitx5 and IBus adapters on Linux; validate Wayland and X11 separately.
4. Implement independent rigged 3D animations, optimized atlas/texture streaming and Windows overlay QA; preserve Full/Minimal/Off and reduced-motion support.
5. Run long-duration CPU/RAM/performance tests, accessibility QA, installer/uninstaller and signed release builds.
