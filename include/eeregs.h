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

#endif
