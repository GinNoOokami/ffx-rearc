#ifndef SIFDEV_H
#define SIFDEV_H

/*
 * EE-side file I/O (libkernl, SDK 2.3.x). Only what FFX is shown to use.
 *
 * Each function is a SIF RPC client of IOP server 0x80000001 (FILEIO):
 * func_002FBF10 binds the client struct D_00794B80 to 0x80000001, and every
 * function below calls sceSifCallRpc on D_00794B80 with a fixed function
 * number, which identifies the operation (FILEIO protocol: 0 open, 1 close,
 * 2 read, 3 write, 4 lseek, 5 ioctl; the rest as decoded from the disc's IOP
 * FILEIO module, see each declaration).
 *
 * Prototypes follow the SDK 2.4.2 header; FFX's sceOpen saves a2-t3 in its
 * prologue (varargs), which is the 2.4.2 form, not 2.0's two-argument one.
 */

/* sceOpen flags */
#define SCE_RDONLY 0x0001
#define SCE_NOWAIT 0x8000 /* asynchronous: poll with sceIoctl(SCE_FS_EXECUTING) */

/* sceLseek origins */
#define SCE_SEEK_SET 0
#define SCE_SEEK_END 2

/* Layouts from the SDK 2.4.2 header; sizes not yet checked against FFX */
struct sce_stat {
    unsigned int st_mode;
    unsigned int st_attr;
    unsigned int st_size;
    unsigned char st_ctime[8];
    unsigned char st_atime[8];
    unsigned char st_mtime[8];
    unsigned int st_hisize;
    unsigned int st_private[6];
};

struct sce_dirent {
    struct sce_stat d_stat;
    char d_name[256];
    void* d_private;
};

/* sceIoctl requests */
#define SCE_FS_EXECUTING 0x1 /* *arg = nonzero while an async operation runs */

int sceOpen(const char* filename, int flag, ...); /* 0x2FC1A0, fno 0 */
int sceClose(int fd);                             /* 0x2FC428, fno 1 */
int sceRead(int fd, void* buf, int nbyte);        /* 0x2FC7E0, fno 2 */
int sceWrite(int fd, const void* buf, int nbyte); /* 0x2FCA50, fno 3 */
int sceLseek(int fd, int offset, int where);      /* 0x2FC5A8, fno 4 */
int sceIoctl(int fd, int req, void* arg);         /* 0x2FCD10, fno 5 */

int sceIoctl2(
    int fd, int request, void* argp, unsigned int arglen, void* bufp, unsigned int buflen);   /* 0x2FD060, fno 0x1A */
int sceRemove(const char* filename);                                                          /* 0x2FD3E0, fno 0x06 */
int sceFormat(const char* path, const char* blkdevname, void* arg, int arglen);               /* 0x2FD5D0, fno 0x0E */
int sceDopen(const char* dirname);                                                            /* 0x2FD980, fno 0x09 */
int sceDclose(int fd);                                                                        /* 0x2FDA48, fno 0x0A */
int sceDread(int fd, struct sce_dirent* buf);                                                 /* 0x2FDBB0, fno 0x0B */
int sceGetstat(const char* name, struct sce_stat* buf);                                       /* 0x2FDD08, fno 0x0C */
int sceSync(const char* path, int flag);                                                      /* 0x2FE2F8, fno 0x13 */
int sceMount(const char* fsdevname, const char* blkdevname, int flag, void* arg, int arglen); /* 0x2FE490, fno 0x14 */
int sceUmount(const char* name);                                                              /* 0x2FE700, fno 0x15 */
long sceLseek64(int fd, long offset, int whence);                                             /* 0x2FE720, fno 0x16 */
int sceDevctl(const char* devname, int cmd, void* arg, unsigned int arglen, void* bufp,
              unsigned int buflen); /* 0x2FE958, fno 0x17 */

/* IOP modules and reset (module loading: IOP server 0x80000006, numbers decoded from the disc's
   LOADFILE module) */
int sceSifLoadModule(const char* filename, int args, const char* argp); /* 0x2FF930, fno 0 */
int sceSifRebootIop(const char* img);                                   /* 0x2FFE30 */
int sceSifSyncIop(void);                                                /* 0x2FFDF8 */

#endif
