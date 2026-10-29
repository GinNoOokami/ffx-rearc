#ifndef COMMON_H
#define COMMON_H

#include "include_asm.h"

#define NULL ((void*)0)

/* Signed integers */
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long s64;
typedef int s128 __attribute__((mode(TI)));         /* 128-bit (EE registers, lq/sq) */

/* Unsigned integers */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
typedef unsigned int u128 __attribute__((mode(TI)));

/* Floating point */
typedef float f32;
typedef double f64;

#endif
