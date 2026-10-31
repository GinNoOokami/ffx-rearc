#include <stdio.h>

#include "common.h"

#define FILE_NAME_CHG_TBL_CNT 16

// Archive state (D_0078DF98). Function slots are filled by func_00165588; the
// comments give the release function, and the host one from this file where
// func_002558B0 overrides the slot. "()" = parameters not known yet.
typedef struct {
    /* 0x00 */ s32 (*unk0)();     // func_00164040
    /* 0x04 */ s32 (*unk4)(s32);  // func_00163198 / func_00254EF0
    /* 0x08 */ s32 (*unk8)();     // func_00164970
    /* 0x0C */ void (*unkC)(s32); // func_00165588 (this table's own init)
    /* 0x10 */ s32 (*unk10)();    // func_00163FD0
    /* 0x14 */ s32 (*unk14)(s32); // func_00162EB8 / func_00254E08
    /* 0x18 */ s32 unk18;         // func_00165588's argument; 0 = host mode
    /* 0x1C */ s32 (*unk1C)();    // func_00165300
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ void* unk28; // cdrom.mdg
    /* 0x2C */ s16* unk2C;  // cdrom.fid
    /* 0x30 */ void* unk30; // cdrom.fnd (host mode)
    /* 0x34 */ u8 pad34[0x54 - 0x34];
    /* 0x54 */ void* (*unk54)(const char*, s32*); // func_00162D30 / func_00254F40
    /* 0x58 */ const char* (*unk58)(s32);         // func_00164A10 / func_00254DA8
    /* 0x5C */ s32 unk5C;                         // file descriptor during a host read (func_00255158); -1 when idle
    /* 0x60 */ void (*unk60)(const char*);        // func_00162D18 / func_002556A0
    /* 0x64 */ s32 (*unk64)();                    // func_00162E48
    /* 0x68 */ void (*unk68)();                   // func_00163E90 / func_00255158
    /* 0x6C */ s32 (*unk6C)();                    // func_00163168
    /* 0x70 */ s32 unk70;                         // 1 while func_00255158 processes an entry, 0 after
    /* 0x74 */ s32 unk74;                         // index of that entry in D_0078E030, -1 after
    /* 0x78 */ s32 (*unk78)(void);                // func_00164628 / func_00255880
    /* 0x7C */ s32 (*unk7C)();                    // func_00165378
    /* 0x80 */ s32 (*unk80)();                    // func_001648F8
    /* 0x84 */ s32 (*unk84)();                    // func_001653A0
    /* 0x88 */ s32 (*unk88)();                    // func_00165548 / func_002558A0
    /* 0x8C */ s32 (*unk8C)();                    // func_00165528 / func_002558A8
    /* 0x90 */ s32 (*unk90)();                    // func_00164320
} ArchiveState;

ArchiveState* func_00162DD0(void);

extern s8 D_00326E1C;
extern const char* D_006A2C90[FILE_NAME_CHG_TBL_CNT];
extern const char* D_006A2CD0[FILE_NAME_CHG_TBL_CNT];

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

