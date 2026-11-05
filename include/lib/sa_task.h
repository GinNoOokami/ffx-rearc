#ifndef SA_TASK_H
#define SA_TASK_H

#include "common.h"

/* Task buffer: func_00171A88(n) bytes from func_00171C78(n), set up by func_00171AB0 and registered in
 * D_005979B8[i].
 *
 *   +0x00           +0x28       unkC             unk10       unk14
 *   +---------------+-----------+----------------+-----------+-----------+----------------+
 *   | SaTask header | pad to 16 | n thread slots | save area | n entries | rest, to a     |
 *   | (0x28)        |           | 0x1310 each    | 0x370     | 0x20 each | multiple of 64 |
 *   +---------------+-----------+----------------+-----------+-----------+----------------+
 *
 * unk0/unk4/unk8 are slot i's entries of the tables D_0030E568/D_0030E588/D_0030E548. */
typedef struct {
    /* 0x00 */ void (*unk0)(void);                      // runs the task (func_00171918..func_00171950)
    /* 0x04 */ void (*unk4)(void);                      // removes it (func_001719A0..func_001719D8)
    /* 0x08 */ s32 (*unk8)(void (*fn)(void), s32, s32); // adds a thread running fn (func_001716F8..func_00171848)
    /* 0x0C */ s32 unkC;                                // thread slots, 16-aligned after the header
    /* 0x10 */ s32 unk10;                               // register save area, right after the slots
    /* 0x14 */ s32 unk14;                               // per-slot 0x20 entries, after the save area
    /* 0x18 */ s32 unk18;                               // n, number of thread slots
    /* 0x1C */ s32 unk1C;                               // incremented each time func_00171878 runs the task
    /* 0x20 */ s32 unk20;                               // index i in D_005979B8
    /* 0x24 */ s32 unk24;                               // nonzero: freed on removal (func_00171958)
} SaTask;                                               // size: 0x28

SaTask* func_00171C78(/*?*/ s32);

// Variant getters (`jr ra; li v0, N`): nine values, seven variants each, picked by libmscd.c's
// func_001625F8 and its siblings
s32 func_00171CC8(void);
s32 func_00171CD0(void);
s32 func_00171CD8(void);
s32 func_00171CE0(void);
s32 func_00171CE8(void);
s32 func_00171CF0(void);
s32 func_00171CF8(void);
s32 func_00171D00(void);
s32 func_00171D08(void);
s32 func_00171D10(void);
s32 func_00171D18(void);
s32 func_00171D20(void);
s32 func_00171D28(void);
s32 func_00171D30(void);
s32 func_00171D38(void);
s32 func_00171D40(void);
s32 func_00171D48(void);
s32 func_00171D50(void);
s32 func_00171D58(void);
s32 func_00171D60(void);
s32 func_00171D68(void);
s32 func_00171D70(void);
s32 func_00171D78(void);
s32 func_00171D80(void);
s32 func_00171D88(void);
s32 func_00171D90(void);
s32 func_00171D98(void);
s32 func_00171DA0(void);
s32 func_00171DA8(void);
s32 func_00171DB0(void);
s32 func_00171DB8(void);
s32 func_00171DC0(void);
s32 func_00171DC8(void);
s32 func_00171DD0(void);
s32 func_00171DD8(void);
s32 func_00171DE0(void);
s32 func_00171DE8(void);
s32 func_00171DF0(void);
s32 func_00171DF8(void);
s32 func_00171E00(void);
s32 func_00171E08(void);
s32 func_00171E10(void);
s32 func_00171E18(void);
s32 func_00171E20(void);
s32 func_00171E28(void);
s32 func_00171E30(void);
s32 func_00171E38(void);
s32 func_00171E40(void);
s32 func_00171E48(void);
s32 func_00171E50(void);
s32 func_00171E58(void);
s32 func_00171E60(void);
s32 func_00171E68(void);
s32 func_00171E70(void);
s32 func_00171E78(void);
s32 func_00171E80(void);
s32 func_00171E88(void);
s32 func_00171E90(void);
s32 func_00171E98(void);
s32 func_00171EA0(void);
s32 func_00171EA8(void);
s32 func_00171EB0(void);
s32 func_00171EB8(void);

#endif
