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

/* Hand-written task code (0x170BA4-0x171628, before sa_task.c) */
// The current group's SaTask fields, for the hand-written code: TaskCreate, TaskBuffCreate, TaskRun
// and TaskBuffInit copy them from the SaTask before calling it
extern s32 cur_save_area;
extern SaTask* cur_task;
extern s32 cur_slot_num;
extern s32 cur_slots;
extern s32 cur_queue;

/* Hand-written. Clears the +0xB0 quad (task_create_asm's free-slot test) of each of cur_slot_num slots
 * from cur_slots. */
void task_init_asm(void);

/* Hand-written. Scans cur_slot_num slots of 0x1310 bytes from cur_slots for one whose +0xB0 quad is
 * zero (spins forever in func_0017101C if none), then sets it up to start at func_00171070:
 * a0/a1/a2 at +0x30/+0x10/+0x20, gp at +0xE0, fp at +0xD0, sp = slot + 0x12F0 at +0xC0, +0x300 = 1.
 * Returns the slot. */
s32 task_create_asm(TaskThreadFunc fn, s32 unk, s32 bucket);
/* Hand-written. Rebuilds the run queue (cur_queue) from the in-use slots, resumes each slot whose wait
 * count reaches 0, bucket by bucket, and returns how many it resumed. */
s32 task_run_asm(void);
void task_wait(s32);

SaTask* TaskBuffAlloc(s32 n);

#endif
