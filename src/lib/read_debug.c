#include <sdk/sifdev.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "extern.h"
#include "lib/libmscd.h"
#include "mem.h"
#include "lib/read_debug.h"
#include "common/tklib.h"

#define FILE_NAME_CHG_TBL_CNT 16

extern char cddata_mdg_path[];       // host:/ffx/proj/battle/jp/cddata/cdrom.mdg
extern char cddata_fid_path[];       // host:/ffx/proj/battle/jp/cddata/cdrom.fid
extern char cddata_fnd_path[];       // host:/ffx/proj/battle/jp/cddata/cdrom.fnd
extern char cdidx_mdg_path[];        // host:/ffx/proj/prog/cdidx/jp/cdrom.mdg
extern char cdidx_fid_path[];        // host:/ffx/proj/prog/cdidx/jp/cdrom.fid
extern char cdidx_sizetbl_path[];    // host:/ffx/proj/prog/cdidx/jp/sizetbl.bin
extern char cdidx_modulesize_path[]; // host:/ffx/proj/prog/cdidx/jp/modulesize.bin
extern s8 file_name_chg_init;
extern s8 D_00565DF0;
extern const char* file_name_chg_from[FILE_NAME_CHG_TBL_CNT];
extern const char* file_name_chg_to[FILE_NAME_CHG_TBL_CNT];
extern char file_name_buff[0x200]; // name buffer, size inferred
extern char D_006A2F10[];

// rodata string literals
extern char D_00573400[]; // "read_debug.c", emitted by INCLUDE_RODATA

void HostReadQServe(void);

// Clears the file-name change table, the first time only (file_name_chg_init).
void FileNameChgInit(void) {
    s32 i;

    if (file_name_chg_init == 0) {
        file_name_chg_init = 1;
        for (i = 0; i < FILE_NAME_CHG_TBL_CNT; i++) {
            file_name_chg_from[i] = NULL;
            file_name_chg_to[i] = NULL;
        }
    }
}

// Adds a name change: text matching arg0 in a name from TblNameGet is replaced by arg1.
// Returns its slot.
s32 FileNameChgAdd(const char* arg0, const char* arg1) {
    s32 i = 0;

    if (file_name_chg_from[0] != NULL) {
        for (i = 0; i < FILE_NAME_CHG_TBL_CNT; i++) {
            if (file_name_chg_from[i] == NULL) {
                break;
            }
        }
    }

    if (i == FILE_NAME_CHG_TBL_CNT) {
        dbgAssert("i!=FILE_NAME_CHG_TBL_CNT", D_00573400, 0x50);
    }

    file_name_chg_from[i] = arg0;
    file_name_chg_to[i] = arg1;
    return i;
}

// Removes name change arg0.
void FileNameChgDel(s32 arg0) {
    file_name_chg_from[arg0] = NULL;
    file_name_chg_to[arg0] = NULL;
}

// Returns the cdrom.fnd name table (sa_file.name_tbl), NULL if none is loaded.
void* NameTblGet(void) { return SaFileGet()->name_tbl; }

