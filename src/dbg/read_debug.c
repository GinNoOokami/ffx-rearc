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

/*?*/ void func_00157788(/*?*/ u8*, /*?*/ u8*, /*?*/ s32); /* extern */

s32 func_002549A8(s32 arg0, s32 arg1) {
    s32 i = 0;

    if (D_006A2C90[0] != 0) {
        for (i = 0; i < 16; i++) {
            if (D_006A2C90[i] == 0) {
                break;
            }
        }
    }

    if (i == 16) {
        func_00157788("i!=FILE_NAME_CHG_TBL_CNT", "read_debug.c", 0x50);
    }

    D_006A2C90[i] = arg0;
    D_006A2CD0[i] = arg1;
    return i;
}

void func_00254A88(s32 arg0) {
    D_006A2C90[arg0] = 0;
    D_006A2CD0[arg0] = 0;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254AB0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254AD0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254DA8);

/*?*/ s32 func_00254AD0(s32);                               /* extern */
extern s16* D_0078DFC4;

s32 func_00254DD8(s32 arg0, s32 arg1) {
    return func_00254AD0(D_0078DFC4[arg0] + arg1);
}

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
