# splat segment type "vutext": VU microcode kept as an opaque blob (splat can't disassemble VU
# code), emitted into its own .vutext section so the linker places it where the original has it.
from splat.segtypes.common.textbin import CommonSegTextbin


class PS2SegVutext(CommonSegTextbin):
    def get_linker_section(self) -> str:
        return ".vutext"

    def get_section_flags(self) -> str:
        return "ax"  # allocated, executable
