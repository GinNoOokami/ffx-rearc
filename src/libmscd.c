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

#define TBL_ENTRY_SIZE_MASK 0x3FFFFF
#define TBL_ENTRY_COMPRESSED_FLAG 0x400000
#define TBL_ENTRY_EMPTY_FLAG 0x800000

typedef enum {
    DBG_OUT_NONE,
    DBG_OUT_LOG_FILE,
    DBG_OUT_LOG_MEMORY,
    DBG_OUT_STREAM_FFXDEBUG,
    DBG_OUT_STREAM_FILE
} DebugOutLogMode;

typedef struct {
    /* 0x0 */ u32 file_tbl_lsn;   // CDROM.MDG LSN
    /* 0x4 */ u32 file_tbl_size;  // CDROM.MDG size
    /* 0x8 */ u32 group_tbl_lsn;  // CDROM.FID LSN
    /* 0xC */ u32 group_tbl_size; // CDROM.FID size
} Unk0030DA88;

typedef struct {
    /* 0x00 */ u8 pad0[0x1C];
    /* 0x1C */ s32 unk1C;
} Unk0057C71C;

/* Task object from sa_task.c (read_task_buff): a buffer allocated and set up by
 * func_00171C78(1) in func_00164A70; stub, only slot 0 so far.
 * Known from its fill: +0x0/+0x4/+0x8 are per-group function slots, +0x8 starts a task. */
typedef struct {
    /* 0x00 */ void (*unk0)(void); // called with no args, then func_002D3C80(0) in the wait loop
} SaTask;

extern SaTask* read_task_buff;

void FlushCache(s32);
void func_00125B40(void);
sceCdRMode* func_0015B320();
void func_0015B330(void);
s32 func_0015B830(void);
s32 func_001636F0(void);
void func_00163E90();
s32 func_00164040();
s32 func_00164320();
s32 func_00164628();
/*?*/ s32 func_001653E8(s32, /*?*/ s32, /*?*/ s32);
s32 func_00165EB8(void);
s32 func_00165F30(s32);
void func_00166650(s32, void*, s32, s32);
void func_00166798(char*, void*, s32, s32);
void func_00166878(void);

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
void func_002558B0();
// TK frame wait: blocks until at least max(n, 1) VBlanks (INTC 2 handler func_002D3B08 counts
// them in D_0078AC00) have passed since the last call, then resets the count and runs the
// per-frame TK updates.
/*?*/ void func_002D3C80(s32);
s32 func_002D6418(void);
s32 func_002D6478(void);
void func_002D8E20(const char* ch, const char* str, u32 len);

extern s32 D_0030DA78;
extern s8 D_0030DA7C;
extern s8 sa_file_read_init;
extern s32 D_0030DA80;
extern s32 D_0030DA84;
extern Unk0030DA88 demo_desc;
extern sceCdRMode D_0030DA98;
extern sceCdRMode D_0030DAA0;

extern s8 D_0030DAA4;
extern s8 D_0030DAA5;
extern s32 D_0030DAA8;
extern s32 lsn_base;
extern s32 ex_file_cnt;
extern char D_0030DAB8[];
extern s8 D_0030DAC1;
extern s32 D_0030DAC4;
extern s32 dbg_line_index; // ring buffer index
extern s8 dbg_pending;     // dirty flag
extern s8 dbg_out_mode;
extern s8 dbg_filter_mode; // 1: print only lines starting with dbg_filter; 0: only lines that don't
extern s8 D_0030DACF;
extern char* D_0030DCD8;

// rodata string literals
extern char D_0056B720[]; // "libmscd.c"
extern char D_0056B730[]; // "PcFileTblSet  DVD MODE ONLY"
extern char D_0056B750[]; // "PCload  DVD MODE ONLY"
extern char D_0056B768[]; // "pfs0:/ex/%s"
extern char D_0056B778[]; // "sa_file_read_init!=0"
extern char D_0056B800[]; // "read_task_buff != NULL"
extern char D_0056B818[]; // "0"
extern char D_0056B940[]; // "host:printf.log"
extern char D_0056B950[]; // "FFXDEBUG"

