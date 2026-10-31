#include <stdio.h>
#include <string.h>
#include <sifdev.h>
#include <libcdvd.h>

#include "common.h"
#include "libmscd.h"
#include "mem.h"

typedef struct {
    /* 0x00 */ u8 pad0[0x1C];
    /* 0x1C */ s32 unk1C;
} Unk0057C71C;

void FlushCache(s32);
void func_00125B40(void);
s32 func_00165EB8(void);
void func_00166798(char*, void*, s32, s32);

extern s32 D_0030DA80;
extern s32 D_0030DA84;
extern sceCdRMode D_0030DA98;
extern sceCdRMode D_0030DAA0;
extern s8 D_0030DAA4;
extern s8 D_0030DAA5;
extern s32 D_0030DAA8;
extern s32 D_0030DAB0;
extern char D_0056B720[]; // "libmscd.c"
extern char D_0056B730[]; // "PcFileTblSet  DVD MODE ONLY"
extern char D_0056B768[]; // "pfs0:/ex/%s"
extern Unk0057C71C* D_0057C71C;
extern char* D_00595AD0;
extern s32 D_00595AD4;        // sectorNum
extern s32 D_00595AD8;        // size
extern void* D_00595ADC;      // buffer
extern sceCdRMode D_00595AE0; // mode
extern Unk00595AEC* D_00595AEC;
extern char D_00595AF0[];

void func_00162CB8(s8 arg0) { D_0030DAA5 = arg0; }

void func_00162CC8(void) {
    D_0030DAB0 = 0;
    D_00595AEC = (Unk00595AEC*)func_0011D370(512 * sizeof(Unk00595AEC), 10, D_0056B720, 0x203);
    memset(D_00595AEC, 0, 512 * sizeof(Unk00595AEC));
}

void func_00162D18(const char* path) { dbgAssert(D_0056B730, D_0056B720, 0x20C); }

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162D30);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162D48);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162D58);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162DC0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162DD0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162DE0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162E48);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162EB8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162FA0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163090);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_001630F0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163138);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163168);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163198);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_001631C0);

void func_0015B330(void);

s32 func_001631D0(s32 arg0, s32 size, void* buffer, sceCdRMode* mode) {
    s32 sectors;
    D_0030DAA4 = 0;
    D_0030DAA8 = 0;

    D_00595AD4 = arg0;
    D_00595AD8 = size;
    D_00595ADC = buffer;
    D_00595AE0 = *mode;

    func_0015B330();

    sectors = size / 2048;
    FlushCache(0);
    return sceCdRead(arg0, sectors, buffer, mode);
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163290);

u32 func_00163168(/*?*/ s32, /*?*/ s32); /* extern */
extern s8 D_0030DAA5;
extern char* D_0030DCD8;

// After the disc lid was opened: waits for the drive, then re-reads a sector until its first 32 bytes
// match D_0030DCD8 (same disc). Returns 1 if the lid wasn't opened or there is no ID, else 0.
// gotos: gcc 2.95 moves a loop's test to the bottom if a break/condition exits it near the top, but the
// inner wait loop keeps its test at the top; no break/continue shape found matches, these gotos do.
s32 func_001632B8(void) {
    u32 lsn;
    s32 result;

    if (sceCdStatus() == SCECdStatShellOpen) {
    restart:
        while (1) {
            if (sceCdDiskReady(1) == SCECdComplete && sceCdStatus() != SCECdStatShellOpen) {
            read:
                if (D_0030DAA5 == 0) {
                    return 0;
                }
                if (D_0030DCD8 == NULL) {
                    return 1;
                }

                func_0017133C(1);
                func_0015B330();

                lsn = func_00163168(1, 1);
                FlushCache(0);
                if (sceCdRead(lsn, 1, D_00595AD0, &D_0030DA98) == 0) {
                    goto restart;
                }

                while ((result = func_0015B830()) != 0) {
                    if (result != -1) {
                        goto restart;
                    }
                    func_0017133C(1);
                }

                FlushCache(0);
                D_00595AD0[32] = '\0';
                if (memcmp(D_00595AD0, D_0030DCD8, 32) == 0) {
                    return 0;
                }

                while (1) {
                    if (sceCdDiskReady(1) == SCECdComplete) {
                        if (sceCdStatus() == SCECdStatShellOpen) {
                            goto restart;
                        }
                        goto read;
                    }
                    func_0017133C(1);
                }
            }

            D_0030DAA4 = 1;
            func_0017133C(1);
        }
    }
    return 1;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163450);

s32 func_0015B830(); /* extern */

s32 func_001636F0(void) {
    s32 status = func_0015B830();

    if (status == 0) {
        return 0;
    }

    if (status == -1) {
        return 1;
    }

    if (sceCdDiskReady(1) != SCECdComplete) {
        return -1;
    }

    sceCdBreak();
    func_001631D0(D_00595AD4, D_00595AD8, D_00595ADC, &D_0030DAA0);
    return 1;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163768);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163938);

