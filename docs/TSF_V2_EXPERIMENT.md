# Lakakey Windows TSF v2 — development experiment

© 2026 Larx Vo · https://vovinhlac.com

**Not a release. Not for daily use. No installer or registration is supplied.**

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
8. Add an input-source picker/enable control, signed release pipeline, and a
   reversible per-user installer only after the tests above pass.

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
- **Do not register or deploy this DLL to end users.** It is built by
  CI only as a compile test, with no installer and no release artifact.

The existing `v0.8.0-preview` remains explicitly Settings-only and is not upgraded
to a working IME by merging this prototype.