extern Unk0057C71C* D_0057C71C;
extern u8 D_00591AD0[0x4000];
extern char* D_00595AD0;
extern s32 cd_read_lsn;
extern s32 cd_read_size;
extern void* cd_read_buff;
extern sceCdRMode cd_read_mode;
extern u8* size_tbl;
extern ExFile* ex_file_tbl; // 512 allocated
extern char D_00595AF0[];
extern char dbg_line_buff[DBG_LINE_COUNT][DBG_LINE_SIZE]; // ring buffer
extern char dbg_filter[];

/*?*/ s32 func_00171D08();
/*?*/ s32 func_00171D50();
/*?*/ s32 func_00171D98();
/*?*/ s32 func_00171DE0();
/*?*/ s32 func_00171E28();
/*?*/ s32 func_00171E70();
/*?*/ s32 func_00171EB8();
s32 func_001BD228();
u32 func_002D9460();

s32 func_001625F8(void) {
    if (func_001BD228() == 0) {
        switch (func_002D9460()) {
        case 2:
            return func_00171E70();
        case 3:
            return func_00171D08();
        case 4:
            return func_00171D50();
        case 5:
            return func_00171EB8();
        case 6:
            return func_00171DE0();
        default:
        case 0:
            return func_00171D98();
        case 1:
            return func_00171E28();
        }
    }
    return func_00171E28();
}

/*?*/ s32 func_00171CC8();
/*?*/ s32 func_00171D10();
/*?*/ s32 func_00171D58();
/*?*/ s32 func_00171DA0();
/*?*/ s32 func_00171DE8();
/*?*/ s32 func_00171E30();
/*?*/ s32 func_00171E78();

s32 func_001626B8(void) {
    if (func_001BD228() == 0) {
        switch (func_002D9460()) {
        case 2:
            return func_00171E30();
        case 3:
            return func_00171CC8();
        case 4:
            return func_00171D10();
        case 5:
            return func_00171E78();
        case 6:
            return func_00171DA0();
        default:
        case 0:
            return func_00171D58();
        case 1:
            return func_00171DE8();
        }
    }
    return func_00171DE8();
}

/*?*/ s32 func_00171CD0();
/*?*/ s32 func_00171D18();
/*?*/ s32 func_00171D60();
/*?*/ s32 func_00171DA8();
/*?*/ s32 func_00171DF0();
/*?*/ s32 func_00171E38();
/*?*/ s32 func_00171E80();

s32 func_00162778(void) {
    if (func_001BD228() == 0) {
        switch (func_002D9460()) {
        case 2:
            return func_00171E38();
        case 3:
            return func_00171CD0();
        case 4:
            return func_00171D18();
        case 5:
            return func_00171E80();
        case 6:
            return func_00171DA8();
        default:
        case 0:
            return func_00171D60();
        case 1:
            return func_00171DF0();
        }
    }
    return func_00171DF0();
}

/*?*/ s32 func_00171CD8();
/*?*/ s32 func_00171D20();
/*?*/ s32 func_00171D68();
/*?*/ s32 func_00171DB0();
/*?*/ s32 func_00171DF8();
/*?*/ s32 func_00171E40();
/*?*/ s32 func_00171E88();

s32 func_00162838(void) {
    if (func_001BD228() == 0) {
        switch (func_002D9460()) {
        case 2:
            return func_00171E40();
        case 3:
            return func_00171CD8();
        case 4:
            return func_00171D20();
        case 5:
            return func_00171E88();
        case 6:
            return func_00171DB0();
        default:
        case 0:
            return func_00171D68();
        case 1:
            return func_00171DF8();
        }
    }
    return func_00171DF8();
}

/*?*/ s32 func_00171CE0();
/*?*/ s32 func_00171D28();
/*?*/ s32 func_00171D70();
/*?*/ s32 func_00171DB8();
/*?*/ s32 func_00171E00();
/*?*/ s32 func_00171E48();
/*?*/ s32 func_00171E90();

