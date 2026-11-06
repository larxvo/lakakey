# Lakakey 0.12 — Native Input Integration (developer milestone)

Maintainer: Larx Vo | https://vovinhlac.com | © 2026

## Implemented

- Deterministic C++ key-routing contract shared by TSF `OnTestKeyDown` and `OnKeyDown`.
- Navigation and punctuation are not swallowed. Finalization is attempted before host processing.
- Failed synchronous edit sessions pass original keys back to the application.
- Unit tests cover disabled mode, sensitive contexts, shortcut modifiers, focus mismatch and backspace.
- Native desktop writes only Enabled and Scheme REG_DWORD with advapi32 (no reg.exe subprocess per mode change).
- Win32 QA host with regular, multiline and password fields.
- Windows Actions builds TSF research DLL and QA host separately from user installer.

## Verification

- C++17 TSF policy test compiled with `-Wall -Wextra -Werror` and passed locally on Linux.
- CTest policy harness passed locally.
- Go desktop model tests passed locally on Linux.
- Windows x64 desktop and installer cross-compiled to PE32+.
- Windows MSVC pipeline, protected field behavior and native app interoperability are **pending verification**.

## Important release boundary

This desktop installer DOES NOT install or register the experimental TSF DLL. It provides Settings and tray. A working system-wide input method is **not yet established** by these tests. Do not install the research DLL on a daily-use computer.

## Windows manual QA (isolated test VM)

1. Test regular and multiline text input with Telex and VNI in the native QA host.
2. Verify backspace, cursor movement, selection, Enter, Tab, punctuation and focus changes without missing or doubled characters.
3. Test password and security-sensitive inputs; if any keystroke is intercepted, block release.
4. Test US/Vietnamese layouts, CapsLock, Shift, AltGr, dead keys and shortcuts.
5. Test Notepad, browsers, office editors and VS Code separately.
6. Test Explorer restart, disabling while composing, uninstall and revert to previous input source.

## Remaining engineering gates

- Confirm successful Windows MSVC build and runtime QA.
- Review composition termination and focus lifecycle.
- Implement reliable secure input detection; present status checks alone are not sufficient.
- Pass cross-application QA, installer recovery, signing and publication review.

© 2026 Larx Vo — https://vovinhlac.com