// Not matching yet. For compressed entries (entry->unk33, unk0 < size) the function reads the first
// sectors, tests the first data byte and, if it is nonzero, carries on with the generic read code:
// the long read joins at the `rest` test (0x163B68), the one-sector read at the FlushCache/memcpy
// tail, so that path runs FlushCache twice (0x163AFC, 0x163BE8). The PC port's version (0x76E1C0)
// has the same flow.
// Writing those continuations out as repeated code reproduces most of it (about 84%): gcc
// cross-jumps the copies into the generic code. The difference left: at the shared partial read the
// target recomputes size / 2048 (0x163B70-0x163B84) instead of reusing the value from the test,
// which needs that code to be reachable from both paths before CSE, i.e. a join in the source.
// Labels at the two joins give about 90% but are unlikely to be original. Helpers are ruled out:
// this compiler won't inline a static inline function that ends in a call (memcpy), so they'd
// show up as calls. Also unsolved: the exits to the shared end (timing stores, cleanup) after the
// HDD read and the uncompressed copies are still gotos in every version tried.
// Compiler options are not the cause: the rest of read_debug.c (same library) matches with
// -O2 -G0, and -fno-cse-follow-jumps, the only option found that makes the division recompute,
// breaks two of those matches.
#ifndef NON_MATCHING
INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163950);
#else
void func_00163950(ArchiveState* archive, Unk0078E030* entry, s32 index) {
    Unk00595AEC* exFile;
    s32 start;
    s32 size;
    s32 secs;
    s32 rest;

    if (archive->unk70 != 0) {
        return;
    }

    archive->unk74 = index;
    archive->unk70 = 1;
    archive->unk5C = 0;
    start = D_0057C71C->unk1C;
    size = entry->unk2C;
    exFile = func_00165D00(entry->unk28, entry->unk8);

    if (exFile != NULL) {
        sprintf(D_00595AF0, D_0056B768, exFile->unk24);
        func_00166798(D_00595AF0, entry->unk4, exFile->unk10, exFile->unkC);
        if (func_00165EB8() == 0) {
            goto end;
        }
        D_00565DF4 = 0;
        func_00125B40();
    }

    if (entry->unk33 != 0 && entry->unk0 < size) {
        size = entry->unk0 + 5;
        secs = size / 2048;
        if (secs != 0 && size > 0) {
            func_00163450(entry->unk1C, size, entry->unk4, &D_0030DA98, entry->unk28);
            FlushCache(0);

            if (*(char*)entry->unk4 == 0) {
                memcpy(entry->unk4, (char*)entry->unk4 + 5, secs * 2048);
                size -= secs * 2048;
                func_00163450(entry->unk1C + secs, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
                FlushCache(0);
                memcpy((char*)entry->unk4 + secs * 2048, D_00595AD0, size - 5);
                goto end;
            }

            if (size & 0x7FF) {
                func_00163450(entry->unk1C + size / 2048, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
                rest = size & 0x7FF;
                FlushCache(0);
                memcpy((char*)entry->unk4 + size - rest, D_00595AD0, rest);
            } else {
                FlushCache(0);
            }
        } else {
            func_00163450(entry->unk1C, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
            FlushCache(0);

            if (*D_00595AD0 == 0) {
                memcpy(entry->unk4, D_00595AD0 + 5, size - 5);
                goto end;
            }

            rest = size & 0x7FF;
            FlushCache(0);
            memcpy((char*)entry->unk4 + size - rest, D_00595AD0, rest);
        }
    } else {
        if (size / 2048 != 0) {
            func_00163450(entry->unk1C, entry->unk2C, entry->unk4, &D_0030DA98, entry->unk28);
            if (size & 0x7FF) {
                func_00163450(entry->unk1C + size / 2048, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
                rest = size & 0x7FF;
                FlushCache(0);
                memcpy((char*)entry->unk4 + size - rest, D_00595AD0, rest);
            } else {
                FlushCache(0);
            }
        } else {
            func_00163450(entry->unk1C, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
            rest = size & 0x7FF;
            FlushCache(0);
            memcpy((char*)entry->unk4 + size - rest, D_00595AD0, rest);
        }
    }

    if (entry->unk33 != 0) {
        if (func_001244D0()->unk10 != 0) {
            while (D_0030DAB4 != 0) {
                func_0017133C(1);
            }
            D_0057C700 = entry;
            D_0030DAB4 = 1;
            func_002D1C60();
            while (D_0030DAB4 != 0) {
                func_0017133C(1);
            }
        } else {
            func_00163768(entry);
        }
    }

end:
    D_0030DA84 = D_00595AD8;
    D_0030DA80 = D_0057C71C->unk1C - start;
    if (entry->unk20 != NULL) {
        func_00163938(entry);
    }

    archive->unk70 = 0;
    archive->unk5C = -1;
    archive->unk74 = -1;
    entry->unk31 = 0;
    entry->unk24 = 0;
}
#endif

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163D20);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163E90);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163FD0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164040);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164320);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164628);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164820);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_001648F8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164970);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164A10);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164A20);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164A50);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164A60);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164A70);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165300);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165378);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_001653A0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_001653E8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165528);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165548);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165588);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_001656B8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_001656E0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_001657A8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165808);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165A18);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165A50);

void func_00165AE0(void) {}

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165AE8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165B18);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165D00);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165D70);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165DD0);
