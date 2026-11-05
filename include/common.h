#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

#include "include_asm.h"

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define ALIGN_16(x) (ALIGN(x, 16))
#define ALIGN_32(x) (ALIGN(x, 32))
#define ALIGN_64(x) (ALIGN(x, 64))

/* Signed integers */
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long s64;
typedef int s128 __attribute__((mode(TI))); /* 128-bit (EE registers, lq/sq) */

/* Unsigned integers */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
typedef unsigned int u128 __attribute__((mode(TI)));

/* Floating point */
typedef float f32;
typedef double f64;

// dbgAssert: called with an expression or message string, a file name and a line number
// ("fd >= 0", "read_debug.c", 0x14D; "PcFileTblSet  DVD MODE ONLY", "libmscd.c", 0x20C)
void dbgAssert(const char* expr, const char* file, s32 line);

static inline void dbgBreak(void) { asm("break 0\n\t"); }

#endif
