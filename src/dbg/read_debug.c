#include "common.h"

// Archive state (D_0078DF98). Function slots are filled by func_00165588; the
// comments give the release function, and the host one from this file where
// func_002558B0 overrides the slot. "()" = parameters not known yet.
typedef struct {
    /* 0x00 */ s32 (*unk0)();                        // func_00164040
    /* 0x04 */ s32 (*unk4)(s32);                     // func_00163198 / func_00254EF0
    /* 0x08 */ s32 (*unk8)();                        // func_00164970
    /* 0x0C */ void (*unkC)(s32);                    // func_00165588 (this table's own init)
    /* 0x10 */ s32 (*unk10)();                       // func_00163FD0
    /* 0x14 */ s32 (*unk14)(s32);                    // func_00162EB8 / func_00254E08
    /* 0x18 */ s32 unk18;                            // func_00165588's argument; 0 = host mode
    /* 0x1C */ s32 (*unk1C)();                       // func_00165300
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ void* unk28;                          // cdrom.mdg
    /* 0x2C */ s16* unk2C;                           // cdrom.fid
    /* 0x30 */ void* unk30;                          // cdrom.fnd (host mode)
    /* 0x34 */ u8 pad34[0x54 - 0x34];
    /* 0x54 */ void* (*unk54)(const char*, s32*);    // func_00162D30 / func_00254F40
    /* 0x58 */ const char* (*unk58)(s32);            // func_00164A10 / func_00254DA8
    /* 0x5C */ u8 pad5C[0x60 - 0x5C];
    /* 0x60 */ void (*unk60)(const char*);           // func_00162D18 / func_002556A0
    /* 0x64 */ s32 (*unk64)();                       // func_00162E48
    /* 0x68 */ void (*unk68)();                      // func_00163E90 / func_00255158
    /* 0x6C */ s32 (*unk6C)();                       // func_00163168
    /* 0x70 */ u8 pad70[0x78 - 0x70];
    /* 0x78 */ s32 (*unk78)(void);                   // func_00164628 / func_00255880
    /* 0x7C */ s32 (*unk7C)();                       // func_00165378
    /* 0x80 */ s32 (*unk80)();                       // func_001648F8
    /* 0x84 */ s32 (*unk84)();                       // func_001653A0
    /* 0x88 */ s32 (*unk88)();                       // func_00165548 / func_002558A0
    /* 0x8C */ s32 (*unk8C)();                       // func_00165528 / func_002558A8
    /* 0x90 */ s32 (*unk90)();                       // func_00164320
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

// Debug PCload
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

// Debug PcFileTblSet
void func_002556A0(const char* path) {
    if (D_0078DF98.unk30 != NULL) {
        func_0011D590(D_0078DF98.unk30, 10);
    }
    D_0078DF98.unk30 = func_00255020(path, NULL);
}

/*?*/ s32 func_001ACEE8(char *); // getRegionCode?
extern char D_00326CE0[]; // host:/ffx/proj/battle/jp/cddata/cdrom.mdg
extern char D_00326D10[]; // host:/ffx/proj/battle/jp/cddata/cdrom.fid
extern char D_00326D40[]; // host:/ffx/proj/battle/jp/cddata/cdrom.fnd
extern char D_00326D70[]; // host:/ffx/proj/prog/cdidx/jp/cdrom.mdg
extern char D_00326D98[]; // host:/ffx/proj/prog/cdidx/jp/cdrom.fid

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

extern char D_00326DC0[]; // host:/ffx/proj/prog/cdidx/jp/sizetbl.bin

void* func_00255800(void) {
    char region[2];

    func_001ACEE8(region);

    D_00326DC0[0x1A] = region[0];
    D_00326DC0[0x1B] = region[1];

    return func_00254F40(D_00326DC0, NULL);
}

extern char D_00326DF0[]; // host:/ffx/proj/prog/cdidx/jp/modulesize.bin

void* func_00255840(void) {
    char region[2];

    func_001ACEE8(region);

    D_00326DF0[0x1A] = region[0];
    D_00326DF0[0x1B] = region[1];

    return func_00254F40(D_00326DF0, NULL);
}

/*?*/ s32 func_00164628();                                  /* extern */

s32 func_00255880(void) {
    return func_00164628();
}

s32 func_002558A0(void) {
    return -1;
}

s32 func_002558A8(void) {
    return -1;
}

/*?*/ void func_00255158();

// Installs this file's loader functions into the archive state (D_0078DF98).
// func_00165588(arg) stores arg at +0x18; with arg == 0 it calls this function,
// otherwise it stores the release versions into the same nine slots:
//
//   slot   debug (here)    release         type (from the debug versions)
//   +0x04  func_00254EF0   func_00163198   s32 (*)(s32)
//   +0x14  func_00254E08   func_00162EB8   s32 (*)(s32)
//   +0x54  func_00254F40   func_00162D30   void* (*)(const char*, s32*)
//   +0x58  func_00254DA8   func_00164A10   const char* (*)(s32)
//   +0x60  func_002556A0   func_00162D18   void (*)(const char*)
//   +0x68  func_00255158   func_00163E90   ?
//   +0x78  func_00255880   func_00164628   s32 (*)(void)
//   +0x88  func_002558A0   func_00165548   s32 (*)(?)
//   +0x8C  func_002558A8   func_00165528   s32 (*)(?)
//
// func_00255880 just calls the release +0x78, and func_00254E08 calls the
// release +0x14 when D_00565DF4 != 0. The +0x88/+0x8C stubs only return -1,
// so their parameters, if any, are not visible here.

void func_002558B0(void) {
    D_0078DF98.unk14 = func_00254E08;
    D_0078DF98.unk58 = func_00254DA8;
    D_0078DF98.unk54 = func_00254F40;
    D_0078DF98.unk68 = func_00255158;
    D_0078DF98.unk60 = func_002556A0;
    D_0078DF98.unk78 = func_00255880;
    D_0078DF98.unk88 = func_002558A0;
    D_0078DF98.unk8C = func_002558A8;
    D_0078DF98.unk4 = func_00254EF0;
}