// Returns file `index`'s name from the cdrom.fnd table, expanded into a static buffer:
// "$USER$" becomes func_0011A480()'s string, and a prefix found in the name-change table
// (FileNameChgAdd) becomes its replacement. NULL if there is no table or no such name.
const char* TblNameGet(s32 index) {
    const char* name = NULL;
    const char* user;
    const char* str;
    s32* fileTbl;
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 n;

    // fileTbl[0] = entry count, fileTbl[1 + n] = offset of name n from the table start; a name
    // runs to the next offset, and an entry with no length has no name.
    fileTbl = NameTblGet();
    if (fileTbl != NULL && index < fileTbl[0]) {
        s32 offset = fileTbl[index + 1];

        if (fileTbl[index + 2] - offset > 0) {
            name = (char*)fileTbl + offset;
        }
    }

    if (name != NULL) {
        user = func_0011A480();
        for (j = 0, i = 0; file_name_buff[j] = name[i], name[i] != '\0'; i++, j++) {
            const char* p = &name[i];

            if (p[0] == '$' && p[1] == 'U' && p[2] == 'S' && p[3] == 'E' && p[4] == 'R' && p[5] == '$') {
                if (user != NULL) {
                    for (k = 0; file_name_buff[j++] = user[k], user[k] != '\0'; k++) {
                    }
                    // The copy leaves j past the '\0'; back up so the loop's j++ lands on it.
                    j -= 2;
                    i += 5;
                }
            } else {
                // A match doesn't end the search: later entries are tried at the new position,
                // and a second match overwrites the last character of the first replacement.
                for (n = 0; n < FILE_NAME_CHG_TBL_CNT; n++) {
                    if (file_name_chg_from[n] == NULL) {
                        continue;
                    }

                    str = file_name_chg_from[n];
                    for (k = 0; name[i + k] != '\0' && str[k] != '\0' && name[i + k] == str[k]; k++) {
                    }

                    if (str[k] == '\0') {
                        str = file_name_chg_to[n];
                        for (m = 0; file_name_buff[j++] = str[m], str[m] != '\0'; m++) {
                        }
                        j -= 2;
                        i += k - 1;
                    }
                }
            }
        }

        name = file_name_buff;
    }

    return name;
}

// Name of file id in the current group (TblNameGet).
const char* FileNameGet(s32 arg0) { return TblNameGet(SaFileGet()->id_base + arg0); }

// Name of file arg1 in group arg0 (TblNameGet).
const char* GroupFileNameGet(s32 arg0, s32 arg1) { return TblNameGet(sa_file.group_tbl[arg0] + arg1); }

// Host slot +0x14: the ex-file size if there is one; else FileSizeGet when D_00565DF4 is set,
// else the host file's size rounded up to 16 bytes.
s32 HostFileSizeGet(s32 id) {
    SaFile* state = SaFileGet();
    s32 size = ExFileSizeGet(sa_file.group, id);
    s32 index;
    s32 fd;

    if (size != 0) {
        return size;
    }
    if (D_00565DF4 != 0) {
        size = FileSizeGet(id);
    } else {
        index = state->id_base + id;
        if (NameTblGet() != NULL) {
            const char* name = TblNameGet(index);
            if (name != NULL) {
                fd = sceOpen(name, SCE_RDONLY);
                if (fd < 0) {
                    return 0;
                }
                size = sceLseek(fd, 0, SCE_SEEK_END);
                size = (size + 15) / 16 * 16;
                sceClose(fd);
            }
        }
    }
    return size;
}

// Host slot +0x04: the same lookup as FileLsnGet.
s32 HostFileLsnGet(s32 arg0) { return TblLsnGet(sa_file.id_base + arg0); }

INCLUDE_RODATA("asm/jp_intl/nonmatchings/lib/read_debug", D_00573400);

// Assert an invalid fd
void FdCheck(s32 fd, const char* path) {
    if (fd < 0) {
        dbgAssert("fd >= 0", D_00573400, 0x14D);
    }
}

// Debug PCload
void* HostPcLoad(const char* path, s32* arg1) {
    void* block;
    s32 size;
    s32 fd;

    fd = sceOpen(path, SCE_RDONLY);
    FdCheck(fd, path);

    size = sceLseek(fd, 0, SCE_SEEK_END);
    sceLseek(fd, 0, SCE_SEEK_SET);

    block = func_0011D370((size + 3) / 4 * 4, 10, D_00573400, 0x165);

    FlushCache(0);

    sceRead(fd, block, size);
    sceClose(fd);
    FlushCache(0);

    if (arg1 != NULL) {
        *arg1 = size;
    }

    return block;
}

