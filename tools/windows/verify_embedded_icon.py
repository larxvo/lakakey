#!/usr/bin/env python3
"""Fail Windows CI if either Lakakey executable lacks a real embedded icon.

A file merely named .exe or a shortcut referencing an external .ico is not enough.
Uses pefile only as a build-time verification dependency.
"""
from __future__ import annotations

import argparse
from pathlib import Path
import pefile

ICON = 3
GROUP_ICON = 14

def verify(path: Path) -> None:
    if not path.is_file():
        raise FileNotFoundError(path)
    pe = pefile.PE(str(path), fast_load=False)
    pe.parse_data_directories(
        directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_RESOURCE"]]
    )
    groups = {item.id for item in pe.DIRECTORY_ENTRY_RESOURCE.entries}
    if ICON not in groups or GROUP_ICON not in groups:
        raise AssertionError(f"{path.name}: missing RT_ICON or RT_GROUP_ICON resource")
    # Inno Setup uses a 32-bit bootstrap executable by default, even when
    # its installed application and payload are Windows x64. Both can carry
    # the official icon resource. Only the desktop program must be PE x64.
    machine = pe.FILE_HEADER.Machine
    if machine not in (0x14C, 0x8664):
        raise AssertionError(f"{path.name}: invalid Windows PE architecture {machine:#x}")
    if path.name.lower() == "lakakeydesktop.exe" and machine != 0x8664:
        raise AssertionError(f"{path.name}: desktop application is not x64")
    architecture = "x64" if machine == 0x8664 else "x86 setup bootstrapper"
    print(f"PASS: {path.name} — {architecture}, embedded Lakakey icon")

def main() -> None:
    cli = argparse.ArgumentParser()
    cli.add_argument("executables", nargs="+", type=Path)
    args = cli.parse_args()
    for file in args.executables:
        verify(file)

if __name__ == "__main__":
    main()