s32 func_001628F8(void) {
    if (func_001BD228() == 0) {
        switch (func_002D9460()) {
        case 2:
            return func_00171E48();
            return;
        case 3:
            return func_00171CE0();
            return;
        case 4:
            return func_00171D28();
            return;
        case 5:
            return func_00171E90();
            return;
        case 6:
            return func_00171DB8();
            return;
        default:
        case 0:
            return func_00171D70();
        case 1:
            return func_00171E00();
        }
    }
    return func_00171E00();
}

/*?*/ s32 func_00171CE8(); /* extern */
/*?*/ s32 func_00171D30(); /* extern */
/*?*/ s32 func_00171D78(); /* extern */
/*?*/ s32 func_00171DC0(); /* extern */
/*?*/ s32 func_00171E08(); /* extern */
/*?*/ s32 func_00171E50(); /* extern */
/*?*/ s32 func_00171E98(); /* extern */

s32 func_001629B8(void) {
    if (func_001BD228() == 0) {
        switch (func_002D9460()) {
        case 2:
            return func_00171E50();
        case 3:
            return func_00171CE8();
        case 4:
            return func_00171D30();
        case 5:
            return func_00171E98();
        case 6:
            return func_00171DC0();
        default:
        case 0:
            return func_00171D78();
        case 1:
            return func_00171E08();
        }
    }
    return func_00171E08();
}

/*?*/ s32 func_00171CF0(); /* extern */
/*?*/ s32 func_00171D38(); /* extern */
/*?*/ s32 func_00171D80(); /* extern */
/*?*/ s32 func_00171DC8(); /* extern */
/*?*/ s32 func_00171E10(); /* extern */
/*?*/ s32 func_00171E58(); /* extern */
/*?*/ s32 func_00171EA0(); /* extern */

s32 func_00162A78(void) {
    if (func_001BD228() == 0) {
        switch (func_002D9460()) {
        case 2:
            return func_00171E58();
        case 3:
            return func_00171CF0();
        case 4:
            return func_00171D38();
        case 5:
            return func_00171EA0();
        case 6:
            return func_00171DC8();
        default:
        case 0:
            return func_00171D80();
        case 1:
            return func_00171E10();
        }
    }
    return func_00171E10();
}

/*?*/ s32 func_00171CF8(); /* extern */
/*?*/ s32 func_00171D40(); /* extern */
/*?*/ s32 func_00171D88(); /* extern */
/*?*/ s32 func_00171DD0(); /* extern */
/*?*/ s32 func_00171E18(); /* extern */
/*?*/ s32 func_00171E60(); /* extern */
/*?*/ s32 func_00171EA8(); /* extern */

s32 func_00162B38(void) {
    if (func_001BD228() == 0) {
        switch (func_002D9460()) {
        case 2:
            return func_00171E60();
        case 3:
            return func_00171CF8();
        case 4:
            return func_00171D40();
        case 5:
            return func_00171EA8();
        case 6:
            return func_00171DD0();
        default:
        case 0:
            return func_00171D88();
        case 1:
            return func_00171E18();
        }
    }
    return func_00171E18();
}

/*?*/ s32 func_00171D00(); /* extern */
/*?*/ s32 func_00171D48(); /* extern */
/*?*/ s32 func_00171D90(); /* extern */
/*?*/ s32 func_00171DD8(); /* extern */
/*?*/ s32 func_00171E20(); /* extern */
/*?*/ s32 func_00171E68(); /* extern */
/*?*/ s32 func_00171EB0(); /* extern */

s32 func_00162BF8(void) {
    if (func_001BD228() == 0) {
        switch (func_002D9460()) {
        case 2:
            return func_00171E68();
        case 3:
            return func_00171D00();
        case 4:
            return func_00171D48();
        case 5:
            return func_00171EB0();
        case 6:
            return func_00171DD8();
        default:
        case 0:
            return func_00171D90();
        case 1:
            return func_00171E20();
        }
    }
    return func_00171E20();
}

void func_00162CB8(s8 arg0) { D_0030DAA5 = arg0; }

