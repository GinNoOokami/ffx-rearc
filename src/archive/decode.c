#include "common.h"

typedef s32 fnHandler(u8 *src, void *dst, s32 *out);

extern fnHandler func_00255928;
extern fnHandler func_00255AA0;

void func_002EEBAC(void*, u8*, s32);                /* memcpy? */
void func_00157788(char *, char *, s32);            /* assert? */

// Reads a little endian 'size' from src, then copies that many bytes from src to dst
s32 func_00254830(u8 *src, void *dst, s32 *out) {
    s32 size = src[1] + (src[2] << 8) + (src[3] << 16) + (src[4] << 24);

    func_002EEBAC(dst, src + 5, size);

    *out = size;
    return size;
}

s32 func_002548A0(void* dst, u8 *src) {
  static fnHandler *dispatchTable[] = { func_00254830, func_00255928, func_00255AA0 };
  s32 result;

  dispatchTable[src[0]](src, dst, &result);
  return result;
}

s32 func_002548E8(u8 *src) {
    if (src[0] != 1) {
        func_00157788("0", "decode.c", 0x40);
    }
    return src[1] + (src[2] << 8) + (src[3] << 16) + (src[4] << 24);
}
