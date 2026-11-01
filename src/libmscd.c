#include <stdio.h>
#include <string.h>
#include <sifdev.h>
#include <libcdvd.h>
#include <stdarg.h>

#include "common.h"
#include "libmscd.h"
#include "mem.h"

#define DBG_LINE_SIZE 40
#define DBG_LINE_COUNT 128

typedef enum {
    DBG_OUT_NONE,
    DBG_OUT_LOG_FILE,
    DBG_OUT_LOG_MEMORY,
    DBG_OUT_STREAM_FFXDEBUG,
    DBG_OUT_STREAM_FILE
} DebugOutLogMode;

typedef struct {
    /* 0x0 */ u32 unk0; // CDROM.MDG LSN
    /* 0x4 */ u32 unk4; // CDROM.MDG size
    /* 0x8 */ u32 unk8; // CDROM.FID LSN
    /* 0xC */ u32 unkC; // CDROM.FID size
} Unk0030DA88;

typedef struct {
    /* 0x00 */ u8 pad0[0x1C];
    /* 0x1C */ s32 unk1C;
} Unk0057C71C;

void FlushCache(s32);
void func_00125B40(void);
sceCdRMode* func_0015B320();
/*?*/ s32 func_001653E8(s32, /*?*/ s32, /*?*/ s32);
void func_001656E0(const char* str, u32 len);
s32 func_00165EB8(void);
void func_00166798(char*, void*, s32, s32);
void func_00166878();

/* Hand-written: moves are `addu rd,$0,rs` (compiled code here uses `daddu rd,rs,$0`), it calls
 * through $1 (gcc never allocates `at`), has no frame, and switches sp to a caller-supplied buffer.
 * Saves ra/sp/s0 at stack[0..2], runs fn(arg) with sp = stack + size - 0x10, then restores them;
 * v0 from fn passes through. */
void func_00171510(void* stack, u32 size, void* fn, void* arg); // calls a function on the given stack
/* Hand-written (`addu rd,$0,rs` moves, $1 as scratch, no frame). Patches printf to jump to fn:
 * copies printf's first two instructions to func_001715A0, which then jumps to printf+8 (so it
 * acts as the original printf), and overwrites printf's first two with `j fn; nop`.
 * The caller must flush the caches afterwards. */
void func_00171548(void* fn); // redirects printf to fn
s32 func_001715A0(const char* fmt, ...);

void func_002D1C70(void (*)());
/*?*/ s32 func_0020D378();
// TK frame wait: blocks until at least max(n, 1) VBlanks (INTC 2 handler func_002D3B08 counts
// them in D_0078AC00) have passed since the last call, then resets the count and runs the
// per-frame TK updates.
/*?*/ void func_002D3C80(s32);
void func_002D8E20(const char* ch, const char* str, u32 len);

extern char D_0056B950[];                              // "FFXDEBUG"
extern char D_00595BF0[DBG_LINE_COUNT][DBG_LINE_SIZE]; // ring buffer
extern char D_00596FF0[];

extern s32 D_0030DA80;
extern s32 D_0030DA84;
extern Unk0030DA88 D_0030DA88;
extern sceCdRMode D_0030DA98;
extern sceCdRMode D_0030DAA0;

extern s8 D_0030DAA4;
extern s8 D_0030DAA5;
extern s32 D_0030DAA8;
extern s32 D_0030DAAC; // lsn
extern s32 D_0030DAB0; // Entry count
extern char D_0030DAB8[];
extern s8 D_0030DAC1;
extern s32 D_0030DAC4;
extern s32 D_0030DAC8; // ring buffer index
extern s8 D_0030DACC;  // isDirty log msg pending flag
extern s8 D_0030DACD;
extern s8 D_0030DACE; // 1: print only lines starting with the filter D_00596FF0; 0: only lines that don't
extern s8 D_0030DACF;

extern char D_0056B720[]; // "libmscd.c"
extern char D_0056B730[]; // "PcFileTblSet  DVD MODE ONLY"
extern char D_0056B750[]; // "PCload  DVD MODE ONLY"
extern char D_0056B768[]; // "pfs0:/ex/%s"
extern char D_0056B940[]; // "host:printf.log"
extern Unk0057C71C* D_0057C71C;
extern u8 D_00591AD0[0x4000];
extern char* D_00595AD0;
extern s32 D_00595AD4;        // sectorNum
extern s32 D_00595AD8;        // size
extern void* D_00595ADC;      // buffer
extern sceCdRMode D_00595AE0; // mode
extern Unk00595AEC* D_00595AEC; // D_0030DAB0 entries, 512 allocated
extern char D_00595AF0[];

void func_00162CB8(s8 arg0) { D_0030DAA5 = arg0; }