void* func_00255020(const char* path, s32* arg1) {
    s32 size;
    s32 fd;
    void* block;

    fd = sceOpen(path, SCE_RDONLY);
    FdCheck(fd, path);

    size = sceLseek(fd, 0, SCE_SEEK_END);
    sceLseek(fd, 0, SCE_SEEK_SET);

    if ((func_002D93E8() == 1) && D_00565DF0 == 0) {
        block = func_0011D370((size + 3) / 4 * 4, 10, D_00573400, 0x188);
    } else {
        block = func_0011D420((size + 3) / 4 * 4, 10, D_00573400, 0x18A);
    }
    FlushCache(0);

    sceRead(fd, block, size);
    sceClose(fd);
    FlushCache(0);

    if (arg1 != NULL) {
        *arg1 = size;
    }

    return block;
}

// Not matching yet. With NON_MATCHING the C below is one instruction off, but only because of
// three changes that steer the compiler and are unlikely to be in the original:
// - `goto copy`: the target shares one FlushCache/memcpy tail between the two reads of the
//   disc_size block. Written as plain if/else, the first scheduler moves `rest = size & 0x7FF`
//   above the secs == 0 read call (the target keeps it after); the call then ends its basic
//   block, flow inserts a (use (const_int 0)) after it, and cross-jumping no longer merges
//   the two identical tails.
// - the empty `do {} while (0)` at the end of the D_00565DF4 != 0 branch: without it there
//   is an extra jump at the end of that branch.
// - `one`: the ex-file waits pass a local set to 1. It makes `archive->busy = 1` use a0 and
//   keeps the D_0030DAB4 = 1 / D_0057C700 = entry store order, as in the target.
// Still different: in the secs == 0 read, `rest` lands in the call's delay slot; the target
// sets a3 = sp there and computes rest after the call. The same C without the three changes
// is 23 instructions off. The release counterpart ReadQEntryRead has the same read logic.
#ifndef NON_MATCHING
INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/read_debug", HostReadQServe);
#else
void HostReadQServe(void) {
    sceCdRMode mode;
    s32 stat;
    SaFile* archive;
    char* buf;
    ReadQEntry* entry;
    ExFile* exFile;
    s32 fd;
    s32 size;
    s32 secs;
    s32 rest;
    s32 i;
    s32 one;

    archive = SaFileGet();
    buf = ScratchBuffGet();

    for (i = 0; i < 64; i++) {
        entry = &read_q[i];
        if (entry->active == 0) {
            continue;
        }
        if (entry->cancel != 0) {
            ReadQEntryClear(i);
            continue;
        }

        archive->busy_index = i;
        archive->busy = 1;
        exFile = ExFileFind(entry->group, entry->no);
        if (exFile != NULL) {
            sprintf(D_006A2F10, "pfs0:/ex/%s", exFile->name);
            one = 1;
            fd = sceOpen(D_006A2F10, SCE_RDONLY | SCE_NOWAIT);
            if (fd < 0) {
                dbgBreak();
            }
            while (sceIoctl(fd, SCE_FS_EXECUTING, &stat), stat != 0) {
                func_0017133C(one);
            }
            sceLseek(fd, exFile->offset, SCE_SEEK_SET);
            while (sceIoctl(fd, SCE_FS_EXECUTING, &stat), stat != 0) {
                func_0017133C(one);
            }
            FlushCache(0);
            sceRead(fd, entry->buffer, exFile->size);
            while (sceIoctl(fd, SCE_FS_EXECUTING, &stat), stat != 0) {
                func_0017133C(one);
            }
            sceClose(fd);
            FlushCache(0);
        } else if (D_00565DF4 != 0) {
            size = entry->disc_size;
            if (entry->compressed != 0 && entry->size < size) {
                size = entry->size + 5;
                secs = size / 2048;
                if (secs != 0 && size > 0) {
                    rest = secs * 2048;
                    CdReadRetry(entry->lsn, size, entry->buffer, &mode, entry->group);
                    FlushCache(0);
                    memcpy(entry->buffer, (char*)entry->buffer + 5, rest);
                    CdReadRetry(entry->lsn + secs, 0x800, buf, &mode, entry->group);
                    FlushCache(0);
                    size -= rest;
                    memcpy((char*)entry->buffer + rest, buf, size - 5);
                } else {
                    CdReadRetry(entry->lsn, 0x800, buf, &mode, entry->group);
                    FlushCache(0);
                    memcpy(entry->buffer, buf + 5, size - 5);
                }
            } else {
                secs = size / 2048;
                if (secs != 0) {
                    CdReadRetry(entry->lsn, entry->disc_size, entry->buffer, &mode, entry->group);
                    rest = size & 0x7FF;
                    if (rest != 0) {
                        CdReadRetry(entry->lsn + secs, 0x800, buf, &mode, entry->group);
                        goto copy;
                    }
                    FlushCache(0);
                } else {
                    CdReadRetry(entry->lsn, 0x800, buf, &mode, entry->group);
                    rest = size & 0x7FF;
                copy:
                    FlushCache(0);
                    memcpy((char*)entry->buffer + size - rest, buf, rest);
                }
                if (entry->compressed != 0) {
                    if (func_001244D0()->unk10 != 0) {
                        while (D_0030DAB4 != 0) {
                            func_0017133C(1);
                        }
                        D_0030DAB4 = 1;
                        D_0057C700 = entry;
                        func_002D1C60();
                        while (D_0030DAB4 != 0) {
                            func_0017133C(1);
                        }
                    } else {
                        ReadQEntryUnpack(entry);
                    }
                }
            }
            do {
            } while (0);
        } else {
            if (entry->lsn != 0) {
                archive->host_fd = sceOpen(read_q_path[i], SCE_RDONLY);
                FdCheck(archive->host_fd, read_q_path[i]);
                sceLseek(archive->host_fd, entry->lsn << 11, SCE_SEEK_SET);
                FlushCache(0);
                sceRead(archive->host_fd, entry->buffer, entry->size);
                sceClose(archive->host_fd);
                FlushCache(0);
            } else {
                archive->host_fd = sceOpen(read_q_path[i], SCE_RDONLY | SCE_NOWAIT);
                FdCheck(archive->host_fd, read_q_path[i]);
                FlushCache(0);
                sceRead(archive->host_fd, entry->buffer, entry->size);
                while (sceIoctl(archive->host_fd, SCE_FS_EXECUTING, &stat), stat != 0) {
                    func_0017133C(1);
                }
                sceClose(archive->host_fd);
                FlushCache(0);
            }
        }

        if (entry->callback != NULL) {
            ReadQCallbackCall(entry);
        }
        archive->busy = 0;
        archive->busy_index = -1;
        archive->host_fd = -1;
        entry->active = 0;
    }
}
#endif

