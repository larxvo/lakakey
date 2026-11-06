# Lakakey — Vietnamese Input Platform

**Created by Larx Vo** · [vovinhlac.com](https://vovinhlac.com) · © 2026 Larx Vo

Lakakey is a free, local-first Vietnamese input platform under active development. The Windows application uses native Win32 controls and a system-tray companion; a portable C++ input engine and a Windows Text Services Framework (TSF) prototype are maintained separately.

## Windows 0.11 Native Foundation

- Compact/expanded native settings and VN/EN *preference* state in the tray.
- Per-user Windows desktop installer with Start Menu entry and uninstall support.
- Quiet startup and inline help/about instead of repetitive popups.
- Safe staged file replacement during upgrades; no deleting the installed executable before a replacement succeeds.
- Settings schema v2: atomic writes, last-valid backup recovery, migration and downgrade protection.
- Separate C++ core with Telex/VNI baseline, Unicode validation, secure-field and disabled-mode bypass contracts.
- CI builds Windows desktop and an experimental TSF DLL separately; native engine regression tests run on Linux/Windows.

**Input integration status:** The current downloadable Windows desktop installer does **not** register the experimental TSF DLL. It is a desktop development build, not a finished system-wide input processor. The TSF implementation needs integration and native application QA before it can ship. No keyboard hooks or input interception are installed by this package.

## Repository

- `src/engine.cpp` and `include/` — core rules, Unicode and input-session contract
- `platform/windows/tsf/lakakey_tsf_v2.cpp` — experimental Windows TSF text-service implementation
- `desktop/windows/` — native Windows UI, tray and persisted settings
- `desktop/installer/` — per-user Windows desktop setup
- `tests/` — deterministic regression tests
- `.github/workflows/windows-desktop-beta.yml` — automated core, TSF compile and installer checks
- `docs/FOUNDATION-0.11.md` — engineering report, limitations and release gates
- `docs/STATUS.md` — verified vs. incomplete features

## Download a development build

Open [GitHub Actions — Lakakey Native CI](https://github.com/larxvo/lakakey/actions/workflows/windows-desktop-beta.yml), select a successful run and download the **Lakakey-0.11-Windows-x64-Dev** artifact. Source changes live in [PR #1](https://github.com/larxvo/lakakey/pull/1) until reviewed and merged.

The unsigned development installer has not been verified on all Windows 10/11 configurations. Verify provenance and SHA-256; do not ignore operating-system security warnings without assessment.

## Build and verify

Linux:
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DLAKAKEY_BUILD_TESTS=ON
cmake --build build -j2
ctest --test-dir build --output-on-failure
go test -race -count=1 ./desktop/windows
```

Windows (Go 1.23+, MSVC/CMake for TSF):
```powershell
go test ./desktop/windows
go build -trimpath -ldflags '-s -w -H=windowsgui' -o desktop/installer/payload/LakakeyDesktop.exe ./desktop/windows
go build -trimpath -ldflags '-s -w -H=windowsgui' -o LakakeyDesktopSetup.exe ./desktop/installer
cmake -S . -B build-win -A x64 -DLAKAKEY_BUILD_TSF_V2=ON
cmake --build build-win --config Release --target LakakeyTsfV2
```

**Do not register the TSF research DLL on a daily-use machine.** It is not included in the installer.

## Privacy, licensing and attribution

Core operation is intended to be offline. No accounts, ads or telemetry are required. The desktop application does not log key contents. Input interception is not part of this installer.

This public repository does not yet have an open-source license; public visibility is not an automatic grant of reuse or redistribution rights. Build artifacts are supplied for evaluation only.

© 2026 **Larx Vo** · [vovinhlac.com](https://vovinhlac.com)
