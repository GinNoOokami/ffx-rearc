#ifndef EEREGS_H
#define EEREGS_H

/* EE register layouts, as the SDK's eeregs.h defines them. Only what FFX is shown to use. */

/* Dn_CHCR. libdma's code sets DIR (bit 0), MOD (bits 2-3) and STR (bit 8). */
typedef struct {
    unsigned DIR : 1; /* direction: 1 = from memory */
    unsigned p0 : 1;
    unsigned MOD : 2; /* mode: 0 normal, 1 chain, 2 interleave */
    unsigned ASP : 2; /* address stack pointer */
    unsigned TTE : 1; /* tag transfer enable */
    unsigned TIE : 1; /* tag interrupt enable */
    unsigned STR : 1; /* start */
    unsigned p1 : 7;
    unsigned TAG : 16; /* DMA tag */
} tD_CHCR;

/* GS privileged registers used by sceGsDispEnv (identical in SDK 2.0 and 2.5.1) */

typedef struct {
    unsigned EN1 : 1;
    unsigned EN2 : 1;
    unsigned CRTMD : 3;
    unsigned MMOD : 1;
    unsigned AMOD : 1;
    unsigned SLBG : 1;
    unsigned ALP : 8;
    unsigned p0 : 16;
    unsigned int p1;
} tGS_PMODE;

typedef struct {
    unsigned INT : 1;
    unsigned FFMD : 1;
    unsigned DPMS : 2;
    unsigned p0 : 28;
    unsigned int p1;
} tGS_SMODE2;

typedef struct {
    unsigned FBP : 9;
    unsigned FBW : 6;
    unsigned PSM : 5;
    unsigned p0 : 12;
    unsigned DBX : 11;
    unsigned DBY : 11;
    unsigned p1 : 10;
} tGS_DISPFB2;

typedef struct {
    unsigned DX : 12;
    unsigned DY : 11;
    unsigned MAGH : 4;
    unsigned MAGV : 2;
    unsigned p0 : 3;
    unsigned DW : 12;
    unsigned DH : 11;
    unsigned p1 : 9;
} tGS_DISPLAY2;

typedef struct {
    unsigned R : 8;
    unsigned G : 8;
    unsigned B : 8;
    unsigned p0 : 8;
    unsigned int p1;
} tGS_BGCOLOR;

#endif
