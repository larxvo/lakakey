# Lakakey — Implementation status

Updated 2026-11-06. This file distinguishes source code, reproducible tests, and real-device verification.

| Area | Source status | Verification |
| --- | --- | --- |
| Windows Win32 Settings + tray | Implemented desktop shell | Go Windows x64 cross-compiled; native UI testing pending |
| Per-user EXE installer | Builds and performs staged file replacement | Windows QA/rollback still required |
| Quiet launch + inline Help/About | Implemented | Source review; real Windows visual QA pending |
| Settings schema v2 / backups | Implemented | 7 Go tests passed on Linux |
| C++17 Vietnamese mapping core | Telex/VNI baseline | Existing mapping tests; expanded native CI added |
| Disabled/secure field contract | Defensive bypass added | New C++ contract tests pending CI |
| Unicode scalar validation | Surrogate rejection added | New C++ contract tests pending CI |
| Windows TSF text-service prototype | Native composition implementation in source | MSVC compile check added; not verified as working input |
| Word/browser/IDE integration | Not delivered | Requires real Windows application QA |
| Mascot animation runtime | Separate research assets | Not integrated with current desktop build |
| macOS InputMethodKit | Planned | Not built |
| Linux Fcitx5/IBus | Planned | Not built |
| Signing and public stable release | Infrastructure work ongoing | Not signed, not production ready |

## 0.11 engineering priorities

1. Build native TSF v2 with Windows MSVC, resolve compilation and interface issues.
2. Add deterministic input harness: focus, caret, secure fields, key passthrough and composition termination.
3. Integrate actual mode selection with engine state and establish native end-to-end QA.
4. Test installer update/recovery/uninstall on actual Windows 10/11 x64.
5. Verify code signing, secure artifact integrity and rollback before stable release.

See [FOUNDATION-0.11.md](FOUNDATION-0.11.md).

© 2026 Larx Vo · https://vovinhlac.com
