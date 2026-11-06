# Lakakey 0.14 — Atomic Typing Transactions & Windows Compatibility

Created by **Larx Vo** · https://vovinhlac.com · © 2026

## Key improvement: Space commits the entire preedit plus separator

In earlier TSF prototypes, `OnTestKeyDown` returned **not eaten** for
Space, while `OnKeyDown` attempted to finalize composition. Some
hosts need not call `OnKeyDown` after `OnTestKeyDown` declines the key.

0.14 adds a distinct `CommitSpace` action:
1. The common `plan_key` function checks whether a composition is active.
2. When Space ends a word, `OnTestKeyDown` and `OnKeyDown` both declare
   that the key is handled.
3. One TSF edit session calls the portable engine with a space.
4. The engine returns the committed word **including one trailing space**.
5. The host text range is updated and the composition is ended.
6. If the edit session is rejected, the TSF adapter does not consume the key.

Printable ASCII punctuation uses the same native commit transaction when a
composition is active; otherwise punctuation stays with the host. Enter,
Tab, cursor movement and other navigation remain the host's responsibility
and still require application-specific verification.

## Verification / regression coverage

- `tests/test_tsf_policy.cpp`: Space is consumed only with active, nonempty
  composition and an enabled, nonsecure, matching input context.
- `tests/test_typing_pipeline.cpp`: Host-neutral Telex/VNI/punctuation transaction
  simulation, word+separator, tone undo, password/disabled pass-through,
  shortcut bypass and focus reset.
- Existing core tests, Windows keyboard layout checks, TSF COM smoke and
  ephemeral register/unregister checks remain part of CI.
- GitHub Actions runs Windows native and Linux core tests; the experimental
  DLL and desktop installer remain separate artifacts.

## Remaining prerequisites for system-wide release

The ordinary 0.14 installer is still the native Settings/tray controller,
not an installed/activated Windows input service. Automated transaction tests
do **not** prove that typing works in a real Windows editor. Before enabling
native input for users, verify in an isolated Windows machine:

- Notepad: uppercase, tones, caret, Delete/Backspace and punctuation.
- Chrome/Edge: plain input, contenteditable, address bar, password fields.
- Word: composition underline, selection, undo/redo, document switching.
- VS Code: editor, search, terminal, autocomplete and shortcuts.
- Keyboard layouts, AltGr/dead keys, IME selection and clipboard safety.
- Setup/repair/update/uninstall, TSF registration and rollback.
- Accessibility, resource usage, code signing and application stability.

Never log typed text. No marketing popups, account requirements or
unexpected auto-registration of experimental components.

## Current release gate

**Native TSF registration + COM smoke passing is not equivalent to live
system-wide Vietnamese input.** Production compatibility QA is pending.

See also `docs/WINDOWS-COMPAT-0.13.md` and `docs/STATUS.md`.

## Optional TSF test on an isolated Windows VM

The research-only GitHub artifact includes `LakakeyTsfV2.dll`,
`LakakeyTsfLabCtl.exe` and native QA tools. Nothing in the ordinary desktop
installer registers this DLL. For testing only, first create a disposable
Windows VM snapshot and extract the research files into a fixed folder.

From a PowerShell window in that folder:

```powershell
.\\LakakeyTsfLabCtl.exe --status
.\\LakakeyTsfLabCtl.exe --register .\\LakakeyTsfV2.dll --dev-only
```

In Windows input settings, check whether the Lakakey Vietnamese experimental
profile appears and, if available, select it for isolated testing in the
native QA host and Notepad. Do not enter real passwords or sensitive data.
Registration does not guarantee activation or successful text input.

After testing, select another input source and run:

```powershell
.\\LakakeyTsfLabCtl.exe --unregister --dev-only
.\\LakakeyTsfLabCtl.exe --status
```

Remove the test files only after registration has been cleaned up.
The command-line tool performs changes only after explicit `--dev-only`
commands and gives text feedback without popup dialogs. No typed content is
logged, transmitted or persisted.
