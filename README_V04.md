# Lakakey v0.4 — Mascot Studio (Windows-first)

## What's implemented
- 3D mascot sprite assets (12-frame waving sequence), derived from the existing Lakakey 3D artwork.
- Interactive `ui/mascot-studio.html` preview: Full / Minimal / Off, 12 event demos, accent color, cursor-follow demonstration, reduced motion, settings persistence via browser localStorage.
- Windows Win32 Settings source: Full / Minimal / Off selector saved to `%APPDATA%\\Lakakey\\settings.ini` as `mascot_mode=0|1|2`.
- Original C++17 input core preserved.

## Run
Open `ui/mascot-studio.html` in a modern browser. No network needed.
To compile C++ engine and tests: `cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure`.

## Limitations
This is a working **mascot settings/UI prototype**, not a Windows TSF input method. Windows native mascot rendering/overlay is not implemented. The HTML UI does not automatically synchronize with Windows settings.ini. The waving frames are real image assets; other events use the same 3D mascot with CSS motion and message variations rather than separate rigged 3D animations. The asset board is reference material, not a rigged 3D model. Native Windows build has not been tested here. Mascot never captures keystrokes; cursor-follow demonstration is limited to the preview area.
Copyright © 2026 Larx Vo — https://vovinhlac.com — Free to use.
