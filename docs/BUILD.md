# Build Lakakey — developer guide

## Windows Settings Preview

Prerequisites: Go 1.23+, Windows 10/11; no Electron. Use PowerShell in repository root:

```powershell
go build -trimpath -ldflags '-s -w -H=windowsgui' -o winpreview/installer/payload/LakakeyPreview.exe winpreview/app/main.go
go build -trimpath -ldflags '-s -w -H=windowsgui' -o LakakeyPreviewSetup.exe winpreview/installer/main.go
```

The installer embeds three existing files: `winpreview/installer/payload/LakakeyPreview.exe`, `mascot.bmp` and `README_PREVIEW.txt`. `go:embed` fails if they are missing. GitHub Actions builds these in the correct order.

## C++17 engine tests (Linux/macOS)

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Windows C++ engine

Visual Studio 2022 and Windows SDK:

```powershell
cmake -S . -B build-win -G 'Visual Studio 17 2022' -A x64 -DLAKAKEY_BUILD_EXPERIMENTAL_TSF=OFF
cmake --build build-win --config Release
ctest --test-dir build-win -C Release --output-on-failure
```

The experimental TSF COM DLL is disabled by default; **do not register it on your daily-use machine**. The Go preview is not connected to the C++ engine and cannot act as system input method.
