#ifndef LIBMSCD_H
#define LIBMSCD_H

#include <sdk/libcdvd.h>

#include "common.h"

#define READ_Q_CNT 64
#define READ_Q_PATH_SIZE 128

/* Request type of a read queue entry: read data, or only seek to its LSN. */
typedef enum { READ_Q_TYPE_READ, READ_Q_TYPE_SEEK } ReadQType;

/* Read queue entry: 64 entries at read_q (= archive state +0x98), stride 0x38. */
typedef struct {
    /* 0x00 */ u32 size;     // read size (sceRead); compared unsigned with disc_size
    /* 0x04 */ void* buffer; // destination buffer (sceRead, memcpy)
    /* 0x08 */ s32 no;       // ExFileFind's second argument
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 lsn;        // sector: lsn << 11 is the seek offset
    /* 0x20 */ void* callback; // nonzero: ReadQCallbackCall(entry) is called
    /* 0x24 */ s32 req_no;
    /* 0x28 */ s32 group;     // ExFileFind's first argument; CdReadRetry's fifth
    /* 0x2C */ u32 disc_size; // size: rounded to and masked by 0x800
    /* 0x30 */ s8 type;       // ReadQType
    /* 0x31 */ s8 active;     // 0: entry skipped; cleared at the end
    /* 0x32 */ s8 cancel;     // nonzero: ReadQEntryClear(index) instead
    /* 0x33 */ s8 compressed; // selects the copy path
    /* 0x34 */ s8 unk34;
    /* 0x35 */ u8 pad35[0x38 - 0x35];
} ReadQEntry; // size: 0x38

/* Archive state (sa_file). Function slots are filled by SaFileModeSet; the
   comments give the release function, and the host one from read_debug.c where
   HostModeSet overrides the slot. "()" = parameters not known yet. */
