#!/usr/bin/env python3
"""Decompile one function with m2c, using its C file as context.

Usage: tools/decompile.py func_002548A0 [m2c options...]   (or just the address: 2548A0)

The function must still be an INCLUDE_ASM in a C file, so its asm is in
asm/<version>/nonmatchings/<file>/<name>.s. That C file, run through the C preprocessor with
M2CTX defined (which drops the INCLUDE_ASM lines), is the context: m2c then knows the file's
declarations and types. Any further options go to m2c unchanged (e.g. --stack-structs).
"""

import argparse
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def find_asm(version: str, name: str) -> Path:
    hits = list((ROOT / "asm" / version / "nonmatchings").rglob(f"{name}.s"))
    if not hits:
        sys.exit(
            f"{name}: no asm in asm/{version}/nonmatchings (not an INCLUDE_ASM, or not split?)"
        )
    return hits[0]


def main() -> None:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("function", help="function name or address")
    parser.add_argument("--version", default=os.environ.get("VERSION", "jp_intl"))
    args, m2c_args = parser.parse_known_args()

    name = args.function
    if not name.startswith("func_"):
        name = "func_" + name.upper().removeprefix("0X").zfill(8)

    asm = find_asm(args.version, name)
    nonmatchings = ROOT / "asm" / args.version / "nonmatchings"
    c_file = ROOT / "src" / asm.parent.relative_to(nonmatchings).with_suffix(".c")

    ctx = (
        ROOT
        / "build"
        / args.version
        / "ctx"
        / asm.parent.relative_to(nonmatchings).with_suffix(".ctx.c")
    )
    ctx.parent.mkdir(parents=True, exist_ok=True)
    with open(ctx, "w") as out:
        subprocess.run(
            [
                "cpp",
                "-P",
                "-undef",
                "-DM2CTX",
                "-I",
                str(ROOT / "include"),
                str(c_file),
            ],
            stdout=out,
            check=True,
        )

    # Read-only data shared by several functions isn't moved into any one function's .s; splat puts
    # it in its own D_*.s next to them (INCLUDE_RODATA). Passing those too lets m2c show its contents.
    rodata = sorted(
        str(p) for p in asm.parent.glob("*.s") if not p.name.startswith("func_")
    )

    sys.exit(
        subprocess.run(
            [
                sys.executable,
                "-m",
                "m2c.main",
                "-t",
                "mipsee-gcc-c",
                "--context",
                str(ctx),
                "--function",
                name,
                *m2c_args,
                str(asm),
                *rodata,
            ]
        ).returncode
    )


if __name__ == "__main__":
    main()
