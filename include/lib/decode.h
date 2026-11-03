#ifndef DECODE_H
#define DECODE_H

#include "common.h"

// Unpacked size from a packed block's header (bytes 1-4, little endian)
#define DECODE_SIZE(p) ((p)[1] + ((p)[2] << 8) + ((p)[3] << 16) + ((p)[4] << 24))

// decode.c
s32 func_002548A0(void* dst, u8* src); // unpacks src (method byte + LE size + data) into dst, returns the size

// Hand-written LZ decoders (archive/255928.s). Methods 1 and 2, called through
// func_002548A0's table:
typedef s32 fnDecodeHandler(u8* src, void* dst, s32* out);
extern fnDecodeHandler func_00255928;
extern fnDecodeHandler func_00255AA0;

// The same decoders with an end-of-input check, for input split between src (len bytes) and
// tail; used by ReadQEntryUnpack:
void func_00255C18(u8* src, void* dst, s32 len, char* tail);
void func_00255DE8(u8* src, void* dst, s32 len, char* tail);

#endif
