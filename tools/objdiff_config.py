#!/usr/bin/env python3
"""Write objdiff.json: one unit per C file in the splat config.

usage: objdiff_config.py <splat yaml> <build dir> <output>
Base objects are <build dir>/src/<name>.c.o, targets the same path under expected/ (make expected).
"""

import json
import sys

import yaml


def c_files(config):
    for segment in config["segments"]:
        if not isinstance(segment, dict):
            continue
        for sub in segment.get("subsegments", []):
            kind, name = (
                (sub[1], sub[2])
                if isinstance(sub, list)
                else (sub["type"], sub["name"])
            )
            if kind == "c":
                # splat accepts bare numbers as names (e.g. 11D230 vs 254958), yaml reads some as ints
                yield str(name)


def main():
    yaml_path, build_dir, out_path = sys.argv[1:]
    with open(yaml_path) as f:
        config = yaml.safe_load(f)
    units = []
    for name in c_files(config):
        obj = f"{build_dir}/src/{name}.c.o"
        units.append({"name": name, "target_path": f"expected/{obj}", "base_path": obj})
    objdiff = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "custom_make": "make",
        "build_base": True,
        "build_target": False,
        "watch_patterns": ["*.c", "*.h", "*.s", "*.inc"],
        "units": units,
    }
    with open(out_path, "w") as f:
        json.dump(objdiff, f, indent=2)
        f.write("\n")


if __name__ == "__main__":
    main()
