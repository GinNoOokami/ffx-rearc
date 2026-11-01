#ifndef LIBMSCD_H
#define LIBMSCD_H

#include <libcdvd.h>

#include "common.h"

/* Read queue entry: 64 entries at D_0078E030 (= archive state +0x98), stride 0x38. */
typedef struct {
    /* 0x00 */ u32 unk0; // read size (sceRead); compared unsigned with unk2C
    // Tentative union, may be fake: a union access (alias set 0) is the only pointer-typed shape found
    // that reproduces func_00162DE0's scheduling; any bare pointer type schedules one copy differently.
    /* 0x04 */ union {
        void* p;
    } unk4;              // destination buffer (sceRead, memcpy)
    /* 0x08 */ s32 unk8; // func_00165D00's second argument
    /* 0x0C */ u8 padC[0x1C - 0xC];
    /* 0x1C */ s32 unk1C;   // sector: unk1C << 11 is the seek offset
    /* 0x20 */ void* unk20; // nonzero: func_00163938(entry) is called
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28; // func_00165D00's first argument; func_00163450's fifth
    /* 0x2C */ u32 unk2C; // size: rounded to and masked by 0x800
    /* 0x30 */ u8 unk30;
    /* 0x31 */ s8 unk31; // 0: entry skipped; cleared at the end
    /* 0x32 */ s8 unk32; // nonzero: func_00162DE0(index) instead
    /* 0x33 */ s8 unk33; // selects the copy path
    /* 0x34 */ u8 pad34[0x38 - 0x34];
} Unk0078E030; // size: 0x38

/* Archive state (D_0078DF98). Function slots are filled by func_00165588; the
   comments give the release function, and the host one from read_debug.c where
   func_002558B0 overrides the slot. "()" = parameters not known yet. */
typedef struct {
    /* 0x00 */ s32 (*unk0)();     // func_00164040
    /* 0x04 */ s32 (*unk4)(s32);  // func_00163198 / func_00254EF0
    /* 0x08 */ s32 (*unk8)();     // func_00164970
    /* 0x0C */ void (*unkC)(s32); // func_00165588 (this table's own init)
    /* 0x10 */ s32 (*unk10)();    // func_00163FD0
    /* 0x14 */ s32 (*unk14)(s32); // func_00162EB8 / func_00254E08
    /* 0x18 */ s32 unk18;         // func_00165588's argument; 0 = host mode
    /* 0x1C */ s32 (*unk1C)();    // func_00165300
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32* unk28;  // cdrom.mdg
    /* 0x2C */ s16* unk2C;  // cdrom.fid
    /* 0x30 */ void* unk30; // cdrom.fnd (host mode)
    /* 0x34 */ u8 pad34[0x54 - 0x34];
    /* 0x54 */ void* (*unk54)(const char*, s32*); // func_00162D30 / func_00254F40
    /* 0x58 */ const char* (*unk58)(s32);         // func_00164A10 / func_00254DA8
    /* 0x5C */ s32 unk5C;                         // file descriptor during a host read (func_00255158); -1 when idle
    /* 0x60 */ void (*unk60)(const char*);        // func_00162D18 / func_002556A0
    /* 0x64 */ s32 (*unk64)();                    // func_00162E48
    /* 0x68 */ void (*unk68)();                   // func_00163E90 / func_00255158
    /* 0x6C */ s32 (*unk6C)();                    // func_00163168
    /* 0x70 */ s32 unk70;                         // 1 while an entry is being processed, 0 after
    /* 0x74 */ s32 unk74;                         // index of that entry in D_0078E030, -1 after
    /* 0x78 */ s32 (*unk78)(void);                // func_00164628 / func_00255880
    /* 0x7C */ s32 (*unk7C)();                    // func_00165378
    /* 0x80 */ s32 (*unk80)();                    // func_001648F8
    /* 0x84 */ s32 (*unk84)();                    // func_001653A0
    /* 0x88 */ s32 (*unk88)();                    // func_00165548 / func_002558A0
    /* 0x8C */ s32 (*unk8C)();                    // func_00165528 / func_002558A8
    /* 0x90 */ s32 (*unk90)();                    // func_00164320
    /* 0x94 */ s32 unk94;
} ArchiveState; // size: 0x98

/* Entries of the table at *D_00595AEC (D_0030DAB0 entries, 512 allocated), found by func_00165D00. */
typedef struct {
    /* 0x00 */ u8 unk0; // matched against func_00165D00's first argument
    /* 0x01 */ u8 pad1[0x8 - 0x1];
    /* 0x08 */ s32 unk8;  // matched against its second argument
    /* 0x0C */ s32 unkC;  // seek offset in the loose file
    /* 0x10 */ s32 unk10; // read size
    /* 0x14 */ u8 pad14[0x24 - 0x14];
    /* 0x24 */ char unk24[0x10]; // file name under pfs0:/ex/
} Unk00595AEC;                   // size: 0x34

/* Used by libmscd but defined outside it (owning files not split yet). */
typedef struct {
    /* 0x00 */ u8 pad0[0xB];
    /* 0x0B */ s8 unkB;
    /* 0x0C */ s8 unkC;
    /* 0x0D */ u8 padD[0x10 - 0xD];
    /* 0x10 */ s8 unk10;
} Unk00588A00;

Unk00588A00* func_001244D0(void);
void func_0017133C(s32);
s32 func_002D1C60(void);

extern s8 D_00565DF4;

ArchiveState* func_00162DD0(void);
char* func_00162D48(void);
void func_00162DE0(s32);
void func_00163450(s32, s32, void*, sceCdRMode*, s32);
void func_00163768(Unk0078E030*);
void func_00163938(Unk0078E030*);
Unk00595AEC* func_00165D00(s32, s32);

extern volatile u8 D_0030DAB4;
extern Unk0078E030* D_0057C700;
extern ArchiveState D_0078DF98;
extern Unk0078E030 D_0078E030[];
extern char D_0078EE30[64][128];

#endif
