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

// Release the game runs as (one .data byte, set by script command C05D). Codes from the US alpha's
// per-release host paths (menu/<code>/...).
typedef enum {
    GAME_REGION_JP, // default (International's initial value)
    GAME_REGION_US,
    GAME_REGION_DE,
    GAME_REGION_FR,
    GAME_REGION_IT,
    GAME_REGION_ES,
    GAME_REGION_UK,
} GameRegion;

GameRegion GetGameRegion(void);

#endif
