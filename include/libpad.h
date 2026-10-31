#ifndef LIBPAD_H
#define LIBPAD_H

#include <eetypes.h>

/*
 * EE-side controller library (libpad, ID string "PsIIlibpad  2300"). Only what FFX is shown to use.
 *
 * scePadInit binds IOP servers 0x80000100 and 0x80000101 (ps2sdk's "new" padman IDs). Every RPC is
 * function 1 with a 0x80-byte packet whose first word is the command, in ps2sdk's numbering:
 * 0x01 open, 0x06 main mode, 0x08 act align, 0x0E close, 0x0F end, 0x10 init. The other
 * functions make no RPC: they read the per-port state block (0x70 bytes per port, 0x1C per slot)
 * and follow ps2sdk's logic step for step. Prototypes follow the SDK 2.4.2 header.
 */

/* scePadGetState results */
#define scePadStateExecCmd (5) /* reported while the state is Stable and a request is busy */
#define scePadStateStable (6)
#define scePadStateClosed (99) /* port not open (SDK 2.1+) */

int scePadInit(int mode);                                              /* 0x2DD908 */
int scePadEnd(void);                                                   /* 0x2DDAE8, cmd 0x0F */
int scePadPortOpen(int port, int slot, u_long128* addr);               /* 0x2DDB68, cmd 0x01 */
int scePadPortClose(int port, int slot);                               /* 0x2DDD50, cmd 0x0E */
int scePadRead(int port, int slot, unsigned char* rdata);              /* 0x2DDEB8 */
int scePadGetState(int port, int slot);                                /* 0x2DDF38 */
int scePadGetReqState(int port, int slot);                             /* 0x2DE050 */
int scePadInfoAct(int port, int slot, int actno, int term);            /* 0x2DE0D8 */
int scePadInfoComb(int port, int slot, int listno, int offs);          /* 0x2DE1F8 */
int scePadInfoMode(int port, int slot, int term, int offs);            /* 0x2DE318 */
int scePadSetMainMode(int port, int slot, int offs, int lock);         /* 0x2DE450, cmd 0x06 */
int scePadSetActDirect(int port, int slot, const unsigned char* data); /* 0x2DE508 */
int scePadSetActAlign(int port, int slot, const unsigned char* data);  /* 0x2DE5C0, cmd 0x08 */
int scePadInfoPressMode(int port, int slot);                           /* 0x2DE800 */
int scePadEnterPressMode(int port, int slot);                          /* 0x2DE860 */

#endif
