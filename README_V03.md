# Lakakey v0.3 — Windows-first upgrade

- Preserves the C++17 engine and tests from v0.2.
- Adds a Windows-native Win32 settings executable, a system-tray settings menu, theme mode and six selectable accent colors, persistent local preferences, and a clickable author website.
- Adds Windows VERSIONINFO copyright metadata: Larx Vo, vovinhlac.com.
- Existing browser prototype remains under `ui/index.html`.
- The Settings app is **not** an IME. No TSF integration, no global typing support, no Windows binary built in this Linux environment.
- The theme selection is saved, but Win32 controls currently use standard Windows button rendering; accent color is represented by selection state, not a fully custom-drawn design system.

Next: implement TSF COM text service, composition lifecycle, reconversion, per-app handling, secure-field behavior, installation/uninstallation and Windows QA.
