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

Other navigation keys and shortcuts remain the host's responsibility;
composing through Enter, punctuation and caret movement still requires
manual and framework-specific testing.

## Verification / regression coverage

- `tests/test_tsf_policy.cpp`: Space is consumed only with active, nonempty
  composition and an enabled, nonsecure, matching input context.
- `tests/test_typing_pipeline.cpp`: Host-neutral Telex/VNI transaction
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
