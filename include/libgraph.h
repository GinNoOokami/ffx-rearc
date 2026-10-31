#ifndef LIBGRAPH_H
#define LIBGRAPH_H

#include <eetypes.h>
#include <eeregs.h>
#include <eestruct.h>

/*
 * GS library (libgraph, ID string "PsIIlibgraph2300"). Only what FFX is shown to use.
 *
 * Most functions match the SDK 2.5.1 graphdev.c objects exactly (built with
 * ee-gcc2.9-991111-01-dtls13010); sceGsSyncPath, sceGsExecStoreImage, sceGsSyncV and
 * sceGsResetGraph are named from their code. Types and prototypes are the same in the SDK 2.0 and
 * 2.5.1 headers. FFX's sceGsSyncV never reads its argument.
 */

typedef struct {
    sceGsTest testa;
    long testaaddr;
    sceGsPrim prim;
    long primaddr;
    sceGsRgbaq rgbaq;
    long rgbaqaddr;
    sceGsXyz xyz2a;
    long xyz2aaddr;
    sceGsXyz xyz2b;
    long xyz2baddr;
    sceGsTest testb;
    long testbaddr;
} sceGsClear __attribute__((aligned(16)));

typedef struct {
    tGS_PMODE pmode;
    tGS_SMODE2 smode2;
    tGS_DISPFB2 dispfb;
    tGS_DISPLAY2 display;
    tGS_BGCOLOR bgcolor;
} sceGsDispEnv;

typedef struct {
    sceGsFrame frame1;
    u_long frame1addr;
    sceGsZbuf zbuf1;
    long zbuf1addr;
    sceGsXyoffset xyoffset1;
    long xyoffset1addr;
    sceGsScissor scissor1;
    long scissor1addr;
    sceGsPrmodecont prmodecont;
    long prmodecontaddr;
    sceGsColclamp colclamp;
    long colclampaddr;
    sceGsDthe dthe;
    long dtheaddr;
    sceGsTest test1;
    long test1addr;
} sceGsDrawEnv1 __attribute__((aligned(16)));

typedef struct {
    sceGsDispEnv disp[2];
    sceGifTag giftag0;
    sceGsDrawEnv1 draw0;
    sceGsClear clear0;
    sceGifTag giftag1;
    sceGsDrawEnv1 draw1;
    sceGsClear clear1;
} sceGsDBuff;

typedef struct {
    sceGsFrame frame2;
    u_long frame2addr;
    sceGsZbuf zbuf2;
    long zbuf2addr;
    sceGsXyoffset xyoffset2;
    long xyoffset2addr;
    sceGsScissor scissor2;
    long scissor2addr;
    sceGsPrmodecont prmodecont;
    long prmodecontaddr;
    sceGsColclamp colclamp;
    long colclampaddr;
    sceGsDthe dthe;
    long dtheaddr;
    sceGsTest test2;
    long test2addr;
} sceGsDrawEnv2 __attribute__((aligned(16)));

typedef struct {
    sceGsDispEnv disp[2];
    sceGifTag giftag0;
    sceGsDrawEnv1 draw01;
    sceGsDrawEnv2 draw02;
    sceGsClear clear0;
    sceGifTag giftag1;
    sceGsDrawEnv1 draw11;
    sceGsDrawEnv2 draw12;
    sceGsClear clear1;
} sceGsDBuffDc;

typedef struct {
    short sceGsInterMode;
    short sceGsOutMode;
    short sceGsFFMode;
    short sceGsVersion;
    volatile int (*sceGsVSCfunc)(int);
    int sceGsVSCid;
} sceGsGParam __attribute__((aligned(16)));

typedef struct {
    sceGifTag giftag0;
    sceGsBitbltbuf bitbltbuf;
    long bitbltbufaddr;
    sceGsTrxpos trxpos;
    long trxposaddr;
    sceGsTrxreg trxreg;
    long trxregaddr;
    sceGsTrxdir trxdir;
    long trxdiraddr;
    sceGifTag giftag1;
} sceGsLoadImage __attribute__((aligned(16)));

typedef struct {
    u_int vifcode[4];
    sceGifTag giftag;
    sceGsBitbltbuf bitbltbuf;
    long bitbltbufaddr;
    sceGsTrxpos trxpos;
    long trxposaddr;
    sceGsTrxreg trxreg;
    long trxregaddr;
    sceGsFinish finish;
    long finishaddr;
    sceGsTrxdir trxdir;
    long trxdiraddr;
} sceGsStoreImage __attribute__((aligned(16)));

void sceGsResetGraph(short mode, short inter, short omode, short ffmode);                                 /* 0x2D9538 */
sceGsGParam* sceGsGetGParam(void);                                                                        /* 0x2D96C8 */
void sceGsResetPath(void);                                                                                /* 0x2D96D8 */
void sceGsSetDefDispEnv(sceGsDispEnv* disp, short psm, short w, short h, short dx, short dy);             /* 0x2D9740 */
int sceGsSetDefDrawEnv(sceGsDrawEnv1* draw, short psm, short w, short h, short ztest, short zpsm);        /* 0x2D9B38 */
void sceGsSetDefDBuff(sceGsDBuff* dp, short psm, short w, short h, short ztest, short zpsm, short clear); /* 0x2D9F10 */
int sceGsSyncV(int mode);                                                                                 /* 0x2DA208 */
int sceGsSyncPath(int mode, u_short timeout);                                                             /* 0x2DA2A0 */
int sceGsSetDefLoadImage(
    sceGsLoadImage* lp, short dbp, short dbw, short dpsm, short x, short y, short w, short h); /* 0x2DA5B8 */
int sceGsSetDefStoreImage(
    sceGsStoreImage* sp, short sbp, short sbw, short spsm, short x, short y, short w, short h);     /* 0x2DA7A0 */
int sceGsExecLoadImage(sceGsLoadImage* lp, u_long128* srcaddr);                                     /* 0x2DA8E0 */
int sceGsExecStoreImage(sceGsStoreImage* sp, u_long128* dstaddr);                                   /* 0x2DAA60 */
int sceGsSetDefDrawEnv2(sceGsDrawEnv2* draw, short psm, short w, short h, short ztest, short zpsm); /* 0x2DB0F0 */
int sceGsSwapDBuffDc(sceGsDBuffDc* db, int id);                                                     /* 0x2DB2D0 */

#endif
