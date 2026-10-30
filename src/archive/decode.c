#include "common.h"

extern s32 func_00254830(u8 *src, void *dst, s32 *out);
extern void func_00255928();
extern void func_00255AA0();

void (*D_00326CD0[])() = { func_00254830, func_00255928, func_00255AA0, NULL };

void func_002EEBAC(void*, u8*, s32);                /* memcpy? */

s32 func_00254830(u8 *src, void *dst, s32 *out) {
    s32 size = src[1] + (src[2] << 8) + (src[3] << 16) + (src[4] << 24);

    func_002EEBAC(dst, src + 5, size);

    *out = size;
    return size;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/archive/decode", func_002548A0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/archive/decode", func_002548E8);
