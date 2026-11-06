# Lakakey 0.14 — Native Windows Typing Verification

Maintainer: Larx Vo · https://vovinhlac.com · © 2026

## Delivered

- TSF registration now explicitly enables its Vietnamese input profile.
- The experimental service defaults to **disabled** until the desktop controller publishes `Enabled=1` in the current user's local input settings. Only mode and scheme are stored; typed text is never persisted.
- Windows native TSF key routing commits **space and printable ASCII delimiters** inside the same native edit session as the current preedit. A rejected edit session passes the original key back to the host.
- The previous active layout/dead-key-safe translation remains in place. Outside an active composition, punctuation is not intercepted by Lakakey.
- An opt-in executable tests the real `ITfThreadMgr` activation and key-sink lifetime against Windows TSF.
- A separate **experimental interactive harness** registers the profile temporarily in a disposable CI runner, activates it, sends fixed synthetic Telex keys to its own Win32 EDIT field, validates the expected text, then removes the profile and test settings.
- Boundary regression tests cover space, punctuation, Telex and VNI with repeated sequences.
- The old Windows preview build workflow is removed, and the quiet native diagnostics UI and installer are versioned 0.14.

## CI verification types

| Level | Check | Release implication |
|---|---|---|
| A | C++/Go tests | verifies portable engine and desktop behavior |
| B | Windows TSF DLL compilation | verifies binary interface linkage |
| C | COM thread-manager activation | validates real TSF interfaces and lifecycle, not text I/O |
| D | Profile registration/unregistration in disposable runner | verifies basic Windows registration lifecycle |
| E | Synthetic typing into Win32 EDIT | tests real Windows text I/O, if runner allows interactive focus |
| F | Notepad, browser, Office, VS Code, security fields | **requires device/application QA** |

Levels A–D do not establish system-wide typing. The level-E live-input probe is **research only, non-blocking** in CI. A failing or inaccessible interactive session must be reported as **NOT VERIFIED**, not as success.

## Reproduce locally (isolated VM, developer only)

```powershell
cmake -S . -B build-win -A x64 -DLAKAKEY_BUILD_TESTS=ON -DLAKAKEY_BUILD_TSF_V2=ON
cmake --build build-win --config Release --target LakakeyTsfV2 LakakeyTsfActivationSmoke LakakeyTsfLiveTyping
ctest --test-dir build-win -C Release --output-on-failure
```

The interactive typing harness requires an explicit `--ephemeral-runner` flag and refuses to alter existing Lakakey TSF or input-preference registry keys. It is not designed to be run on a daily-use computer.

## Release rules

- `LakakeyDesktopSetup.exe` contains the desktop Settings/tray application only; **it does not register the experimental TSF service**.
- Research TSF DLLs, activation tests, registration tests and the live typing harness are shipped only as separate CI development artifacts.
- No auto-release, telemetry, account requirement, browser access or repeated notification dialogs.
- A functional native input release needs repeated Windows 10/11 end-to-end tests on real applications, secure field verification, upgrade/rollback and signing.

© 2026 Larx Vo — https://vovinhlac.com
