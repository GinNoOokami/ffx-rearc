#include <string.h>
#include "common.h"
#include "decode.h"

// Reads a little endian 'size' from src, then copies that many bytes from src to dst
s32 func_00254830(u8* src, void* dst, s32* out) {
    s32 size = DECODE_SIZE(src);

    memcpy(dst, src + 5, size);

    *out = size;
    return size;
}

s32 func_002548A0(void* dst, u8* src) {
    static fnDecodeHandler* dispatchTable[] = {func_00254830, func_00255928, func_00255AA0};
    s32 result;

    dispatchTable[src[0]](src, dst, &result);
    return result;
}

s32 func_002548E8(u8* src) {
    if (src[0] != 1) {
        dbgAssert("0", "decode.c", 0x40);
    }
    return DECODE_SIZE(src);
}