typedef struct {
    /* 0x00 */ s32 (*unk0)();      // ReadQFileAdd
    /* 0x04 */ s32 (*unk4)(s32);   // FileLsnGet / HostFileLsnGet
    /* 0x08 */ s32 (*unk8)();      // ReadQSync
    /* 0x0C */ void (*unkC)(s32);  // SaFileModeSet (this table's own init)
    /* 0x10 */ s16 (*unk10)(s32);  // GroupSet
    /* 0x14 */ s32 (*unk14)(s32);  // FileSizeGet / HostFileSizeGet
    /* 0x18 */ s32 mode;           // SaFileModeSet's argument; 0 = host mode
    /* 0x1C */ void (*unk1C)(s32); // ReadQCancel
    /* 0x20 */ s32 id_base;
    /* 0x24 */ s32 group;
    /* 0x28 */ u32* file_tbl;      // cdrom.mdg
    /* 0x2C */ s16* group_tbl;     // cdrom.fid
    /* 0x30 */ void* name_tbl;     // cdrom.fnd (host mode)
    /* 0x34 */ s32 unk34;          // func_00162778 (279)
    /* 0x38 */ s32 file_tbl_lsn;   // FileTblLsnGet (280); never read
    /* 0x3C */ s32 group_tbl_lsn;  // GroupTblLsnGet (344); never read
    /* 0x40 */ s32 unk40;          // func_00162A78 (48)
    /* 0x44 */ s32 file_tbl_size;  // FileTblSizeGet (0x20000); never read
    /* 0x48 */ s32 group_tbl_size; // GroupTblSizeGet (130); never read
    /* 0x4C */ s32 unk4C;          // func_00162BF8 (2)
    /* 0x50 */ s32 unk50;
    /* 0x54 */ void* (*pc_load)(const char*, s32*); // PCload / HostPcLoad
    /* 0x58 */ const char* (*unk58)(s32);           // FileNameGet / HostFileNameGet
    /* 0x5C */ s32 host_fd;                         // file descriptor during a host read (HostReadQServe); -1 when idle
    /* 0x60 */ void (*pc_file_tbl_set)(const char*);              // PcFileTblSet / HostPcFileTblSet
    /* 0x64 */ void (*unk64)(void);                               // ReadQInit
    /* 0x68 */ void (*unk68)();                                   // ReadQServe / HostReadQServe
    /* 0x6C */ u32 (*unk6C)(s32, s32);                            // GroupFileLsnGet
    /* 0x70 */ s32 busy;                                          // 1 while an entry is being processed, 0 after
    /* 0x74 */ s32 busy_index;                                    // index of that entry in read_q, -1 after
    /* 0x78 */ s32 (*unk78)(s32, void*, u32, void*, ReadQEntry*); // ReadQLsnAdd / HostReadQLsnAdd
    /* 0x7C */ void (*read_spin_set)(s32);                        // ReadSpinSet
    /* 0x80 */ s32 (*unk80)();                                    // ReadQFileStatGet
    /* 0x84 */ void (*movie_spin_set)(s32);                       // MovieSpinSet
    /* 0x88 */ s32 (*unk88)();                                    // ReadQFileSeekAdd / HostReadQFileSeekAdd
    /* 0x8C */ s32 (*unk8C)();                                    // ReadQLsnSeekAdd / HostReadQLsnSeekAdd
    /* 0x90 */ s32 (*unk90)();                                    // ReadQFilePartAdd
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

SaFile* SaFileGet(void);
char* ScratchBuffGet(void);
void ReadQEntryClear(s32);
s32 FileSizeGet(s32);
s32 TblLsnGet(s32);
s32 CdReadRetry(s32 lsn, s32 size, void* buffer, sceCdRMode* mode, s32 group);
void ReadQEntryUnpack(ReadQEntry*);
void ReadQCallbackCall(ReadQEntry*);
s32 ReadQLsnAdd(s32 lsn, void* buffer, u32 size, void* callback, ReadQEntry* arg4);
ExFile* ExFileFind(s32, s32);
s32 ExFileSizeGet(s32, s32);

// Variant getters, defined in 171CC8.c (`jr ra; li v0, N`): nine values, seven variants each.
// Each selector at the top of libmscd.c returns its getter for GetGameRegion()'s region
s32 SizeTblNoGetFr(void);
s32 func_00171CD0(void);
s32 FileTblLsnGetFr(void);
s32 GroupTblLsnGetFr(void);
s32 FileTblSizeGetFr(void);
s32 func_00171CF0(void);
s32 GroupTblSizeGetFr(void);
s32 func_00171D00(void);
s32 ModuleSizeTblNoGetFr(void);
s32 SizeTblNoGetIt(void);
s32 func_00171D18(void);
s32 FileTblLsnGetIt(void);
s32 GroupTblLsnGetIt(void);
s32 FileTblSizeGetIt(void);
s32 func_00171D38(void);
s32 GroupTblSizeGetIt(void);
s32 func_00171D48(void);
s32 ModuleSizeTblNoGetIt(void);
s32 SizeTblNoGetJp(void);
s32 func_00171D60(void);
s32 FileTblLsnGetJp(void);
s32 GroupTblLsnGetJp(void);
s32 FileTblSizeGetJp(void);
s32 func_00171D80(void);
s32 GroupTblSizeGetJp(void);
s32 func_00171D90(void);
s32 ModuleSizeTblNoGetJp(void);
s32 SizeTblNoGetUk(void);
s32 func_00171DA8(void);
s32 FileTblLsnGetUk(void);
s32 GroupTblLsnGetUk(void);
s32 FileTblSizeGetUk(void);
s32 func_00171DC8(void);
s32 GroupTblSizeGetUk(void);
s32 func_00171DD8(void);
s32 ModuleSizeTblNoGetUk(void);
s32 SizeTblNoGetUs(void);
s32 func_00171DF0(void);
s32 FileTblLsnGetUs(void);
s32 GroupTblLsnGetUs(void);
s32 FileTblSizeGetUs(void);
s32 func_00171E10(void);
s32 GroupTblSizeGetUs(void);
s32 func_00171E20(void);
s32 ModuleSizeTblNoGetUs(void);
s32 SizeTblNoGetDe(void);
s32 func_00171E38(void);
s32 FileTblLsnGetDe(void);
s32 GroupTblLsnGetDe(void);
s32 FileTblSizeGetDe(void);
s32 func_00171E58(void);
s32 GroupTblSizeGetDe(void);
s32 func_00171E68(void);
s32 ModuleSizeTblNoGetDe(void);
s32 SizeTblNoGetEs(void);
s32 func_00171E80(void);
s32 FileTblLsnGetEs(void);
s32 GroupTblLsnGetEs(void);
s32 FileTblSizeGetEs(void);
s32 func_00171EA0(void);
s32 GroupTblSizeGetEs(void);
s32 func_00171EB0(void);
s32 ModuleSizeTblNoGetEs(void);

extern volatile u8 D_0030DAB4;
extern ReadQEntry* D_0057C700;
extern SaFile sa_file;
extern ReadQEntry read_q[READ_Q_CNT];
extern char read_q_path[READ_Q_CNT][READ_Q_PATH_SIZE];

#endif
