#ifndef LIBDMA_H
#define LIBDMA_H

#include <sys/types.h>
#include <eeregs.h>

/*
 * EE DMA library (libdma, ID string "PsIIlibdma  2300"). Only what FFX is shown to use.
 *
 * sceDmaGetChan, sceDmaReset and four other functions match the SDK 2.5.1 source exactly (built
 * with ee-gcc2.9-991111-01-dtls13010); the rest are named from their code and the header's order.
 * Prototypes follow the SDK 2.4.2 header, whose types are the same as 2.0's.
 */

/* Channel ids: index of sceDmaGetChan's table, whose entries are these channels' registers */
#define SCE_DMA_VIF0 0
#define SCE_DMA_VIF1 1
#define SCE_DMA_GIF 2
#define SCE_DMA_fromIPU 3
#define SCE_DMA_toIPU 4
#define SCE_DMA_SIF0 5
#define SCE_DMA_SIF1 6
#define SCE_DMA_SIF2 7
#define SCE_DMA_fromSPR 8
#define SCE_DMA_toSPR 9

typedef struct _sceDmaTag {
    u_short qwc; /* transfer count */
    u_char mark;
    u_char id;
    struct _sceDmaTag* next;
    u_int p[2];
} sceDmaTag __attribute__((aligned(16)));

/* One channel's registers: the libdma functions use chcr +0x0, madr +0x10, qwc +0x20, tadr +0x30 */
typedef struct {
    tD_CHCR chcr;
    u_int p0[3];
    void* madr;
    u_int p1[3];
    u_int qwc;
    u_int p2[3];
    sceDmaTag* tadr;
    u_int p3[3];
    void* as0;
    u_int p4[3];
    void* as1;
    u_int p5[3];
    u_int p6[4];
    u_int p7[4];
    void* sadr;
    u_int p8[3];
} sceDmaChan;

int sceDmaReset(int mode);                             /* 0x2DB390 */
sceDmaChan* sceDmaGetChan(int id);                     /* 0x2DB368 */
void sceDmaSend(sceDmaChan* d, void* tag);             /* 0x2DB6B8 */
void sceDmaSendN(sceDmaChan* d, void* addr, int size); /* 0x2DB790 */
void sceDmaRecvN(sceDmaChan* d, void* addr, int size); /* 0x2DBA28 */
int sceDmaSync(sceDmaChan* d, int mode, int timeout);  /* 0x2DBC08 */

#endif
