#include "common.h"

extern s32 func_00255928(u8 *src, void *dst, s32 *out);
extern s32 func_00255AA0(u8 *src, void *dst, s32 *out);

void func_002EEBAC(void*, u8*, s32);                /* memcpy? */

s32 func_00254830(u8 *src, void *dst, s32 *out) {
    s32 size = src[1] + (src[2] << 8) + (src[3] << 16) + (src[4] << 24);

    func_002EEBAC(dst, src + 5, size);

    *out = size;
    return size;
}

s32 func_002548A0(void* dst, u8 *src) {
  static s32 (*dispatchTable[])(u8*, void*, s32*) = { func_00254830, func_00255928, func_00255AA0 };
  s32 result;

  dispatchTable[src[0]](src, dst, &result);
  return result;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/archive/decode", func_002548E8);