void func_00162CC8(void) {
    D_0030DAB0 = 0;
    D_00595AEC = (Unk00595AEC*)func_0011D370(512 * sizeof(Unk00595AEC), 10, D_0056B720, 0x203);
    memset(D_00595AEC, 0, 512 * sizeof(Unk00595AEC));
}

void func_00162D18(const char* path) { dbgAssert(D_0056B730, D_0056B720, 0x20C); }

void func_00162D30(const char* path) { dbgAssert(D_0056B750, D_0056B720, 0x211); }

char* func_00162D48(void) { return D_00595AD0; }

void func_00162D58(void) {
    switch ((s8)D_0030DAB4) {
    case 1:
        func_00163768(D_0057C700);
        D_0030DAB4 = 0;
        break;
    case 3:
        func_00166878();
        D_0030DAB4 = 0;
        break;
    }
}

void func_00162DC0(void) { func_002D1C70(func_00162D58); }

ArchiveState* func_00162DD0(void) { return &D_0078DF98; }

void func_00162DE0(s32 arg0) {
    if (D_0078DF98.unk18 == 0) {
        *D_0078EE30[arg0] = 0;
    }

    D_0078E030[arg0].unk8 = 0;
    D_0078E030[arg0].unk4.p = NULL;
    D_0078E030[arg0].unk0 = 0;
    D_0078E030[arg0].unk30 = 0;
    D_0078E030[arg0].unk31 = 0;
    D_0078E030[arg0].unk24 = 0;
    D_0078E030[arg0].unk32 = 0;
    D_0078E030[arg0].unk33 = 0;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162E48);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162EB8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00162FA0);

#define TBL_ENTRY_SIZE_MASK 0x3FFFFF
#define TBL_ENTRY_COMPRESSED_FLAG 0x400000
#define TBL_ENTRY_EMPTY_FLAG 0x800000

s32 func_00163090(s32 id) {
    s32 index = D_0078DF98.unk20 + id;
    u32 entry = D_0078DF98.unk28[index];
    s32 size = 0;

    if (!(entry & TBL_ENTRY_EMPTY_FLAG)) {
        u32 curr = entry & TBL_ENTRY_SIZE_MASK;
        u32 next = D_0078DF98.unk28[index + 1] & TBL_ENTRY_SIZE_MASK;
        size = ((next - curr) * 2048) - ((entry >> 24) * 8);
    }
    return size;
}

s32 func_001630F0(s32 id) {
    u32 entry = D_0078DF98.unk28[D_0078DF98.unk20 + id];
    if (!(entry & TBL_ENTRY_EMPTY_FLAG)) {
        if (entry & TBL_ENTRY_COMPRESSED_FLAG) {
            return 1;
        }
    }
    return 0;
}

s32 func_00163138(s32 arg0) { return (D_0078DF98.unk28[arg0] & TBL_ENTRY_SIZE_MASK) + D_0030DAAC; }

u32 func_00163168(s32 arg0, s32 arg1) { return func_00163138(D_0078DF98.unk2C[arg0] + arg1); }

s32 func_00163198(s32 arg0) { return func_00163138(D_0078DF98.unk20 + arg0); }

void func_001631C0(s8 arg0) { D_0030DAA4 = arg0; }

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

s32 func_00163290(void) {
    if (D_0030DAA4 != 0) {
        return func_0020D378();
    }
    // no return on this path: a void function or a return value here compiles to a tail jump
}

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

