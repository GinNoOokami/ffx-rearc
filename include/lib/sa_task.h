#ifndef SA_TASK_H
#define SA_TASK_H

#include "common.h"

/* Task object from sa_task.c (read_task_buff): a buffer allocated and set up by
 * func_00171C78(1) in SaFileReadInit; stub, only slot 0 so far.
 * Known from its fill: +0x0/+0x4/+0x8 are per-group function slots, +0x8 starts a task. */
typedef struct {
    /* 0x00 */ void (*unk0)(void); // called with no args, then func_002D3C80(0) in the wait loop
    /* 0x04 */ s32 unk4;
    /* 0x08 */ void (*unk8)(void (*fn)(void), s32, s32);
} SaTask;

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