void func_00162CC8(void) {
    ex_file_cnt = 0;
    ex_file_tbl = (ExFile*)func_0011D370(512 * sizeof(ExFile), 10, D_0056B720, 0x203);
    memset(ex_file_tbl, 0, 512 * sizeof(ExFile));
}

void func_00162D18(const char* path) { dbgAssert(D_0056B730, D_0056B720, 0x20C); }

void* func_00162D30(const char* path, s32* unused) { dbgAssert(D_0056B750, D_0056B720, 0x211); }

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

SaFile* SaFileGet(void) { return &sa_file; }

void func_00162DE0(s32 arg0) {
    if (sa_file.mode == 0) {
        *read_q_path[arg0] = 0;
    }

    read_q[arg0].no = 0;
    read_q[arg0].buff.p = NULL;
    read_q[arg0].size = 0;
    read_q[arg0].unk30 = 0;
    read_q[arg0].active = 0;
    read_q[arg0].req_no = 0;
    read_q[arg0].cancel = 0;
    read_q[arg0].compressed = 0;
}

void func_00162E48(void) {
    s32 i;

    for (i = 0; i < 64; i++) {
        func_00162DE0(i);
    }

    D_0030DA7C = 0;
    cd_read_lsn = 0;
    cd_read_size = 0;
    D_0030DA80 = 0;
    D_0030DA84 = 0;
    cd_read_buff = NULL;
    D_0030DA78 = 0;

    func_0015B330();
}

s32 FileSizeGet(s32 id) {
    s32 index = sa_file.id_base + id;
    s32 offset = index * 3;
    s32 size;
    u8* data;

    size = ExFileSizeGet(sa_file.group, id);
    if (size == 0) {
        u32 entry = sa_file.file_tbl[index];
        if (!(entry & TBL_ENTRY_EMPTY_FLAG)) {
            if (entry & TBL_ENTRY_COMPRESSED_FLAG) {
                data = size_tbl + offset;
                size = data[0];
                size += data[1] << 8;
                size += data[2] << 16;
                size *= 8;
            } else {
                u32 curr = entry & TBL_ENTRY_SIZE_MASK;
                u32 next = sa_file.file_tbl[index + 1] & TBL_ENTRY_SIZE_MASK;
                size = ((next - curr) * 2048) - ((entry >> 24) * 8);
            }
        }
        return size;
    }
    // No return when the override hook returns nonzero: its result is still in v0
}

s32 GroupFileSizeGet(s32 arg0, s32 id) {
    s32 index = sa_file.group_tbl[arg0] + id;
    s32 offset = index * 3;
    s32 size;
    u8* data;

    size = ExFileSizeGet(arg0, id);
    if (size == 0) {
        u32 entry = sa_file.file_tbl[index];
        if (!(entry & TBL_ENTRY_EMPTY_FLAG)) {
            if (entry & TBL_ENTRY_COMPRESSED_FLAG) {
                data = size_tbl + offset;
                size = data[0];
                size += data[1] << 8;
                size += data[2] << 16;
                size *= 8;
            } else {
                u32 curr = entry & TBL_ENTRY_SIZE_MASK;
                u32 next = sa_file.file_tbl[index + 1] & TBL_ENTRY_SIZE_MASK;
                size = ((next - curr) * 2048) - ((entry >> 24) * 8);
            }
        }
        return size;
    }
    // No return when the override hook returns nonzero: its result is still in v0
}

s32 FileDiscSizeGet(s32 id) {
    s32 index = sa_file.id_base + id;
    u32 entry = sa_file.file_tbl[index];
    s32 size = 0;

    if (!(entry & TBL_ENTRY_EMPTY_FLAG)) {
        u32 curr = entry & TBL_ENTRY_SIZE_MASK;
        u32 next = sa_file.file_tbl[index + 1] & TBL_ENTRY_SIZE_MASK;
        size = ((next - curr) * 2048) - ((entry >> 24) * 8);
    }
    return size;
}

