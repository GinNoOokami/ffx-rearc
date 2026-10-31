#ifndef LIBCDVD_H
#define LIBCDVD_H

#include <sys/types.h>

/*
 * EE-side CD/DVD library (libcdvd, ID string "PsIIlibcdvd 2340"). Only what FFX is shown to use.
 *
 * Each function is a SIF RPC client of one IOP server, named from the server it binds and the
 * function number it passes to sceSifCallRpc:
 *   0x80000592 init (sceCdInit), 0x80000593 S-commands, 0x80000595 N-commands,
 *   0x80000597 file search (sceCdSearchFile), 0x8000059A disk ready (sceCdDiskReady).
 * The numbering is ps2sdk's CDVD protocol; prototypes follow the SDK 2.4.2 header.
 *
 * Functions without an RPC call, named from their code:
 *   sceCdSync: mode 0 prints "N cmd wait\n" and waits while the read flag is set or
 *     sceSifCheckStatRpc reports the N-command client busy; other modes return 1 if busy, else 0.
 *   sceCdInitEeCB: creates the callback thread with initPriority = arg 0, stack = arg 1,
 *     stackSize = arg 2 (ThreadParam), or changes its priority if it exists.
 *   sceCdCallback: returns 0 if sceCdSync(1) reports busy, else stores the function and
 *     returns the previous one.
 *   sceCdPOffCallback: stores a function and its argument, returns the previous function
 *     (two arguments: not the one-argument sceCdCallback).
 */

/* sceCdRead mode block: copied byte by byte into the read request; datapattern selects
   the sector size (2048, 2328, 2340 for 0, 1, 2) */
typedef struct {
    u_char trycount;
    u_char spindlctrl;
    u_char datapattern;
    u_char pad;
} sceCdRMode;

/* sceCdSearchFile result: FFX copies 0x24 bytes back, so it has the 2.4.2 flag field
   (2.0's struct ends at date) */
typedef struct {
    u_int lsn;  /* first sector */
    u_int size; /* bytes */
    char name[16];
    u_char date[8];
    u_int flag;
} sceCdlFILE;

/* sceCdReadClock result: 8 bytes copied from the reply */
typedef struct {
    u_char stat;
    u_char second;
    u_char minute;
    u_char hour;
    u_char pad;
    u_char day;
    u_char month;
    u_char year;
} sceCdCLOCK;

/* Command results ("Interrupts" in the SDK); sceCdDiskReady returns SCECdComplete when the
   drive is ready, SCECdNotReady when not */
#define CdlNoIntr 0x00
#define CdlDataReady 0x01
#define SCECdComplete 0x02
#define CdlAcknowledge 0x03 /* reserved */
#define CdlDataEnd 0x04
#define CdlDiskError 0x05
#define SCECdNotReady 0x06

/* sceCdStatus results ("Status Contents" in the SDK) */
#define SCECdStatStop 0x00
#define SCECdStatShellOpen 0x01 /* lid opened since the last check */
#define SCECdStatSpin 0x02
#define SCECdStatRead 0x06
#define SCECdStatPause 0x0A
#define SCECdStatSeek 0x12
#define SCECdStatEmg 0x20 /* error */

int sceCdInit(int init_mode);                                     /* 0x2E10B0, 0x80000592 */
int sceCdInitEeCB(int cb_prio, void* stack_addr, int stack_size); /* 0x2E0580 */
int* sceCdCallback(void (*func)(int));                            /* 0x2E03C8 */
int* sceCdPOffCallback(void (*func)(void*), void* addr);          /* 0x2E0810 */
int sceCdDiskReady(int mode);                                     /* 0x2E1390, 0x8000059A */
int sceCdSearchFile(sceCdlFILE* fp, const char* name);            /* 0x2E0928, 0x80000597 */

/* N-commands (0x80000595) */
int sceCdRead(u_int lbn, u_int sectors, void* buf, sceCdRMode* mode); /* 0x2E1650, fno 0x01 */
int sceCdSeek(u_int lbn);                                             /* 0x2E1830, fno 0x05 */
int sceCdStandby(void);                                               /* 0x2E1900, fno 0x06 */
int sceCdPause(void);                                                 /* 0x2E19B0, fno 0x08 */
int sceCdSync(int mode);                                              /* 0x2E0E30 */

/* S-commands (0x80000593) */
int sceCdReadClock(sceCdCLOCK* rtc); /* 0x2E1DB8, fno 0x01 */
int sceCdGetDiskType(void);          /* 0x2E1A60, fno 0x03 */
int sceCdGetError(void);             /* 0x2E1AF8, fno 0x04 */
int sceCdStatus(void);               /* 0x2E1B90, fno 0x0C */
int sceCdBreak(void);                /* 0x2E1C48, fno 0x16 */
int sceCdPowerOff(int* stat);        /* 0x2E1D00, fno 0x21 */
int sceCdMmode(int media);           /* 0x2E1588, fno 0x22 */

#endif