void func_00163938(Unk0078E030* arg0) { func_00171510(D_00591AD0, 0x4000, arg0->unk20, arg0); }

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
        func_00166798(D_00595AF0, entry->unk4.p, exFile->unk10, exFile->unkC);
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
            func_00163450(entry->unk1C, size, entry->unk4.p, &D_0030DA98, entry->unk28);
            FlushCache(0);

            if (*(char*)entry->unk4.p == 0) {
                memcpy(entry->unk4.p, (char*)entry->unk4.p + 5, secs * 2048);
                size -= secs * 2048;
                func_00163450(entry->unk1C + secs, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
                FlushCache(0);
                memcpy((char*)entry->unk4.p + secs * 2048, D_00595AD0, size - 5);
                goto end;
            }

            if (size & 0x7FF) {
                func_00163450(entry->unk1C + size / 2048, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
                rest = size & 0x7FF;
                FlushCache(0);
                memcpy((char*)entry->unk4.p + size - rest, D_00595AD0, rest);
            } else {
                FlushCache(0);
            }
        } else {
            func_00163450(entry->unk1C, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
            FlushCache(0);

            if (*D_00595AD0 == 0) {
                memcpy(entry->unk4.p, D_00595AD0 + 5, size - 5);
                goto end;
            }

            rest = size & 0x7FF;
            FlushCache(0);
            memcpy((char*)entry->unk4.p + size - rest, D_00595AD0, rest);
        }
    } else {
        if (size / 2048 != 0) {
            func_00163450(entry->unk1C, entry->unk2C, entry->unk4.p, &D_0030DA98, entry->unk28);
            if (size & 0x7FF) {
                func_00163450(entry->unk1C + size / 2048, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
                rest = size & 0x7FF;
                FlushCache(0);
                memcpy((char*)entry->unk4.p + size - rest, D_00595AD0, rest);
            } else {
                FlushCache(0);
            }
        } else {
            func_00163450(entry->unk1C, 0x800, D_00595AD0, &D_0030DA98, entry->unk28);
            rest = size & 0x7FF;
            FlushCache(0);
            memcpy((char*)entry->unk4.p + size - rest, D_00595AD0, rest);
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

const char* func_00164A10(void) { return D_0030DAB8; }

void func_00164A20(void) {
    while (1) {
        D_0078DF98.unk68();
        func_0017133C(1);
    }
}

void func_00164A50(s8 arg0) { D_0030DAC1 = arg0; }

void func_00164A60(s32 arg0) { D_0030DAC4 = arg0; }

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164A70);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165300);

void func_00165378(s32 arg0) {
    switch (arg0) {
    default:
    case 0:
        D_0030DA98.spindlctrl = SCECdSpinNom;
        break;
    case 1:
        D_0030DA98.spindlctrl = SCECdSpinMax;
        break;
    }
}

void func_001653A0(s32 arg0) {
    switch (arg0) {
    default:
    case 0:
        func_0015B320()->spindlctrl = SCECdSpinNom;
        break;
    case 1:
        func_0015B320()->spindlctrl = SCECdSpinMax;
        break;
    }
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_001653E8);

s32 func_00165528(s32 arg0) { return func_001653E8(arg0, 0, 0); }

s32 func_00165548(s32 arg0, s32 arg1) { return func_001653E8(func_00163168(arg0, arg1), arg0, arg1); }

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00165588);

/* Task object from sa_task.c (D_00595AE4, read_task_buff); stub, only slot 0 so far.
 * Known from its fill: +0x0/+0x4/+0x8 are per-group function slots, +0x8 starts a task. */
typedef struct {
    /* 0x00 */ void (*unk0)(void); // called with no args, then func_002D3C80(0) in the wait loop
} Unk00595AE4;

extern Unk00595AE4* D_00595AE4;

void func_001656B8(void) { D_00595AE4->unk0(); }

// Used by func_00165808, creates a new file the first call and appends thereafter
void func_001656E0(const char* str, u32 len) {
    s32 fd;

    if (D_0030DACC == 0) {
        fd = sceOpen(D_0056B940, SCE_TRUNC | SCE_WRONLY | SCE_CREAT);
        if (fd < 0) {
            return;
        }
        sceLseek(fd, 0, SCE_SEEK_SET);
    } else {
        fd = sceOpen(D_0056B940, SCE_TRUNC | SCE_WRONLY);
        if (fd < 0) {
            return;
        }
        sceLseek(fd, 0, SCE_SEEK_END);
    }

    sceWrite(fd, str, len);
    sceClose(fd);
}

void func_001657A8(const char* str, s8 filter) {
    s32 i;

    D_0030DACE = filter;
    if (str == NULL) {
        D_00596FF0[0] = '\0';
        return;
    }

    for (i = 0;; i++) {
        D_00596FF0[i] = str[i];
        if (str[i] == '\0') {
            break;
        }
    }
}

void func_00165808(const char* fmt, va_list ap) {
    char text[0x200];
    char* line;
    u32 len;
    s32 match;
    s32 i;

    vsprintf(text, fmt, ap);
    line = D_00595BF0[D_0030DAC8];
    strncpy(line, text, sizeof(D_00595BF0[0]) - 1);
    line[sizeof(D_00595BF0[0]) - 1] = '\0';
    D_0030DAC8 = (D_0030DAC8 + 1) & (DBG_LINE_COUNT - 1);

    if (func_002D93E8() == 1) {
        if (D_00596FF0[0] == '\0') {
            func_001715A0(text);
        } else {
            match = 0;
            for (i = 0; line[i] != 0; i++) {
                if (D_00596FF0[i] == '\0') {
                    match = 1;
                    break;
                }
                if (line[i] != D_00596FF0[i]) {
                    break;
                }
            }
            if (D_0030DACE == match) {
                func_001715A0(text);
            }
        }
    } else if (func_001244D0()->unkB != 0) {
        if (D_00596FF0[0] == '\0') {
            func_001715A0(text);
        } else {
            match = 0;
            for (i = 0; line[i] != 0; i++) {
                if (D_00596FF0[i] == '\0') {
                    match = 1;
                    break;
                }
                if (line[i] != D_00596FF0[i]) {
                    break;
                }
            }
            if (D_0030DACE == match) {
                func_001715A0(text);
            }
        }
    }

    switch (D_0030DACD) {
    case DBG_OUT_STREAM_FFXDEBUG:
        len = strlen(line);
        if (len != 0) {
            func_002D8E20(D_0056B950, line, len);
        }
        break;
    case DBG_OUT_STREAM_FILE:
        if (func_002D93E8() == 1) {
            len = strlen(line);
            if (len != 0) {
                func_001656E0(line, len);
            }
        }
        break;
    }
    D_0030DACC = 1;
}

void func_00165A18(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    func_00165808(fmt, ap);
}

void func_00165A50(s32 mode) {
    if (mode != DBG_OUT_NONE) {
        if (D_0030DACF == 0) {
            memset(D_00595BF0, 0, DBG_LINE_SIZE * DBG_LINE_COUNT);

            D_00596FF0[0] = '\0';
            D_0030DACE = 0;
            D_0030DACF = 1;

            func_00171548(func_00165A18);
            FlushCache(0);
            FlushCache(2);
        }
    }
    D_0030DACD = mode;
}

void func_00165AE0(void) {}

// Silences printf: redirects it to the empty func_00165AE0, then writes back the data cache (0)
// and invalidates the instruction cache (2) so the patched code is fetched.
void func_00165AE8(void) {
    func_00171548(func_00165AE0);
    FlushCache(0);
    FlushCache(2);
}

void func_00165B18(void) {
    char* buffer;
    char* cursor;
    char* line;
    s32 fd;
    s32 index;
    s32 len;
    s32 i;

    if (func_002D93E8() != 1 || D_0030DACC == 0) {
        return;
    }

    switch (D_0030DACD) {
    case DBG_OUT_NONE:
        break;

    case DBG_OUT_LOG_FILE:
        index = D_0030DAC8;
        fd = sceOpen(D_0056B940, SCE_TRUNC | SCE_WRONLY | SCE_CREAT);
        if (fd < 0) {
            return;
        }

        for (i = 0; i < DBG_LINE_COUNT; i++) {
            line = D_00595BF0[index];
            len = strlen(line);
            if (len != 0) {
                sceWrite(fd, line, len);
            }
            index++;
            index &= DBG_LINE_COUNT - 1;
        }

        sceClose(fd);
        break;

    case DBG_OUT_LOG_MEMORY:
        index = D_0030DAC8;
        len = 0;
        for (i = 0; i < DBG_LINE_COUNT; i++) {
            len += strlen(D_00595BF0[index]);
            index++;
            index &= DBG_LINE_COUNT - 1;
        }

        if (len != 0) {
            buffer = func_0011D370(len, 10, D_0056B720, 0x928);

            index = D_0030DAC8;
            cursor = buffer;
            for (i = 0; i < DBG_LINE_COUNT; i++) {
                line = D_00595BF0[index];
                len = strlen(line);
                if (len != 0) {
                    strncpy(cursor, line, len);
                    cursor += len;
                }
                index++;
                index &= DBG_LINE_COUNT - 1;
            }

            func_0011D590(buffer, 10);
        }
        break;
    }
    D_0030DAC8 = 0;
}

Unk00595AEC* func_00165D00(s32 arg0, s32 arg1) {
    Unk00595AEC* entry;
    s32 i;

    if (D_0030DAB0 == 0 || arg0 == -1 || arg1 == -1) {
        return NULL;
    }

    entry = D_00595AEC;
    for (i = 0; i < D_0030DAB0; i++) {
        if (entry->unk0 == arg0 && entry->unk8 == arg1) {
            return entry;
        }
        entry++;
    }
    return NULL;
}

s32 func_00165D70(s32 arg0, s32 arg1) {
    Unk00595AEC* entry;
    s32 i;

    if (D_0030DAB0 == 0) {
        return 0;
    }

    entry = D_00595AEC;
    for (i = 0; i < D_0030DAB0; i++) {
        if (entry->unk0 == arg0 && entry->unk8 == arg1) {
            return entry->unk10;
        }
        entry++;
    }
    return 0;
}

void func_00165DD0(const char** arg0) {
    sceCdlFILE fp;
    if (arg0 != NULL) {
        while (sceCdSearchFile(&fp, arg0[0]) == 0) {
            func_002D3C80(1);
        }
        D_0030DAAC = fp.lsn;

        while (sceCdSearchFile(&fp, arg0[1]) == 0) {
            func_002D3C80(1);
        }
        D_0030DA88.unk0 = fp.lsn;
        D_0030DA88.unk4 = fp.size;

        while (sceCdSearchFile(&fp, arg0[2]) == 0) {
            func_002D3C80(1);
        }
        D_0030DA88.unk8 = fp.lsn;
        D_0030DA88.unkC = fp.size;
    }
}