// Debug PcFileTblSet
void HostPcFileTblSet(const char* path) {
    if (sa_file.name_tbl != NULL) {
        func_0011D590(sa_file.name_tbl, 10);
    }
    sa_file.name_tbl = func_00255020(path, NULL);
}

// Loads the index tables from host paths with the region code patched in: cdrom.mdg/.fid/.fnd from
// cddata, or with D_00565DF4 set cdrom.mdg/.fid from cdidx and no name table.
void HostIndexLoad(void) {
    char region[2];

    func_001ACEE8(region);

    if (D_00565DF4 == 0) {
        cddata_mdg_path[0x16] = region[0];
        cddata_mdg_path[0x17] = region[1];
        cddata_fid_path[0x16] = region[0];
        cddata_fid_path[0x17] = region[1];
        cddata_fnd_path[0x16] = region[0];
        cddata_fnd_path[0x17] = region[1];

        sa_file.file_tbl = HostPcLoad(cddata_mdg_path, NULL);
        sa_file.group_tbl = HostPcLoad(cddata_fid_path, NULL);
        sa_file.name_tbl = func_00255020(cddata_fnd_path, NULL);
    } else {
        cdidx_mdg_path[0x1A] = region[0];
        cdidx_mdg_path[0x1B] = region[1];
        cdidx_fid_path[0x1A] = region[0];
        cdidx_fid_path[0x1B] = region[1];

        sa_file.file_tbl = HostPcLoad(cdidx_mdg_path, NULL);
        sa_file.group_tbl = HostPcLoad(cdidx_fid_path, NULL);
        sa_file.name_tbl = NULL;
    }
}

