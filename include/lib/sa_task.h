#ifndef SA_TASK_H
#define SA_TASK_H

#include "common.h"

/* Task buffer: TaskBuffSize(n) bytes from TaskBuffAlloc(n), set up by TaskBuffInit and registered in
 * task_group[group].
 *
 *   +0x00           +0x28       slots            save_area   queue
 *   +---------------+-----------+----------------+-----------+---------------+-----------+----------------+
 *   | SaTask header | pad to 16 | n TaskSlots    | save area | 16 buckets    | n nodes   | rest, to a     |
 *   | (0x28)        |           | 0x1310 each    | 0x370     | {head, tail}, | 0x20 each | multiple of 64 |
 *   |               |           |                |           | 0x200         |           |                |
 *   +---------------+-----------+----------------+-----------+---------------+-----------+----------------+
 *
 * run/remove/create are this group's entries of task_run_tbl/task_remove_tbl/task_create_tbl. */
// A thread's entry point, started by TaskCreate
typedef void (*TaskThreadFunc)(void);

typedef struct {
    /* 0x00 */ void (*run)(void);                          // TaskRun0..TaskRun7
    /* 0x04 */ void (*remove)(void);                       // TaskRemove0..TaskRemove7
    /* 0x08 */ s32 (*create)(TaskThreadFunc fn, s32, s32); // TaskCreate0..TaskCreate7: adds a thread running fn
    /* 0x0C */ s32 slots;                                  // 16-aligned after the header
    /* 0x10 */ s32 save_area;                              // register save area, right after the slots
    /* 0x14 */ s32 queue;                                  // run queue, after the save area
    /* 0x18 */ s32 slot_num;
    /* 0x1C */ s32 ticks; // +1 per TaskRun
    /* 0x20 */ s32 group; // index in task_group
    /* 0x24 */ s32 owned; // nonzero: TaskRemove frees it
} SaTask;                 // size: 0x28

SaTask* TaskBuffAlloc(s32 n);

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
