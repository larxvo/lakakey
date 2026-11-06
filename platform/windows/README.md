# Lakakey Windows Settings v0.3

Native Win32 settings companion: VN/EN **preview** toggle, light/dark appearance, six accent choices, tray menu, persistent preferences at `%APPDATA%\Lakakey\settings.ini`, About/author credit and clickable author website.

## Build on Windows

Install Visual Studio 2022 with Desktop development with C++ and CMake. From a Developer PowerShell at the repository root:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
.\build\Release\LakakeySettings.exe
```

**Not a system input method:** This app does not register TSF, intercept keyboard events, or change typing in other programs. The VN/EN control is a settings-state preview only. A Windows TSF text service is required for actual typing. Do not publish as a verified system input method until compatibility testing passes.

Author: Larx Vo · https://vovinhlac.com. Free to use; author website optional.
