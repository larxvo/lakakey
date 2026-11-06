# Lakakey

**A lightweight, local-first Vietnamese input platform for Windows, macOS and Linux.**

Created by **Larx Vo** · [vovinhlac.com](https://vovinhlac.com) · Copyright © 2026.

## Development status — 0.14 Native Windows Typing Verification

Lakakey is under active development. Its portable C++ engine includes baseline Telex/VNI support, Unicode handling and unit tests. A native Windows desktop app provides Settings, system tray, hotkeys and local preferences. A Windows Text Services Framework (TSF) implementation is being tested separately.

**The current Windows desktop installer does not activate system-wide Vietnamese input.** The native TSF DLL is experimental and is not installed by the public desktop setup. Compatibility with Notepad, Office, browsers and development editors remains a release gate, not a completed claim.

## What's in 0.14

- Shared, deterministic TSF key-routing contract for probe and edit paths; punctuation/navigation remain available to the host.
- Fail-open handling for rejected synchronous edit sessions.
- C++ policy tests for mode, modifiers, focus, backspace and context state.
- Faster Windows tray → input-preference bridge, using native registry calls without spawning `reg.exe` on each change.
- Optional native test host with text, multiline and password fields.
- Windows keyboard-layout-aware character translation (rather than assuming one physical layout).
- TSF COM smoke validation of class factory and interface release, without installing the text service.
- Real Windows TSF thread manager activation smoke and opt-in synthetic typing into a native EDIT field on a disposable Windows runner.
- Atomic space/ASCII punctuation commits in active TSF composition, plus repeated boundary regression tests.
- Explicit profile enablement and safe **disabled** default when an input preference has not been published.
- Separate source, desktop setup and research-only artifacts. See [0.14 evidence levels](docs/LIVE-TYPING-0.14.md).
- Best-effort composition finalization on focus loss and an inline native-registration diagnostics screen.
- Windows x64 executable builds, separate research-only TSF build artifacts and CI configuration.
- Quiet, locally stored settings; no login, marketing popups or typed-text telemetry.

## Download and run desktop development builds

Use [GitHub Actions](https://github.com/larxvo/lakakey/actions/workflows/windows-desktop-beta.yml) after a **successful** run on the relevant branch; download the `Lakakey-0.14-Windows-x64-Dev` artifact. Artifacts may require an authenticated GitHub session. This is an **unsigned development build**: verify its origin and SHA-256 and do not bypass security warnings without assessing the risk.

When available, run `LakakeyDesktopSetup.exe`. It installs the desktop settings/tray application for the current user and provides an uninstall entry. The experimental TSF DLL remains separate.

## Source organization

| Path | Purpose |
| --- | --- |
| `include/lakakey_engine.h`, `src/engine.cpp` | Portable Unicode/Telex/VNI core |
| `include/lakakey_session.h` | Host-neutral input session |
| `include/lakakey_tsf_policy.h` | Pure key-routing contract |
| `platform/windows/tsf/` | Experimental TSF adapter and QA host |
| `desktop/windows/` | Native Windows Settings, tray, local preferences |
| `desktop/installer/` | Per-user Windows desktop installer |
| `tests/` | Core, session, Unicode and TSF policy regression tests |
| `docs/LIVE-TYPING-0.14.md` | TSF activation, synthetic typing evidence and 0.14 release gates |
| `docs/WINDOWS-COMPAT-0.13.md` | Previous Windows interoperability matrix |
| `docs/NATIVE-INPUT-0.12.md` | Previous milestone QA criteria |

## Building and testing

Portable C++ core (Linux, or equivalent development environment):

```bash
cmake -S . -B build -DLAKAKEY_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Windows desktop (Go 1.23+):

```powershell
go test ./desktop/windows
go build -o desktop/installer/payload/LakakeyDesktop.exe ./desktop/windows
go build -o LakakeyDesktopSetup.exe ./desktop/installer
```

Experimental Windows native adapter requires Windows SDK and Visual Studio/MSVC:

```powershell
cmake -S . -B build-win -A x64 -DLAKAKEY_BUILD_TSF_V2=ON -DLAKAKEY_BUILD_TESTS=ON
cmake --build build-win --config Release --target LakakeyTsfV2 LakakeyTsfSmokeHost LakakeyTsfComSmoke LakakeyTsfActivationSmoke LakakeyTsfLiveTyping LakakeyKeyboardLayoutSmoke lakakey_tsf_policy_tests
ctest --test-dir build-win -C Release --output-on-failure
```

Do not register the experimental adapter on a daily-use computer. Native integration and secure-field handling must be validated in an isolated test environment.

## Product principles

Local-first. No account, advertising or cloud logging of typed content. Native performance, clear user control and minimal interruptions. Unsupported options must not masquerade as functional features.

## Licensing and contributions

The repository is public for development visibility. No general redistribution license has been granted at this time. Use GitHub Issues for bug reports; do not attach sensitive typing samples or personal content.

Maintainer: **Larx Vo** — [vovinhlac.com](https://vovinhlac.com).
