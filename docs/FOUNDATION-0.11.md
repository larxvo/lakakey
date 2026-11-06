# Lakakey 0.11 — Native Foundation engineering notes

**Owner:** Larx Vo · https://vovinhlac.com · © 2026

## Delivered changes

- Settings schema v2, migration from legacy JSON, rejecting unsupported future schema to avoid destructive downgrade.
- Same-directory temporary writes, fsync and Windows MoveFileExW replacement (REPLACE_EXISTING | WRITE_THROUGH).
- Recovery from the previous valid settings backup. No typed text is stored.
- Installer stages all files and atomically swaps the desktop executable only after preparation succeeds. Failure leaves the installed executable in place.
- Tray single/double-click separation, explicit launch behavior, startup preference wiring and inline information rather than repeated modal dialogs.
- Core input contract: disabled mode passes through input; secure fields do not consume keys. Invalid UTF-8 and surrogate code points are rejected.
- Test suite for config recovery, migration, downgrade protection, Unicode, focus, secure field and 10,000-word deterministic stress run.
- CI split between Linux core tests, Go race checks, Windows MSVC core + TSF compilation, and installer packaging. No automatic public release.

## Architecture

```
Native UI / Tray / Settings
            |
Windows TSF host (experimental)
            |
Portable lakakey::InputSession / Engine
            |
Unicode validation / Telex / VNI rules
```

The present desktop app and TSF prototype are *not yet integrated* into a production input method. Their states must not be described as equivalent. The installer does not register the experimental text service.

## Release acceptance criteria

1. TSF COM interface, registration and cleanup tested in an isolated Windows VM.
2. End-to-end typing in native editor, browsers, IDEs and office applications; no dropped characters or corrupted selection.
3. Secure/password fields bypass composition, raw key events are not persisted.
4. Focus switches, caret navigation, IME termination and app shutdown do not leak composition state.
5. At least 5,000 ground-truth Vietnamese examples plus Unicode/property/fuzz cases.
6. Per-user install, update, repair, failure recovery and uninstall verified on Windows 10/11.
7. Sign Windows distribution and publish per-release hashes; manual release gate.
8. Real-device metrics for latency, memory and crash rate; no unverified benchmarks.

## Verification commands

```sh
go test -race -count=1 ./desktop/windows
cmake -S . -B build -DLAKAKEY_BUILD_TESTS=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

Windows experiment:

```powershell
cmake -S . -B build-win -A x64 -DLAKAKEY_BUILD_TSF_V2=ON
cmake --build build-win --config Release --target LakakeyTsfV2
```

**Never register a research DLL on a regular production workstation.**

## Next development milestone

Native input integration: connect tray state to input processor, establish composition ownership, add complete context tests and collect live host QA evidence. Then advance to app profiles, shortcuts, text expansion and animation as independently tested modules.
