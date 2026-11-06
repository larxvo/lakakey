# Lakakey 0.13 — Windows Compatibility and Native Input QA

Maintainer: **Larx Vo** · https://vovinhlac.com · © 2026

## Delivered source changes

1. **Layout-aware key translation** — TSF reads the active Windows keyboard layout via `ToUnicodeEx` rather than assuming US/QWERTY. The no-state-change flag protects dead-key handling; complex/Unicode symbols and punctuation remain with the host.
2. **Single TSF route** — `OnTestKeyDown` and `OnKeyDown` continue to share the same policy and translated character. Boundary/navigation keys are never swallowed.
3. **Focus composition cleanup** — when the host changes focus or switches contexts, the TSF adapter attempts to finalize the previous composition in the previous context before dropping the local state. Rejected edit sessions do not replay keystrokes into the next field.
4. **Win32 security check** — text-service status and native password EDIT style are checked before composing. Password behavior in browsers, Electron and other frameworks still requires testing.
5. **Non-registering COM smoke** — `LakakeyTsfComSmoke.exe` loads the experimental DLL, creates the COM text service and key/focus sinks, and verifies reference cleanup. It does not register or activate the input method.
6. **Keyboard translation smoke** — checks ASCII, Shift, CapsLock, numeric keys, and punctuation pass-through against an installed US keyboard layout on Windows runners.
7. **Native manual QA host** — regular text, multiline and password controls, built only in the experimental TSF configuration.
8. **Quiet desktop diagnostics** — a built-in panel shows whether the experimental TSF CLSID has a per-user registration. Registration is explicitly not the same as a verified active input service.
9. **Reproducible Windows Actions** — native C++ tests, Go tests, Windows desktop installer and isolated TSF research binaries. No implicit text-service registration on the user machine.

## How to build

Windows 10/11 with Visual Studio 2022, Windows SDK and Go 1.23 or newer:

```powershell
cmake -S . -B build-win -A x64 -DLAKAKEY_BUILD_TESTS=ON -DLAKAKEY_BUILD_TSF_V2=ON
cmake --build build-win --config Release --target LakakeyTsfV2 LakakeyTsfSmokeHost LakakeyTsfComSmoke LakakeyKeyboardLayoutSmoke lakakey_contract_tests lakakey_tsf_policy_tests
ctest --test-dir build-win -C Release --output-on-failure

go test ./desktop/windows
go build -trimpath -ldflags "-s -w -H=windowsgui" -o desktop/installer/payload/LakakeyDesktop.exe ./desktop/windows
go build -trimpath -ldflags "-s -w -H=windowsgui" -o LakakeyDesktopSetup.exe ./desktop/installer
```

The user-facing desktop installer **does not ship or register the experimental TSF DLL**. Research DLLs and QA executables are separate GitHub Actions artifacts.

## End-to-end Windows input acceptance matrix (NOT YET VERIFIED)

| Input scenario | Expected behavior |
| --- | --- |
| Notepad/plain EDIT | Telex/VNI syllables appear once, with correct accents and caret |
| Multiline EDIT | Enter, arrow keys, Home/End, selection and Backspace are never lost |
| Native password EDIT | All input passes to host without composition or logging |
| Browser password | No IME composition in password field across Chrome/Edge |
| Office editor | Correct composition, selection and undo stack |
| VS Code | Typing, search bar, terminal and shortcuts remain intact |
| Focus changes | Prior field never leaks preedit into the next field |
| Layout changes | US, Vietnamese and non-US keyboard layouts preserve characters |
| Modifier shortcuts | Ctrl/Alt/Win combinations pass through |
| Disabled mode | Original keys are not consumed or remapped |
| Explorer restart | Tray restores without duplicating process |
| Update/rollback | No loss of configuration and no orphan TSF registration |

## Required evidence for a functional input release

- Windows 10 and Windows 11 actual-host testing with video/repro steps; not merely successful C++ compilation.
- Test results on password and sensitive inputs. **Do not use the experimental TSF build on a daily-use machine.**
- Zero dropped/doubled keys across the deterministic typing corpus, including selection and focus changes.
- Verify input profile installation/activation/uninstallation and recovery in a disposable Windows VM.
- Code-signing with the verified publisher identity before public stable distribution.
- No collection, storage or transmission of content that the user types.

## Build status terminology

- **Compiled:** the binary was produced by the compiler.
- **Smoke checked:** COM factory and input policy checks pass without system registration.
- **Runtime verified:** tested inside live Windows text fields.
- **Release ready:** all compatibility, privacy, installer and signing gates pass.

Only the last two establish working cross-application input. The first two do not.

© 2026 Larx Vo · https://vovinhlac.com