s32 func_002549A8(const char* arg0, const char* arg1) {
    s32 i = 0;

    if (D_006A2C90[0] != NULL) {
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

// getFileTbl
void* func_00254AB0(void) { return func_00162DD0()->unk30; }

const char* func_0011A480(void); // returns "sugimoto"
extern char D_006A2D10[0x200];   // name buffer, size inferred

// Returns file `index`'s name from the cdrom.fnd table, expanded into a static buffer:
// "$USER$" becomes func_0011A480()'s string, and a prefix found in the name-change table
// (func_002549A8) becomes its replacement. NULL if there is no table or no such name.
const char* func_00254AD0(s32 index) {
    const char* name = NULL;
    const char* user;
    const char* str;
    s32* fileTbl;
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 n;

    // fileTbl[0] = entry count, fileTbl[1 + n] = offset of name n from the table start; a name
    // runs to the next offset, and an entry with no length has no name.
    fileTbl = func_00254AB0();
    if (fileTbl != NULL && index < fileTbl[0]) {
        s32 offset = fileTbl[index + 1];

        if (fileTbl[index + 2] - offset > 0) {
            name = (char*)fileTbl + offset;
        }
    }

    if (name != NULL) {
        user = func_0011A480();
        for (j = 0, i = 0; D_006A2D10[j] = name[i], name[i] != '\0'; i++, j++) {
            const char* p = &name[i];

            if (p[0] == '$' && p[1] == 'U' && p[2] == 'S' && p[3] == 'E' && p[4] == 'R' && p[5] == '$') {
                if (user != NULL) {
                    for (k = 0; D_006A2D10[j++] = user[k], user[k] != '\0'; k++) {
                    }
                    // The copy leaves j past the '\0'; back up so the loop's j++ lands on it.
                    j -= 2;
                    i += 5;
                }
            } else {
                // A match doesn't end the search: later entries are tried at the new position,
                // and a second match overwrites the last character of the first replacement.
                for (n = 0; n < FILE_NAME_CHG_TBL_CNT; n++) {
                    if (D_006A2C90[n] == NULL) {
                        continue;
                    }

                    str = D_006A2C90[n];
                    for (k = 0; name[i + k] != '\0' && str[k] != '\0' && name[i + k] == str[k]; k++) {
                    }

                    if (str[k] == '\0') {
                        str = D_006A2CD0[n];
                        for (m = 0; D_006A2D10[j++] = str[m], str[m] != '\0'; m++) {
                        }
                        j -= 2;
                        i += k - 1;
                    }
                }
            }
        }

        name = D_006A2D10;
    }

    return name;
}

const char* func_00254DA8(s32 arg0) { return func_00254AD0(func_00162DD0()->unk20 + arg0); }

extern s16* D_0078DFC4;

const char* func_00254DD8(s32 arg0, s32 arg1) { return func_00254AD0(D_0078DFC4[arg0] + arg1); }

ArchiveState* func_00162DD0(void);
s32 func_00162EB8(s32);
s32 func_00165D70(s32, s32);
s32 func_002FC1A0(const char*, s32); // sceOpen (returns fd)
s32 func_002FC428(s32);              // sceClose (arg0 = fd)
s32 func_002FC5A8(s32, s32, s32);    // sceLseek
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
        if (func_00254AB0() != NULL) {
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
void func_002FC7E0(s32, void*, s32);             // sceRead

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

// 64 entries at D_0078E030 (= archive state +0x98), stride 0x38. Fields as func_00255158 uses them.
typedef struct {
    /* 0x00 */ u32 unk0;   // read size (func_002FC7E0); compared unsigned with unk2C
    /* 0x04 */ void* unk4; // destination buffer (func_002FC7E0, memcpy)
    /* 0x08 */ s32 unk8;   // func_00165D00's second argument
    /* 0x0C */ u8 padC[0x1C - 0xC];
    /* 0x1C */ s32 unk1C;   // sector: unk1C << 11 is the seek offset
    /* 0x20 */ void* unk20; // nonzero: func_00163938(entry) is called
    /* 0x24 */ u8 pad24[0x28 - 0x24];
    /* 0x28 */ s32 unk28; // func_00165D00's first argument; func_00163450's fifth
    /* 0x2C */ u32 unk2C; // size: rounded to and masked by 0x800
    /* 0x30 */ u8 pad30[0x31 - 0x30];
    /* 0x31 */ s8 unk31; // 0: entry skipped; cleared at the end
    /* 0x32 */ s8 unk32; // nonzero: func_00162DE0(index) instead
    /* 0x33 */ s8 unk33; // selects the copy path
    /* 0x34 */ u8 pad34[0x38 - 0x34];
} Unk0078E030;

// Entries of the table at *D_00595AEC (D_0030DAB0 entries), found by func_00165D00.
typedef struct {
    /* 0x00 */ u8 unk0; // matched against func_00165D00's first argument
    /* 0x01 */ u8 pad1[0x8 - 0x1];
    /* 0x08 */ s32 unk8;  // matched against its second argument
    /* 0x0C */ s32 unkC;  // seek offset in the loose file
    /* 0x10 */ s32 unk10; // read size
    /* 0x14 */ u8 pad14[0x24 - 0x14];
    /* 0x24 */ char unk24[0x10]; // file name under pfs0:/ex/
} Unk00595AEC;                   // 0x34 bytes

typedef struct {
    /* 0x00 */ u8 pad0[0xB];
    /* 0x0B */ s8 unkB;
    /* 0x0C */ s8 unkC;
    /* 0x0D */ u8 padD[0x10 - 0xD];
    /* 0x10 */ s8 unk10;
} Unk00588A00;

Unk00588A00* func_001244D0(void);
void* func_00162D48();                                 /* extern */
/*?*/ void func_00162DE0(s32);                         /* extern */
/*?*/ void func_00163450(s32, s32, void*, void*, s32); /* extern */
/*?*/ void func_00163768(void*);                       /* extern */
/*?*/ void func_00163938(void*);                       /* extern */
Unk00595AEC* func_00165D00(s32, s32);
/*?*/ void func_0017133C(/*?*/ s32); /* extern */
s32 func_002D1C60(void);
/*?*/ void func_002EEBAC(void*, void*, s32); /* extern */
s32 func_002FCD10(s32, s32, s32*);           // sceIoctl
extern volatile u8 D_0030DAB4;
extern void* D_0057C700;
extern char D_006A2F10[];
extern /*?*/ Unk0078E030 D_0078E030[];
extern char D_0078EE30[64][128];

// Not matching yet. With NON_MATCHING the C below is one instruction off, but only because of
// three changes that steer the compiler and are unlikely to be in the original:
// - `goto copy`: the target shares one FlushCache/memcpy tail between the two reads of the
//   unk2C block. Written as plain if/else, the first scheduler moves `rest = size & 0x7FF`
//   above the secs == 0 read call (the target keeps it after); the call then ends its basic
//   block, flow inserts a (use (const_int 0)) after it, and cross-jumping no longer merges
//   the two identical tails.
// - the empty `do {} while (0)` at the end of the D_00565DF4 != 0 branch: without it there
//   is an extra jump at the end of that branch.
// - `one`: the ex-file waits pass a local set to 1. It makes `archive->unk70 = 1` use a0 and
//   keeps the D_0030DAB4 = 1 / D_0057C700 = entry store order, as in the target.
// Still different: in the secs == 0 read, `rest` lands in the call's delay slot; the target
// sets a3 = sp there and computes rest after the call. The same C without the three changes
// is 23 instructions off. The release counterpart func_00163950 has the same read logic.
#ifndef NON_MATCHING
INCLUDE_ASM("asm/jp_intl/nonmatchings/dbg/read_debug", func_00255158);
#else
void func_00255158(void) {
    u8 mode[4];
    s32 stat;
    ArchiveState* archive;
    char* buf;
    Unk0078E030* entry;
    Unk00595AEC* exFile;
    s32 fd;
    s32 size;
    s32 secs;
    s32 rest;
    s32 i;
    s32 one;

    archive = func_00162DD0();
    buf = func_00162D48();

    for (i = 0; i < 64; i++) {
        entry = &D_0078E030[i];
        if (entry->unk31 == 0) {
            continue;
        }
        if (entry->unk32 != 0) {
            func_00162DE0(i);
            continue;
        }

        archive->unk74 = i;
        archive->unk70 = 1;
        exFile = func_00165D00(entry->unk28, entry->unk8);
        if (exFile != NULL) {
            sprintf(D_006A2F10, "pfs0:/ex/%s", exFile->unk24);
            one = 1;
            fd = func_002FC1A0(D_006A2F10, 0x8001);
            if (fd < 0) {
                dbgBreak();
            }
            while (func_002FCD10(fd, 1, &stat), stat != 0) {
                func_0017133C(one);
            }
            func_002FC5A8(fd, exFile->unkC, 0);
            while (func_002FCD10(fd, 1, &stat), stat != 0) {
                func_0017133C(one);
            }
            FlushCache(0);
            func_002FC7E0(fd, entry->unk4, exFile->unk10);
            while (func_002FCD10(fd, 1, &stat), stat != 0) {
                func_0017133C(one);
            }
            func_002FC428(fd);
            FlushCache(0);
        } else if (D_00565DF4 != 0) {
            size = entry->unk2C;
            if (entry->unk33 != 0 && entry->unk0 < size) {
                size = entry->unk0 + 5;
                secs = size / 2048;
                if (secs != 0 && size > 0) {
                    rest = secs * 2048;
                    func_00163450(entry->unk1C, size, entry->unk4, mode, entry->unk28);
                    FlushCache(0);
                    func_002EEBAC(entry->unk4, (char*)entry->unk4 + 5, rest);
                    func_00163450(entry->unk1C + secs, 0x800, buf, mode, entry->unk28);
                    FlushCache(0);
                    size -= rest;
                    func_002EEBAC((char*)entry->unk4 + rest, buf, size - 5);
                } else {
                    func_00163450(entry->unk1C, 0x800, buf, mode, entry->unk28);
                    FlushCache(0);
                    func_002EEBAC(entry->unk4, buf + 5, size - 5);
                }
            } else {
                secs = size / 2048;
                if (secs != 0) {
                    func_00163450(entry->unk1C, entry->unk2C, entry->unk4, mode, entry->unk28);
                    rest = size & 0x7FF;
                    if (rest != 0) {
                        func_00163450(entry->unk1C + secs, 0x800, buf, mode, entry->unk28);
                        goto copy;
                    }
                    FlushCache(0);
                } else {
                    func_00163450(entry->unk1C, 0x800, buf, mode, entry->unk28);
                    rest = size & 0x7FF;
                copy:
                    FlushCache(0);
                    func_002EEBAC((char*)entry->unk4 + size - rest, buf, rest);
                }
                if (entry->unk33 != 0) {
                    if (func_001244D0()->unk10 != 0) {
                        while (D_0030DAB4 != 0) {
                            func_0017133C(1);
                        }
                        D_0030DAB4 = 1;
                        D_0057C700 = entry;
                        func_002D1C60();
                        while (D_0030DAB4 != 0) {
                            func_0017133C(1);
                        }
                    } else {
                        func_00163768(entry);
                    }
                }
            }
            do {
            } while (0);
        } else {
            if (entry->unk1C != 0) {
                archive->unk5C = func_002FC1A0(D_0078EE30[i], 1);
                func_00254F18(archive->unk5C, D_0078EE30[i]);
                func_002FC5A8(archive->unk5C, entry->unk1C << 11, 0);
                FlushCache(0);
                func_002FC7E0(archive->unk5C, entry->unk4, entry->unk0);
                func_002FC428(archive->unk5C);
                FlushCache(0);
            } else {
                archive->unk5C = func_002FC1A0(D_0078EE30[i], 0x8001);
                func_00254F18(archive->unk5C, D_0078EE30[i]);
                FlushCache(0);
                func_002FC7E0(archive->unk5C, entry->unk4, entry->unk0);
                while (func_002FCD10(archive->unk5C, 1, &stat), stat != 0) {
                    func_0017133C(1);
                }
                func_002FC428(archive->unk5C);
                FlushCache(0);
            }
        }

        if (entry->unk20 != NULL) {
            func_00163938(entry);
        }
        archive->unk70 = 0;
        archive->unk74 = -1;
        archive->unk5C = -1;
        entry->unk31 = 0;
    }
}
#endif

void func_0011D590(void*, /*?*/ s32);

// Debug PcFileTblSet
void func_002556A0(const char* path) {
    if (D_0078DF98.unk30 != NULL) {
        func_0011D590(D_0078DF98.unk30, 10);
    }
    D_0078DF98.unk30 = func_00255020(path, NULL);
}

/*?*/ s32 func_001ACEE8(char*); // getRegionCode?
extern char D_00326CE0[];       // host:/ffx/proj/battle/jp/cddata/cdrom.mdg
extern char D_00326D10[];       // host:/ffx/proj/battle/jp/cddata/cdrom.fid
extern char D_00326D40[];       // host:/ffx/proj/battle/jp/cddata/cdrom.fnd
extern char D_00326D70[];       // host:/ffx/proj/prog/cdidx/jp/cdrom.mdg
extern char D_00326D98[];       // host:/ffx/proj/prog/cdidx/jp/cdrom.fid

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

/*?*/ s32 func_00164628(); /* extern */

s32 func_00255880(void) { return func_00164628(); }

s32 func_002558A0(void) { return -1; }

s32 func_002558A8(void) { return -1; }

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
