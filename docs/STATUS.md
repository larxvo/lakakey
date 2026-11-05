# Lakakey — Implementation status and honest compatibility matrix

Updated 2026-11-06.

| Feature | Status | Verified? |
|---|---|---|
| Windows EXE preview build | Go source can cross-compile to PE32+ x64 | Linux cross-compilation only; NOT tested on Windows hardware |
| One-click Windows preview installer | Per-user installer source present | NOT tested on Windows hardware |
| Settings VN/EN selector | Changes stored UI state only | Does not alter Windows language services |
| Accent colors | Selector stored | Preview only |
| Mascot Full/Minimal/Off | Selector stored | Preview only; no always-on native avatar in Go app |
| Telex/VNI core | C++17 engine and regression tests | Linux tests passed in development; CI reruns tests |
| Windows TSF IME | Experiment / probe source | **NOT operational; never register experimental DLL for daily use** |
| Chrome/Word/Edge/VS Code system-wide input | Not implemented | Not supported |
| macOS and Linux IME | Planned adapter architecture | Not built |
| Code-signing and notarization | Not configured | Unsigned preview |

### Product priorities

1. Finish safe Windows TSF key sinks and composition session management, and test focus, password fields, selection replacement, Unicode and accessibility.
2. Test real Windows 10/11 integrations on native apps and browsers, then publish signed setup.
3. Deliver independent rigged 3D mascot animation runtime, keeping Full/Minimal/Off with minimal performance impact.
4. Implement and test macOS InputMethodKit and Linux Fcitx5 / IBus adapters.

Source repository is public; this does not imply an open-source license. Copyright © 2026 Larx Vo.
