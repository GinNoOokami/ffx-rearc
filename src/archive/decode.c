#include "common.h"

extern void func_00254830();
extern void func_00255928();
extern void func_00255AA0();

void (*D_00326CD0[])() = { func_00254830, func_00255928, func_00255AA0, 0 };

INCLUDE_ASM("asm/jp_intl/nonmatchings/archive/decode", func_00254830);

INCLUDE_ASM("asm/jp_intl/nonmatchings/archive/decode", func_002548A0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/archive/decode", func_002548E8);
