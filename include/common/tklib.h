#ifndef TKLIB_H
#define TKLIB_H

#include "common.h"

typedef struct {
    /* 0x00 */ u8 pad0[0xB];
    /* 0x0B */ s8 unkB;
    /* 0x0C */ s8 unkC;
    /* 0x0D */ u8 padD[0x10 - 0xD];
    /* 0x10 */ s8 unk10;
} Unk00588A00;

Unk00588A00* func_001244D0(void);
void func_00125B40(void);

#endif
