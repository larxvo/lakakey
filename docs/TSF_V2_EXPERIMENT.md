# Lakakey Windows TSF v2 — development experiment

© 2026 Larx Vo · https://vovinhlac.com

**Not a public release. Not for daily use. A per-user experimental EXE is built only as a CI artifact. It has NOT passed real Windows typing tests.**

This branch introduces an opt-in COM Text Services Framework (TSF) DLL experiment.
Its purpose is to compile and then validate genuine in-app composition editing,
rather than showing a Settings demo and calling it a system IME.

## Code architecture

- `platform/windows/tsf/lakakey_tsf_v2.cpp`: native COM service, TSF key sink,
  composition sink and synchronous edit-session handling.
- `lakakey::Engine`: independent C++17 Telex/VNI core.
- The service attempts to start an `ITfComposition`, replace preedit using an
  `ITfRange`, update selection and finalize composition at word boundaries.
- It declines secure-field input where the host advertises `TF_SS_PASSWORD`,
  passes through keyboard shortcuts, avoids unknown keyboard layouts,
  and does not intentionally install global hooks or collect keystroke logs.
- The build is opt-in via `-DLAKAKEY_BUILD_TSF_V2=ON`.

## Mandatory validation before any installer

1. Verify Windows x64 MSVC compilation and portable engine unit tests in CI.
2. Run a disposable Windows VM (not the user's daily machine).
3. Check TSF registration and full rollback on installation/uninstallation.
4. Verify Unicode preedit and composition termination for Telex/VNI in Windows
   Notepad, Word, Chrome, Edge and VS Code, including focus changes.
5. Test disabled/secure-field handling, Shift/CapsLock, AltGr layouts, accents,
   numeric VNI, punctuation, arrow-key commits, repeated keys and undo.
6. Test apps refusing synchronous edit sessions: no lost or duplicated characters.
7. Validate all keyboard layouts, accessibility, crash isolation and no data loss.
8. Test per-user installation, TSF profile registration, failed registration rollback,
   and uninstall on disposable Windows 10/11 test virtual machines.
9. Add an input-source picker, reliable TSF registration lifecycle, application QA,
   and a signed public release only when the above tests pass.

## Known risks / incomplete work

- Not verified on a Windows host; no reliable app-compatibility claims.
- TSF lifetime, unexpected host callback reentrancy, and focus-related composition
  cleanup require stress tests.
- `OnTestKeyDown` may advertise consumption ahead of a rejected edit session.
- Keyboard mapping is limited to basic US A-Z and digits; IME handling for
  punctuation, non-US layouts and AltGr is incomplete.
- An application can hold its own selection state: repeated edits may break.
- Engine rule coverage is still partial. VIQR and full English protection missing.
- Password fields can be implemented inconsistently across applications.
- **Do not deploy or use the experimental installer on a daily-use computer.**
  CI builds an unsigned, unverified EXE artifact for controlled disposable VM testing.
  It is NOT distributed as a GitHub Release. Registration can fail, and setup/uninstall
  rollback has NOT been validated.

The existing `v0.8.0-preview` remains explicitly Settings-only and is not upgraded
to a working IME by merging this prototype.

## Windows alpha package (developer-only)

The build workflow compiles C++17 core and TSF DLL, runs unit tests, then uses
Inno Setup to generate `LakakeyNativeAlphaSetup-x64.exe`. If the workflow passes,
it is stored for 30 days under the named GitHub Actions artifact
`Lakakey-Native-Alpha-Windows-x64-UNVERIFIED`.

Install logic is per user (`PrivilegesRequired=lowest`), copies the TSF DLL to
`%LOCALAPPDATA%\\Programs\\LakakeyNativeAlpha`, then invokes 64-bit `regsvr32`
for registration. Uninstall invokes `regsvr32 /u` before file deletion.
No legacy keyboard or UniKey files are modified.

**Caveat:** If Windows TSF rejects per-user profile registration, or setup fails
mid-transaction, the package may not activate or clean up fully. This is an
unvalidated QA build, not an approved public installer. Use an expendable VM only.
