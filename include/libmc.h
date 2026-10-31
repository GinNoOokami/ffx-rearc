#ifndef LIBMC_H
#define LIBMC_H

/*
 * EE-side memory card library (libmc, ID string "PsIIlibmc   2320"). Only what FFX is shown to use.
 *
 * Each function except sceMcMkdir and sceMcSync is a SIF RPC client of IOP server 0x80000400,
 * named from the function number it passes to sceSifCallRpc (ps2sdk's XMCSERV numbering) and,
 * independently, from the command code it stores for sceMcSync, which equals the SDK's
 * sceMcFuncNo* value in every case. sceMcMkdir calls sceMcOpen with mode 0x40 and stores
 * sceMcFuncNoMkdir. Prototypes follow the SDK 2.4.2 header.
 */

/* Command codes: stored by each function, reported by sceMcSync */
#define sceMcFuncNoCardInfo 1
#define sceMcFuncNoOpen 2
#define sceMcFuncNoClose 3
#define sceMcFuncNoSeek 4
#define sceMcFuncNoRead 5
#define sceMcFuncNoWrite 6
#define sceMcFuncNoFlush 10
#define sceMcFuncNoMkdir 11
#define sceMcFuncNoChDir 12
#define sceMcFuncNoGetDir 13
#define sceMcFuncNoFileInfo 14
#define sceMcFuncNoDelete 15
#define sceMcFuncNoFormat 16
#define sceMcFuncNoUnformat 17
#define sceMcFuncNoEntSpace 18
#define sceMcFuncNoRename 19

/* Directory entry: sceMcSetFileInfo copies 0x40 bytes of it. Fields after AttrFile follow the
   SDK 2.1+ header (Reserve2 + PdaAplNo; 2.0 had Reserve2[2]), not checked against FFX. */
typedef struct {
    struct {
        unsigned char Resv2, Sec, Min, Hour;
        unsigned char Day, Month;
        unsigned short Year;
    } _Create;
    struct {
        unsigned char Resv2, Sec, Min, Hour;
        unsigned char Day, Month;
        unsigned short Year;
    } _Modify;
    unsigned FileSizeByte;
    unsigned short AttrFile;
    unsigned short Reserve1;
    unsigned Reserve2;
    unsigned PdaAplNo;
    unsigned char EntryName[32];
} sceMcTblGetDir __attribute__((aligned(64)));

/* Parameter names as in the SDK header: none */
int sceMcInit(void);                                                    /* 0x2DEB80, fno 0xFE */
int sceMcGetInfo(int, int, int*, int*, int*);                           /* 0x2DF6B8, fno 0x01 */
int sceMcOpen(int, int, const char*, int);                              /* 0x2DEEE0, fno 0x02 */
int sceMcClose(int);                                                    /* 0x2DF050, fno 0x03 */
int sceMcSeek(int, int, int);                                           /* 0x2DF108, fno 0x04 */
int sceMcRead(int, void*, int);                                         /* 0x2DF270, fno 0x05 */
int sceMcWrite(int, void*, int);                                        /* 0x2DF390, fno 0x06 */
int sceMcFlush(int);                                                    /* 0x2DFD60, fno 0x0A */
int sceMcMkdir(int, int, const char*);                                  /* 0x2DF018 */
int sceMcChdir(int, int, const char*, char*);                           /* 0x2DFA20, fno 0x0C */
int sceMcGetDir(int, int, const char*, unsigned, int, sceMcTblGetDir*); /* 0x2DF840, fno 0x0D */
int sceMcSetFileInfo(int, int, const char*, const char*, unsigned);     /* 0x2DFE18, fno 0x0E */
int sceMcDelete(int, int, const char*);                                 /* 0x2DFC38, fno 0x0F */
int sceMcFormat(int, int);                                              /* 0x2DFB70, fno 0x10 */
int sceMcUnformat(int, int);                                            /* 0x2E0158, fno 0x11 */
int sceMcGetEntSpace(int, int, const char*);                            /* 0x2E0220, fno 0x12 */
int sceMcRename(int, int, const char*, const char*);                    /* 0x2DFFF0, fno 0x0E, flag 0x10 */
int sceMcSync(int, int*, int*);                                         /* 0x2DF580 */

#endif
