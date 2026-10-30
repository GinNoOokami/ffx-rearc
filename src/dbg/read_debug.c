#include "common.h"

typedef struct {
    /* 0x00 */ u8 pad0[0x20];
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
} ArchiveState;

ArchiveState* func_00162DD0(void);

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

s32 func_00254DA8(s32 arg0) {
    return func_00254AD0(func_00162DD0()->unk20 + arg0);
}

/*?*/ s32 func_00254AD0(s32); /* extern */
extern s16* D_0078DFC4;

s32 func_00254DD8(s32 arg0, s32 arg1) { return func_00254AD0(D_0078DFC4[arg0] + arg1); }

ArchiveState* func_00162DD0(void);
s32 func_00162EB8(s32);
s32 func_00165D70(s32, s32);
s32 func_00254AB0(void);
s32 func_002FC1A0(s32, s32);
s32 func_002FC428(s32);
s32 func_002FC5A8(s32, s32, s32);
extern s8 D_00565DF4;
extern s32 D_0078DFBC;

s32 func_00254E08(s32 id) {
    ArchiveState* state = func_00162DD0();
    s32 size = func_00165D70(D_0078DFBC, id);
    s32 name;
    s32 index;
    s32 fd;

    if (size != 0) {
        return size;
    }
    if (D_00565DF4 != 0) {
        size = func_00162EB8(id);
    } else {
        index = state->unk20 + id;
        if (func_00254AB0() != 0) {
            name = func_00254AD0(index);
            if (name != 0) {
                fd = func_002FC1A0(name, 1);
                if (fd < 0) {
                    return 0;
                }
                size = func_002FC5A8(fd, 0, 2);
                size = (size + 15) / 16 * 16;
                func_002FC428(fd);
            }
        }
    }
    return size;
}

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