// Loads sizetbl.bin from the host (cdidx path, region code patched in).
u8* HostSizeTblLoad(void) {
    char region[2];

    func_001ACEE8(region);

    cdidx_sizetbl_path[0x1A] = region[0];
    cdidx_sizetbl_path[0x1B] = region[1];

    return HostPcLoad(cdidx_sizetbl_path, NULL);
}

// Loads modulesize.bin from the host (cdidx path, region code patched in).
void* HostModuleSizeTblLoad(void) {
    char region[2];

    func_001ACEE8(region);

    cdidx_modulesize_path[0x1A] = region[0];
    cdidx_modulesize_path[0x1B] = region[1];

    return HostPcLoad(cdidx_modulesize_path, NULL);
}

// Host slot +0x78: passes straight to ReadQLsnAdd.
s32 HostReadQLsnAdd(s32 lsn, void* buffer, u32 size, void* callback, ReadQEntry* arg4) {
    return ReadQLsnAdd(lsn, buffer, size, callback, arg4);
}

// Host slot +0x88: a stub that returns -1.
s32 HostReadQFileSeekAdd(void) { return -1; }

// Host slot +0x8C: a stub that returns -1.
s32 HostReadQLsnSeekAdd(void) { return -1; }

// Installs this file's loader functions into the archive state (sa_file).
// SaFileModeSet(arg) stores arg at +0x18; with arg == 0 it calls this function,
// otherwise it stores the release versions into the same nine slots:
//
//   slot   host (here)            release            type
//   +0x04  HostFileLsnGet         FileLsnGet         s32 (*)(s32)
//   +0x14  HostFileSizeGet        FileSizeGet        s32 (*)(s32)
//   +0x54  HostPcLoad             func_00162D30      void* (*)(const char*, s32*)
//   +0x58  FileNameGet            func_00164A10      const char* (*)(s32)
//   +0x60  HostPcFileTblSet       func_00162D18      void (*)(const char*)
//   +0x68  HostReadQServe         ReadQServe         void (*)(void)
//   +0x78  HostReadQLsnAdd        ReadQLsnAdd        s32 (*)(s32, void*, u32, void*, ReadQEntry*)
//   +0x88  HostReadQFileSeekAdd   ReadQFileSeekAdd   s32 (*)(s32, s32)
//   +0x8C  HostReadQLsnSeekAdd    ReadQLsnSeekAdd    s32 (*)(s32)
//
// HostReadQLsnAdd just calls the release +0x78, and HostFileSizeGet calls the
// release +0x14 when D_00565DF4 != 0. The +0x88/+0x8C stubs only return -1,
// so their parameters, if any, are not visible here.

void HostModeSet(void) {
    sa_file.unk14 = HostFileSizeGet;
    sa_file.unk58 = FileNameGet;
    sa_file.pc_load = HostPcLoad;
    sa_file.unk68 = HostReadQServe;
    sa_file.pc_file_tbl_set = HostPcFileTblSet;
    sa_file.unk78 = HostReadQLsnAdd;
    sa_file.unk88 = HostReadQFileSeekAdd;
    sa_file.unk8C = HostReadQLsnSeekAdd;
    sa_file.unk4 = HostFileLsnGet;
}