s32 FileCompressedGet(s32 id) {
    u32 entry = sa_file.file_tbl[sa_file.id_base + id];
    if (!(entry & TBL_ENTRY_EMPTY_FLAG)) {
        if (entry & TBL_ENTRY_COMPRESSED_FLAG) {
            return 1;
        }
    }
    return 0;
}

s32 TblLsnGet(s32 arg0) { return (sa_file.file_tbl[arg0] & TBL_ENTRY_SIZE_MASK) + lsn_base; }

u32 GroupFileLsnGet(s32 arg0, s32 arg1) { return TblLsnGet(sa_file.group_tbl[arg0] + arg1); }

s32 FileLsnGet(s32 arg0) { return TblLsnGet(sa_file.id_base + arg0); }

void func_001631C0(s8 arg0) { D_0030DAA4 = arg0; }

s32 func_001631D0(s32 arg0, s32 size, void* buffer, sceCdRMode* mode) {
    s32 sectors;
    D_0030DAA4 = 0;
    D_0030DAA8 = 0;

    cd_read_lsn = arg0;
    cd_read_size = size;
    cd_read_buff = buffer;
    cd_read_mode = *mode;

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

                lsn = GroupFileLsnGet(1, 1);
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

s32 func_00163450(s32 lsn, s32 size, void* buffer, sceCdRMode* mode, s32 group) {
    s32 sectors;
    s32 found;
    s32 retry;
    s32 tries;
    s32 wait;
    s32 i;

    while (func_002D6418() != 0) {
        func_0017133C(1);
    }

    cd_read_lsn = lsn;
    cd_read_size = size;
    cd_read_buff = buffer;
    cd_read_mode = *mode;
    sectors = size / 2048;
    found = func_00165F30(group);

    if (D_00565DF4 != 0 && found != 0) {
        func_0015B330();
        func_00166650(lsn, buffer, sectors * 2048, group);
        if (func_00165EB8() == 0) {
            return 1;
        }

        D_00565DF4 = 0;
        func_00125B40();
    }

    D_0030DAA8 = 0;
    D_0030DAA4 = 0;
    retry = 0;
    tries = 60;
    wait = 450;

    while (1) {
        for (i = 0; i < tries; i++) {
            if (sceCdDiskReady(1) == SCECdComplete) {
                func_0015B330();
                FlushCache(0);

                if (sceCdRead(lsn, sectors, buffer, mode) == 1) {
                    for (i = 0; i < wait; i++) {
                        if (func_001636F0() == 0) {
                            return 1;
                        }
                        func_0017133C(1);
                    }
                    break;
                }
            }
            func_001632B8();
            D_0030DAA8++;
            D_0030DAA4 = 2;
            func_0017133C(1);
        }

        sceCdBreak();
        D_0030DAA4 = 2;
        func_0017133C(1);

        if (retry > 2) {
            wait = 600;
            tries = 120;

            for (i = 0; i < 60; i++) {
                if (sceCdStandby() != 0) {
                    break;
                }
                func_0017133C(1);
            }

            for (i = 0; i < 120; i++) {
                if (sceCdSync(1) == 0) {
                    break;
                }
                func_0017133C(1);
            }

            mode = &D_0030DAA0;
            continue;
        }
        retry++;
    }
}

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
    func_001631D0(cd_read_lsn, cd_read_size, cd_read_buff, &D_0030DAA0);
    return 1;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163768);

void ReadQCallbackCall(ReadQEntry* arg0) { func_00171510(D_00591AD0, 0x4000, arg0->callback, arg0); }

