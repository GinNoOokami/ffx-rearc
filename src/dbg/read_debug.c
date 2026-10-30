#include "common.h"

typedef struct {
    /* 0x00 */ u8 pad0[0x20];
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ void* unk28;
    /* 0x2C */ s16* unk2C;
    /* 0x30 */ void* unk30;
} ArchiveState;

ArchiveState* func_00162DD0(void);

extern s8 D_00326E1C;
extern s32 D_006A2C90[];
extern s32 D_006A2CD0[];

extern char D_00573400[]; // "read_debug.c", emitted by INCLUDE_RODATA
extern ArchiveState D_0078DF98;

void func_00157788(const char*, const char*, s32); // dbgAssert

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
        func_00157788("i!=FILE_NAME_CHG_TBL_CNT", D_00573400, 0x50);
    }

    D_006A2C90[i] = arg0;
    D_006A2CD0[i] = arg1;
    return i;
}

void func_00254A88(s32 arg0) {
    D_006A2C90[arg0] = 0;
    D_006A2CD0[arg0] = 0;
}

void* func_00254AB0(void) { return func_00162DD0()->unk30; }

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00254AD0);

const char* func_00254AD0(s32);
const char* func_00254DA8(s32 arg0) { return func_00254AD0(func_00162DD0()->unk20 + arg0); }

extern s16* D_0078DFC4;

const char* func_00254DD8(s32 arg0, s32 arg1) { return func_00254AD0(D_0078DFC4[arg0] + arg1); }

ArchiveState* func_00162DD0(void);
s32 func_00162EB8(s32);
s32 func_00165D70(s32, s32);
s32 func_002FC1A0(const char*, s32); // getFd
s32 func_002FC428(s32);
s32 func_002FC5A8(s32, s32, s32);
extern s8 D_00565DF4;
extern s32 D_0078DFBC;

s32 func_00254E08(s32 id) {
    ArchiveState* state = func_00162DD0();
    s32 size = func_00165D70(D_0078DFBC, id);
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
            const char* name = func_00254AD0(index);
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

/*?*/ s32 func_00163138(s32); /* extern */
extern s32 D_0078DFB8;

s32 func_00254EF0(s32 arg0) { return func_00163138(D_0078DFB8 + arg0); }

INCLUDE_RODATA("asm/jp_intl/nonmatchings/dbg/read_debug", D_00573400);

// Assert an invalid fd
void func_00254F18(s32 fd, const char* path) {
    if (fd < 0) {
        func_00157788("fd >= 0", D_00573400, 0x14D);
    }
}

/*?*/ void FlushCache(/*?*/ s32);
void* func_0011D370(s32, s32, const char*, s32); // memalloc
void func_002FC7E0(s32, void*, s32);

void* func_00254F40(const char* path, s32* arg1) {
    void* block;
    s32 size;
    s32 fd;

    fd = func_002FC1A0(path, 1);
    func_00254F18(fd, path);

    size = func_002FC5A8(fd, 0, 2);
    func_002FC5A8(fd, 0, 0);

    block = func_0011D370((size + 3) / 4 * 4, 10, D_00573400, 0x165);

    FlushCache(0);

    func_002FC7E0(fd, block, size);
    func_002FC428(fd);
    FlushCache(0);

    if (arg1 != NULL) {
        *arg1 = size;
    }

    return block;
}

void* func_0011D420(s32, s32, const char*, s32); // Mem_AllocDebug
s32 func_002D93E8(void);
extern s8 D_00565DF0;

void* func_00255020(const char* path, s32* arg1) {
    s32 size;
    s32 fd;
    void* block;

    fd = func_002FC1A0(path, 1);
    func_00254F18(fd, path);

    size = func_002FC5A8(fd, 0, 2);
    func_002FC5A8(fd, 0, 0);

    if ((func_002D93E8() == 1) && D_00565DF0 == 0) {
        block = func_0011D370((size + 3) / 4 * 4, 10, D_00573400, 0x188);
    } else {
        block = func_0011D420((size + 3) / 4 * 4, 10, D_00573400, 0x18A);
    }
    FlushCache(0);

    func_002FC7E0(fd, block, size);
    func_002FC428(fd);
    FlushCache(0);

    if (arg1 != NULL) {
        *arg1 = size;
    }

    return block;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255158);

void func_0011D590(void*, /*?*/ s32);

void func_002556A0(const char* path) {
    if (D_0078DF98.unk30 != 0) {
        func_0011D590(D_0078DF98.unk30, 0xA);
    }
    D_0078DF98.unk30 = func_00255020(path, NULL);
}

/*?*/ s32 func_001ACEE8(char *);
extern char D_00326CE0[]; // "host:/ffx/proj/battle/jp/cddata/cdrom.mdg"
extern char D_00326D10[]; // "host:/ffx/proj/battle/jp/cddata/cdrom.fid"
extern char D_00326D40[]; // "host:/ffx/proj/battle/jp/cddata/cdrom.fnd"
extern char D_00326D70[]; // "host:/ffx/proj/prog/cdidx/jp/cdrom.mdg"
extern char D_00326D98[]; // "host:/ffx/proj/prog/cdidx/jp/cdrom.fid"

void func_002556F8(void) {
    char region[2];

    func_001ACEE8(region);

    if (D_00565DF4 == 0) {
        D_00326CE0[0x16] = region[0];
        D_00326CE0[0x17] = region[1];
        D_00326D10[0x16] = region[0];
        D_00326D10[0x17] = region[1];
        D_00326D40[0x16] = region[0];
        D_00326D40[0x17] = region[1];
        D_0078DF98.unk28 = func_00254F40(D_00326CE0, NULL);
        D_0078DF98.unk2C = func_00254F40(D_00326D10, NULL);
        D_0078DF98.unk30 = func_00255020(D_00326D40, NULL);
    } else {
        D_00326D70[0x1A] = region[0];
        D_00326D70[0x1B] = region[1];
        D_00326D98[0x1A] = region[0];
        D_00326D98[0x1B] = region[1];
        D_0078DF98.unk28 = func_00254F40(D_00326D70, NULL);
        D_0078DF98.unk2C = func_00254F40(D_00326D98, NULL);
        D_0078DF98.unk30 = 0;
    }
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255800);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255840);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255880);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_002558A0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_002558A8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_002558B0);
