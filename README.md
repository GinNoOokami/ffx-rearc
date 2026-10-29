# Final Fantasy X Decompilation

## Summary

A project dedicated to reconstructing a high-quality resemblance of the original source code of FFX (PS2 version).

No assets from the original game are stored in this repository.

## Getting started

### Dependencies

* Python
* make
* mips-binutils
* 7z (optional, for ISO extraction)
* curl (to download the original compiler; see Building)

Install Python (tested with 3.14), install the requirements, and start the venv:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -U -r requirements.txt
```

### Setup

You need a working copy of Final Fantasy X International (JP). There are two ways to do this:

1. *Direct:* Copy SLPS_250.88 from the disc to `orig/jp_intl/SLPS_250.88`.
1. *ISO:* Name it `ffx-jp_intl.iso` and put it in the path `orig/jp_intl/ffx-jp_intl.iso` then run the following command in your terminal:

```bash
make extract
```

## Building

```bash
make split    # disassemble the game with splat
make          # rebuild and check it matches (prints "…SLPS_250.88.bin: OK")
```

After changing the splat config or `symbol_addrs.txt`, run both again.