// Not matching yet. For compressed entries (entry->compressed, entry->size < size) the function reads the first
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
void func_00163950(SaFile* archive, ReadQEntry* entry, s32 index) {
    ExFile* exFile;
    s32 start;
    s32 size;
    s32 secs;
    s32 rest;

    if (archive->busy != 0) {
        return;
    }

    archive->busy_index = index;
    archive->busy = 1;
    archive->host_fd = 0;
    start = D_0057C71C->unk1C;
    size = entry->disc_size;
    exFile = ExFileFind(entry->group, entry->no);

    if (exFile != NULL) {
        sprintf(D_00595AF0, D_0056B768, exFile->name);
        func_00166798(D_00595AF0, entry->buff.p, exFile->size, exFile->offset);
        if (func_00165EB8() == 0) {
            goto end;
        }
        D_00565DF4 = 0;
        func_00125B40();
    }

    if (entry->compressed != 0 && entry->size < size) {
        size = entry->size + 5;
        secs = size / 2048;
        if (secs != 0 && size > 0) {
            func_00163450(entry->lsn, size, entry->buff.p, &D_0030DA98, entry->group);
            FlushCache(0);

            if (*(char*)entry->buff.p == 0) {
                memcpy(entry->buff.p, (char*)entry->buff.p + 5, secs * 2048);
                size -= secs * 2048;
                func_00163450(entry->lsn + secs, 0x800, D_00595AD0, &D_0030DA98, entry->group);
                FlushCache(0);
                memcpy((char*)entry->buff.p + secs * 2048, D_00595AD0, size - 5);
                goto end;
            }

            if (size & 0x7FF) {
                func_00163450(entry->lsn + size / 2048, 0x800, D_00595AD0, &D_0030DA98, entry->group);
                rest = size & 0x7FF;
                FlushCache(0);
                memcpy((char*)entry->buff.p + size - rest, D_00595AD0, rest);
            } else {
                FlushCache(0);
            }
        } else {
            func_00163450(entry->lsn, 0x800, D_00595AD0, &D_0030DA98, entry->group);
            FlushCache(0);

            if (*D_00595AD0 == 0) {
                memcpy(entry->buff.p, D_00595AD0 + 5, size - 5);
                goto end;
            }

            rest = size & 0x7FF;
            FlushCache(0);
            memcpy((char*)entry->buff.p + size - rest, D_00595AD0, rest);
        }
    } else {
        if (size / 2048 != 0) {
            func_00163450(entry->lsn, entry->disc_size, entry->buff.p, &D_0030DA98, entry->group);
            if (size & 0x7FF) {
                func_00163450(entry->lsn + size / 2048, 0x800, D_00595AD0, &D_0030DA98, entry->group);
                rest = size & 0x7FF;
                FlushCache(0);
                memcpy((char*)entry->buff.p + size - rest, D_00595AD0, rest);
            } else {
                FlushCache(0);
            }
        } else {
            func_00163450(entry->lsn, 0x800, D_00595AD0, &D_0030DA98, entry->group);
            rest = size & 0x7FF;
            FlushCache(0);
            memcpy((char*)entry->buff.p + size - rest, D_00595AD0, rest);
        }
    }

    if (entry->compressed != 0) {
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
    D_0030DA84 = cd_read_size;
    D_0030DA80 = D_0057C71C->unk1C - start;
    if (entry->callback != NULL) {
        ReadQCallbackCall(entry);
    }

    archive->busy = 0;
    archive->host_fd = -1;
    archive->busy_index = -1;
    entry->active = 0;
    entry->req_no = 0;
}
#endif

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163D20);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00163E90);

s16 func_00163FD0(s32 arg0) {
    s16 fid;

    if (sa_file_read_init == 0) {
        dbgAssert(D_0056B778, D_0056B720, 0x534);
    }

    fid = sa_file.group_tbl[arg0];
    sa_file.group = arg0;
    sa_file.id_base = fid;
    if (fid < 0) {
        sa_file.id_base = 0;
        fid = -1;
    }
    return fid;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164040);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164320);

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164628);

void ReadQWait(void) {
    char* msg;
    s32 i;

    if (read_task_buff == NULL) {
        dbgAssert(D_0056B800, D_0056B720, 0x63B);
    }

    // Waits until no queue entry is active: the task step and frame wait sit in the for's
    // increment, so they run only between searches (the target jumps straight to the first search)
    for (;; read_task_buff->unk0(), func_002D3C80(0)) {
        for (i = 0; i < READ_Q_CNT; i++) {
            if (read_q[i].active != 0) {
                if (func_002D6478() != 0) {
                    dbgAssert(D_0056B818, D_0056B720, 0x642);
                }
                break;
            }
        }

        if (i == READ_Q_CNT) {
            break;
        }
    }
}

