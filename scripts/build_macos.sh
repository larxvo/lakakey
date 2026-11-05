#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cmake -S "$ROOT" -B "$ROOT/build-macos" -DCMAKE_BUILD_TYPE=Release
cmake --build "$ROOT/build-macos" --parallel
ctest --test-dir "$ROOT/build-macos" --output-on-failure
echo 'This builds the portable core and lab, NOT a macOS InputMethodKit bundle.'
