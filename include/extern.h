#ifndef EXTERN_H
#define EXTERN_H

/* Declarations for code whose source file isn't split yet, grouped by area. Move each group to its
 * owner's header once that file exists. */

#include "common.h"
#include "lib/libmscd.h"

/* SDK: kernel */
void FlushCache(s32);

/* main program */
const char* func_0011A480(void); // returns "sugimoto"
void func_0011C348(s32);

/* HDD (0x165E98-; hddcopy.c from 0x166A90) */
s32 func_00165EB8(void);
void func_00165ED8(void);
s32 func_00165F30(s32);
void func_00165F88(void);
s8 func_00166370(void);
s32 func_00166380(void);
void func_00166438(void);
void func_00166650(s32, void*, s32, s32);
void func_00166798(char*, void*, s32, s32);
void func_00166878(void);
/*?*/ void func_00167C58(/*?*/ s32);
s32 func_00167F58(ExFile*);

/* 0x17133C-0x1715A0, before sa_task.c */
void func_0017133C(s32); // sleep?
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

/* other game code */
void func_00179058(s32, const char*, const char*, ...);
/*?*/ s32 func_001ACEE8(char*); // getRegionCode?
s32 func_001BD228(void);
s32 func_0020D378(void);

/* 0x2D1C60- */
s32 func_002D1C60(void);
void func_002D1C70(void (*)());
// TK frame wait: blocks until at least max(n, 1) VBlanks (INTC 2 handler func_002D3B08 counts
// them in D_0078AC00) have passed since the last call, then resets the count and runs the
// per-frame TK updates.
void func_002D3C80(s32);
s32 func_002D6418(void);
s32 func_002D6478(void);
void func_002D8E20(const char* ch, const char* str, u32 len);
s32 func_002D93E8(void);
u32 func_002D9460(void);

/* data */
extern s8 D_00565DF4;

#endif