s32 ReadQFileStatGet(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 0; i < READ_Q_CNT; i++) {
        if (read_q[i].active != 0 && read_q[i].group == arg0 && read_q[i].no == arg1) {
            if (sa_file.busy != 0 && sa_file.busy_index == i) {
                return 2;
            }
            return 1;
        }
    }

    return 0;
}

s32 ReadQSync(s32 arg0) {
    s32 i;

    switch (arg0) {
    default:
    case 0:
        ReadQWait();
        break;

    case 1:
        for (i = 0; i < READ_Q_CNT; i++) {
            if (read_q[i].active != 0) {
                if (func_002D6478() != 0) {
                    dbgAssert(D_0056B818, D_0056B720, 0x67E);
                }
                return 1;
            }
        }
        break;
    }

    return 0;
}

const char* func_00164A10(s32 unused) { return D_0030DAB8; }

void ReadTask(void) {
    while (1) {
        sa_file.unk68();
        func_0017133C(1);
    }
}

void func_00164A50(s8 arg0) { D_0030DAC1 = arg0; }

void func_00164A60(s32 arg0) { D_0030DAC4 = arg0; }

INCLUDE_ASM("asm/jp_intl/nonmatchings/libmscd", func_00164A70);

void ReadQCancel(s32 arg0) {
    s32 i;

    if (arg0 <= 0) {
        for (i = 0; i < READ_Q_CNT; i++) {
            if (read_q[i].active != 0) {
                read_q[i].cancel = 1;
            }
        }
    } else {
        for (i = 0; i < READ_Q_CNT; i++) {
            if (read_q[i].active != 0 && read_q[i].req_no == arg0) {
                read_q[i].cancel = 1;
            }
        }
    }
}

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

s32 func_00165548(s32 arg0, s32 arg1) { return func_001653E8(GroupFileLsnGet(arg0, arg1), arg0, arg1); }

void func_00165588(s32 mode) {
    sa_file.mode = mode;

    if (mode == 0) {
        func_002558B0();
    } else {
        sa_file.unk14 = FileSizeGet;
        sa_file.unk58 = func_00164A10;
        sa_file.unk68 = func_00163E90;
        sa_file.unk54 = func_00162D30;
        sa_file.unk60 = func_00162D18;
        sa_file.unk4 = FileLsnGet;
        sa_file.unk78 = func_00164628;
        sa_file.unk88 = func_00165548;
        sa_file.unk8C = func_00165528;
    }

    sa_file.unk8 = ReadQSync;
    sa_file.unk1C = ReadQCancel;
    sa_file.unk0 = func_00164040;
    sa_file.unk90 = func_00164320;
    sa_file.unk64 = func_00162E48;
    sa_file.unkC = func_00165588;
    sa_file.unk10 = func_00163FD0;
    sa_file.unk7C = func_00165378;
    sa_file.unk84 = func_001653A0;
    sa_file.unk80 = ReadQFileStatGet;
    sa_file.unk6C = GroupFileLsnGet;
}

void func_001656B8(void) { read_task_buff->unk0(); }

