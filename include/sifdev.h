#ifndef SIFDEV_H
#define SIFDEV_H

/*
 * EE-side file I/O (libkernl, SDK 2.3.x). Only what FFX is shown to use.
 *
 * Each function is a SIF RPC client of IOP server 0x80000001 (FILEIO):
 * func_002FBF10 binds the client struct D_00794B80 to 0x80000001, and every
 * function below calls sceSifCallRpc on D_00794B80 with a fixed function
 * number, which identifies the operation (FILEIO protocol: 0 open, 1 close,
 * 2 read, 3 write, 4 lseek, 5 ioctl).
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

/* sceIoctl requests */
#define SCE_FS_EXECUTING 0x1 /* *arg = nonzero while an async operation runs */

int sceOpen(const char* filename, int flag, ...);       /* 0x2FC1A0, fno 0 */
int sceClose(int fd);                                   /* 0x2FC428, fno 1 */
int sceRead(int fd, void* buf, int nbyte);              /* 0x2FC7E0, fno 2 */
int sceWrite(int fd, const void* buf, int nbyte);       /* 0x2FCA50, fno 3 */
int sceLseek(int fd, int offset, int where);            /* 0x2FC5A8, fno 4 */
int sceIoctl(int fd, int req, void* arg);               /* 0x2FCD10, fno 5 */

#endif
