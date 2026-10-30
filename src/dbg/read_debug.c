#include "common.h"

extern s8 D_00326E1C;
extern s32 D_006A2C90[];
extern s32 D_006A2CD0[];

void func_00254958(void) {
    s32 i;

    if (D_00326E1C == 0) {
        D_00326E1C = 1;
        for (i = 0; i < 16; i++) {
            D_006A2C90[i] = 0;
            D_006A2CD0[i] = 0;
        }
    }
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_002549A8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254A88);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254AB0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254AD0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254DA8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254DD8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254E08);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254EF0);

INCLUDE_RODATA("asm/jp_intl/nonmatchings/dbg/read_debug", D_00573400);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254F18);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254F40);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255020);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255158);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_002556A0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_002556F8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255800);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255840);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255880);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_002558A0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_002558A8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_002558B0);