// Used by func_00165808, creates a new file the first call and appends thereafter
void func_001656E0(const char* str, u32 len) {
    s32 fd;

    if (dbg_pending == 0) {
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

void func_001657A8(const char* str, s8 filter_mode) {
    s32 i;

    dbg_filter_mode = filter_mode;
    if (str == NULL) {
        dbg_filter[0] = '\0';
        return;
    }

    for (i = 0;; i++) {
        dbg_filter[i] = str[i];
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
    line = dbg_line_buff[dbg_line_index];
    strncpy(line, text, sizeof(dbg_line_buff[0]) - 1);
    line[sizeof(dbg_line_buff[0]) - 1] = '\0';
    dbg_line_index = (dbg_line_index + 1) & (DBG_LINE_COUNT - 1);

    if (func_002D93E8() == 1) {
        if (dbg_filter[0] == '\0') {
            func_001715A0(text);
        } else {
            match = 0;
            for (i = 0; line[i] != 0; i++) {
                if (dbg_filter[i] == '\0') {
                    match = 1;
                    break;
                }
                if (line[i] != dbg_filter[i]) {
                    break;
                }
            }
            if (dbg_filter_mode == match) {
                func_001715A0(text);
            }
        }
    } else if (func_001244D0()->unkB != 0) {
        if (dbg_filter[0] == '\0') {
            func_001715A0(text);
        } else {
            match = 0;
            for (i = 0; line[i] != 0; i++) {
                if (dbg_filter[i] == '\0') {
                    match = 1;
                    break;
                }
                if (line[i] != dbg_filter[i]) {
                    break;
                }
            }
            if (dbg_filter_mode == match) {
                func_001715A0(text);
            }
        }
    }

    switch (dbg_out_mode) {
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
    dbg_pending = 1;
}

void func_00165A18(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    func_00165808(fmt, ap);
}

void func_00165A50(s32 mode) {
    if (mode != DBG_OUT_NONE) {
        if (D_0030DACF == 0) {
            memset(dbg_line_buff, 0, DBG_LINE_SIZE * DBG_LINE_COUNT);

            dbg_filter[0] = '\0';
            dbg_filter_mode = 0;
            D_0030DACF = 1;

            func_00171548(func_00165A18);
            FlushCache(0);
            FlushCache(2);
        }
    }
    dbg_out_mode = mode;
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

    if (func_002D93E8() != 1 || dbg_pending == 0) {
        return;
    }

    switch (dbg_out_mode) {
    case DBG_OUT_NONE:
        break;

    case DBG_OUT_LOG_FILE:
        index = dbg_line_index;
        fd = sceOpen(D_0056B940, SCE_TRUNC | SCE_WRONLY | SCE_CREAT);
        if (fd < 0) {
            return;
        }

        for (i = 0; i < DBG_LINE_COUNT; i++) {
            line = dbg_line_buff[index];
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
        index = dbg_line_index;
        len = 0;
        for (i = 0; i < DBG_LINE_COUNT; i++) {
            len += strlen(dbg_line_buff[index]);
            index++;
            index &= DBG_LINE_COUNT - 1;
        }

        if (len != 0) {
            buffer = func_0011D370(len, 10, D_0056B720, 0x928);

            index = dbg_line_index;
            cursor = buffer;
            for (i = 0; i < DBG_LINE_COUNT; i++) {
                line = dbg_line_buff[index];
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
    dbg_line_index = 0;
}

ExFile* ExFileFind(s32 arg0, s32 arg1) {
    ExFile* entry;
    s32 i;

    if (ex_file_cnt == 0 || arg0 == -1 || arg1 == -1) {
        return NULL;
    }

    entry = ex_file_tbl;
    for (i = 0; i < ex_file_cnt; i++) {
        if (entry->group == arg0 && entry->no == arg1) {
            return entry;
        }
        entry++;
    }
    return NULL;
}

s32 ExFileSizeGet(s32 arg0, s32 arg1) {
    ExFile* entry;
    s32 i;

    if (ex_file_cnt == 0) {
        return 0;
    }

    entry = ex_file_tbl;
    for (i = 0; i < ex_file_cnt; i++) {
        if (entry->group == arg0 && entry->no == arg1) {
            return entry->size;
        }
        entry++;
    }
    return 0;
}

void DemoDescSet(const char** arg0) {
    sceCdlFILE fp;
    if (arg0 != NULL) {
        while (sceCdSearchFile(&fp, arg0[0]) == 0) {
            func_002D3C80(1);
        }
        lsn_base = fp.lsn;

        while (sceCdSearchFile(&fp, arg0[1]) == 0) {
            func_002D3C80(1);
        }
        demo_desc.file_tbl_lsn = fp.lsn;
        demo_desc.file_tbl_size = fp.size;

        while (sceCdSearchFile(&fp, arg0[2]) == 0) {
            func_002D3C80(1);
        }
        demo_desc.group_tbl_lsn = fp.lsn;
        demo_desc.group_tbl_size = fp.size;
    }
}
