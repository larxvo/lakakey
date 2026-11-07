#!/usr/bin/env python3
"""Build the official Lakakey Windows icon from the existing brand reference.

The logo is cropped from ui/assets/brand_3d_reference.png, which contains
Lakakey's blue rounded-square L and paw brand mark. This script does not
create an unrelated replacement logo.

Usage: python tools/build_icon.py
Requires: Pillow (see GitHub Actions).
"""
from __future__ import annotations
from pathlib import Path
import struct
from PIL import Image, ImageDraw, ImageFilter

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / "ui/assets/brand_3d_reference.png"
DESTINATION = ROOT / "assets/windows"
SIZES = ((16, 16), (24, 24), (32, 32), (48, 48),
         (64, 64), (128, 128), (256, 256))


def main() -> None:
    DESTINATION.mkdir(parents=True, exist_ok=True)
    if not SOURCE.is_file():
        raise FileNotFoundError(f"Official Lakakey brand reference missing: {SOURCE}")
    with Image.open(SOURCE) as artwork:
        artwork = artwork.convert("RGBA")
        if artwork.size != (1672, 941):
            raise ValueError("Brand sheet dimensions changed; review logo crop before publishing")
        # Original top-left Lakakey logo, excluding the adjacent wordmark.
        square = artwork.crop((49, 34, 140, 125))
    square = square.resize((960, 960), Image.Resampling.LANCZOS)
    mask = Image.new("L", square.size, 0)
    ImageDraw.Draw(mask).rounded_rectangle((11, 11, 948, 948), radius=210, fill=255)
    mask = mask.filter(ImageFilter.GaussianBlur(1.5))
    square.putalpha(mask)
    icon = Image.new("RGBA", (1024, 1024), (0, 0, 0, 0))
    icon.alpha_composite(square, (32, 32))

    icon.save(DESTINATION / "lakakey.png")
    ico = DESTINATION / "lakakey.ico"
    icon.save(ico, format="ICO", sizes=SIZES)
    data = ico.read_bytes()
    if len(data) < 512 or struct.unpack_from("<HHH", data, 0) != (0, 1, len(SIZES)):
        raise ValueError("Generated .ico is incomplete; abort branded installer build")
    with Image.open(ico) as verify:
        frame_sizes = verify.info.get("sizes", set())
    if not set(SIZES).issubset(frame_sizes):
        raise ValueError(f"Windows icon missing sizes: {set(SIZES) - set(frame_sizes)}")
    print(f"Lakakey brand icon: {ico} ({len(data)} bytes, {len(SIZES)} sizes)")


if __name__ == "__main__":
    main()
