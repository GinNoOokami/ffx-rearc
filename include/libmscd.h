#ifndef LIBMSCD_H
#define LIBMSCD_H

#include <libcdvd.h>

#include "common.h"

#define READ_Q_CNT 64
#define READ_Q_PATH_SIZE 128

/* Read queue entry: 64 entries at read_q (= archive state +0x98), stride 0x38. */
typedef struct {
    /* 0x00 */ u32 size; // read size (sceRead); compared unsigned with disc_size
    // Tentative union, may be fake: a union access (alias set 0) is the only pointer-typed shape found
    // that reproduces func_00162DE0's scheduling; any bare pointer type schedules one copy differently.
    /* 0x04 */ union {
        void* p;
    } buff;            // destination buffer (sceRead, memcpy)
    /* 0x08 */ s32 no; // ExFileFind's second argument
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 lsn;        // sector: lsn << 11 is the seek offset
    /* 0x20 */ void* callback; // nonzero: ReadQCallbackCall(entry) is called
    /* 0x24 */ s32 req_no;
    /* 0x28 */ s32 group;     // ExFileFind's first argument; func_00163450's fifth
    /* 0x2C */ u32 disc_size; // size: rounded to and masked by 0x800
    /* 0x30 */ s8 unk30;
    /* 0x31 */ s8 active;     // 0: entry skipped; cleared at the end
    /* 0x32 */ s8 cancel;     // nonzero: func_00162DE0(index) instead
    /* 0x33 */ s8 compressed; // selects the copy path
    /* 0x34 */ s8 unk34;
    /* 0x35 */ u8 pad35[0x38 - 0x35];
} ReadQEntry; // size: 0x38

/* Archive state (sa_file). Function slots are filled by func_00165588; the
   comments give the release function, and the host one from read_debug.c where
   func_002558B0 overrides the slot. "()" = parameters not known yet. */
typedef struct {
    /* 0x00 */ s32 (*unk0)();      // func_00164040
    /* 0x04 */ s32 (*unk4)(s32);   // FileLsnGet / func_00254EF0
    /* 0x08 */ s32 (*unk8)();      // ReadQSync
    /* 0x0C */ void (*unkC)(s32);  // func_00165588 (this table's own init)
    /* 0x10 */ s16 (*unk10)(s32);  // func_00163FD0
    /* 0x14 */ s32 (*unk14)(s32);  // FileSizeGet / func_00254E08
    /* 0x18 */ s32 mode;           // func_00165588's argument; 0 = host mode
    /* 0x1C */ void (*unk1C)(s32); // ReadQCancel
    /* 0x20 */ s32 id_base;
    /* 0x24 */ s32 group;
    /* 0x28 */ u32* file_tbl;  // cdrom.mdg
    /* 0x2C */ s16* group_tbl; // cdrom.fid
    /* 0x30 */ void* name_tbl; // cdrom.fnd (host mode)
    /* 0x34 */ s32 unk34;      // func_00162778 (279)
    /* 0x38 */ s32 unk38;      // func_00162838 (280): cdrom.mdg's disc LSN?
    /* 0x3C */ s32 unk3C;      // func_001628F8 (344): cdrom.fid's disc LSN?
    /* 0x40 */ s32 unk40;      // func_00162A78 (48)
    /* 0x44 */ s32 unk44;      // func_001629B8 (0x20000): cdrom.mdg's size?
    /* 0x48 */ s32 unk48;      // func_00162B38 (130): cdrom.fid's size?
    /* 0x4C */ s32 unk4C;      // func_00162BF8 (2)
    /* 0x50 */ s32 unk50;
    /* 0x54 */ void* (*unk54)(const char*, s32*); // func_00162D30 / func_00254F40
    /* 0x58 */ const char* (*unk58)(s32);         // func_00164A10 / func_00254DA8
    /* 0x5C */ s32 host_fd;                       // file descriptor during a host read (func_00255158); -1 when idle
    /* 0x60 */ void (*unk60)(const char*);        // func_00162D18 / func_002556A0
    /* 0x64 */ void (*unk64)(void);               // func_00162E48
    /* 0x68 */ void (*unk68)();                   // func_00163E90 / func_00255158
    /* 0x6C */ u32 (*unk6C)(s32, s32);            // GroupFileLsnGet
    /* 0x70 */ s32 busy;                          // 1 while an entry is being processed, 0 after
    /* 0x74 */ s32 busy_index;                    // index of that entry in read_q, -1 after
    /* 0x78 */ s32 (*unk78)(s32, void*, u32, void*, ReadQEntry*); // func_00164628 / func_00255880
    /* 0x7C */ void (*unk7C)(s32);                                // func_00165378
    /* 0x80 */ s32 (*unk80)();                                    // ReadQFileStatGet
    /* 0x84 */ void (*unk84)(s32);                                // func_001653A0
    /* 0x88 */ s32 (*unk88)();                                    // func_00165548 / func_002558A0
    /* 0x8C */ s32 (*unk8C)();                                    // func_00165528 / func_002558A8
    /* 0x90 */ s32 (*unk90)();                                    // func_00164320
    /* 0x94 */ s32 unk94;
} SaFile; // size: 0x98

/* Files installed on the HDD that stand in for archive entries, keyed by (group, no). Named after
 * their directory, "pfs0:/ex/%s" (what "ex" stands for is unknown). Entries of ex_file_tbl
 * (ex_file_cnt in use, 512 allocated), found by ExFileFind. */
typedef struct {
    /* 0x00 */ u8 group; // matched against ExFileFind's first argument
    /* 0x01 */ u8 pad1[0x8 - 0x1];
    /* 0x08 */ s32 no;     // matched against its second argument
    /* 0x0C */ s32 offset; // seek offset in the loose file
    /* 0x10 */ s32 size;   // read size
    /* 0x14 */ u8 pad14[0x24 - 0x14];
    /* 0x24 */ char name[0x10]; // file name under pfs0:/ex/
} ExFile;                       // size: 0x34

/* Used by libmscd but defined outside it (owning files not split yet). */
typedef struct {
    /* 0x00 */ u8 pad0[0xB];
    /* 0x0B */ s8 unkB;
    /* 0x0C */ s8 unkC;
    /* 0x0D */ u8 padD[0x10 - 0xD];
    /* 0x10 */ s8 unk10;
} Unk00588A00;

Unk00588A00* func_001244D0(void);
void func_0017133C(s32); // sleep?
s32 func_002D1C60(void);

extern s8 D_00565DF4;

SaFile* SaFileGet(void);
char* func_00162D48(void);
void func_00162DE0(s32);
s32 func_00163450(s32 lsn, s32 size, void* buffer, sceCdRMode* mode, s32 group);
void func_00163768(ReadQEntry*);
void ReadQCallbackCall(ReadQEntry*);
ExFile* ExFileFind(s32, s32);
s32 func_002D93E8();

extern volatile u8 D_0030DAB4;
extern ReadQEntry* D_0057C700;
extern SaFile sa_file;
extern ReadQEntry read_q[READ_Q_CNT];
extern char read_q_path[READ_Q_CNT][READ_Q_PATH_SIZE];

#endif
