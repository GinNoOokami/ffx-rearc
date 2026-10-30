#!/usr/bin/env python3
"""Decompile one function with m2c and put the result in its C file.

Usage: tools/decompile.py func_002548A0 [--print] [m2c options...]
       (or just the address: 2548A0)

The function must still be an INCLUDE_ASM in a C file. m2c gets that file as context
(run through the C preprocessor with M2CTX defined, which drops the INCLUDE_ASM lines),
so it knows the file's declarations and types. Its output replaces the INCLUDE_ASM line.
Types m2c couldn't work out (written "?") become guesses marked /*?*/, so the file
compiles right away. With --print the output goes to stdout instead and the file is left
alone. Other options go to m2c unchanged (e.g. --stack-structs).
"""

import argparse
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SYS_INCLUDE = ROOT / "tools" / "cc" / "include"


def function_name(arg: str) -> str:
    """func_002548A0, or an address like 2548A0 / 0x2548a0."""
    if arg.startswith("func_"):
        return arg
    return "func_" + arg.upper().removeprefix("0X").zfill(8)


def find_asm(nonmatchings: Path, name: str) -> Path:
    hits = list(nonmatchings.rglob(f"{name}.s"))
    if not hits:
        where = nonmatchings.relative_to(ROOT)
        sys.exit(f"{name}: no asm in {where} (not an INCLUDE_ASM, or not split?)")
    return hits[0]


def make_context(c_file: Path, out: Path) -> None:
    """The C file preprocessed with the build's include paths (newlib, not the host)."""
    out.parent.mkdir(parents=True, exist_ok=True)
    with open(out, "w") as f:
        subprocess.run(
            [
                "cpp", "-P", "-undef", "-nostdinc", "-DM2CTX",
                "-I", str(ROOT / "include"),
                "-isystem", str(SYS_INCLUDE / "gcc"),
                "-isystem", str(SYS_INCLUDE / "newlib"),
                str(c_file),
            ],
            stdout=f,
            check=True,
        )  # fmt: skip


def run_m2c(asm: Path, name: str, context: Path, m2c_args: list[str]) -> str:
    # Read-only data shared by several functions isn't moved into any one function's
    # .s; splat puts it in its own D_*.s next to them (INCLUDE_RODATA). Passing those
    # too lets m2c show its contents.
    rodata = sorted(
        str(p) for p in asm.parent.glob("*.s") if not p.name.startswith("func_")
    )
    result = subprocess.run(
        [
            sys.executable, "-m", "m2c.main",
            "-t", "mipsee-gcc-c",
            "--context", str(context),
            "--function", name,
            *m2c_args,
            str(asm),
            *rodata,
        ],
        capture_output=True,
        text=True,
    )  # fmt: skip
    if result.returncode != 0:
        sys.exit(result.stderr or result.stdout)
    return result.stdout


# Where m2c writes "?" as a type: at the start of a line (declarations, locals), after
# "(" or "," (parameters, casts), or after extern/static; followed by a space, "*", ")"
# or ",". A "?" anywhere else is the ternary operator.
UNKNOWN_TYPE = re.compile(
    r"(^\s*|[(,]\s*|\b(?:extern|static)\s+)\?(?=[\s*),])", re.MULTILINE
)


def guess_unknown_types(code: str) -> str:
    """Replace m2c's "?" types with compilable guesses, each marked /*?*/."""
    # A function only used by its address (extern ? func_X;): declare a function, not a
    # variable.
    code = re.sub(
        r"^extern \? (func_\w+);", r"/*?*/ void \1();", code, flags=re.MULTILINE
    )
    # Shared rodata m2c can't give an initializer: the data is still in an
    # INCLUDE_RODATA, so refer to that label instead of defining a second copy.
    code = re.sub(
        r"^static \? (D_\w+);", r"extern /*?*/ s32 \1;", code, flags=re.MULTILINE
    )

    def guess(m: re.Match) -> str:
        rest = code[m.end() :]
        if m.group(1).strip() == "" and re.match(r" \w+\(", rest):
            return m.group(1) + "/*?*/ void"  # unknown return type
        if rest.lstrip().startswith("*"):
            return m.group(1) + "/*?*/ u8"  # unknown pointee
        return m.group(1) + "/*?*/ s32"

    return UNKNOWN_TYPE.sub(guess, code)


def replace_include_asm(c_file: Path, include_asm: str, code: str) -> None:
    """Swap the INCLUDE_ASM line for the code. Written to a temp file first, so a
    failure can't leave the C file half-written."""
    lines = c_file.read_text().splitlines(keepends=True)
    matches = [i for i, line in enumerate(lines) if line.strip() == include_asm]
    if not matches:
        sys.exit(f"{c_file.relative_to(ROOT)}: no line {include_asm}")
    lines[matches[0]] = code.rstrip("\n") + "\n"
    with tempfile.NamedTemporaryFile("w", dir=c_file.parent, delete=False) as tmp:
        tmp.writelines(lines)
    os.replace(tmp.name, c_file)


def main() -> None:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("function", help="function name or address")
    parser.add_argument("--version", default=os.environ.get("VERSION", "jp_intl"))
    parser.add_argument(
        "--print", action="store_true", help="print the result, leave the C file alone"
    )
    args, m2c_args = parser.parse_known_args()

    name = function_name(args.function)
    nonmatchings = ROOT / "asm" / args.version / "nonmatchings"
    asm = find_asm(nonmatchings, name)
    unit = asm.parent.relative_to(nonmatchings)  # e.g. archive/decode
    c_file = ROOT / "src" / unit.with_suffix(".c")
    context = ROOT / "build" / args.version / "ctx" / unit.with_suffix(".ctx.c")

    make_context(c_file, context)
    code = guess_unknown_types(run_m2c(asm, name, context, m2c_args))

    if args.print:
        print(code, end="")
        return
    include_asm = f'INCLUDE_ASM("asm/{args.version}/nonmatchings/{unit}", {name});'
    replace_include_asm(c_file, include_asm, code)
    print(f"{name} decompiled into {c_file.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
