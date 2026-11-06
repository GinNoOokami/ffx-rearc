#include "common.h"

#include "extern.h"
#include "lib/sa_task.h"
#include "mem.h"

#define TASK_GRUPE_MAX 8 // This is the real name from the original code

// A thread: its saved context (read and written by the hand-written task_*_asm code) and its stack
typedef struct {
    /* 0x000 */ s32 args[4];    // the thread function's a0-a3
    /* 0x010 */ u128 unk10;     // task_create_asm's 2nd argument
    /* 0x020 */ u128 bucket;    // task_create_asm's 3rd argument: the run-queue bucket
    /* 0x030 */ u128 regs[13];  // s0-s7, ra, sp, fp, gp, k1; s0 = the thread function, ra = 0: free slot
    /* 0x100 */ u128 fregs[32]; // f0-f31
    /* 0x300 */ u128 wait;      // runs before it resumes: task_wait(n) sets n
    /* 0x310 */ u8 stack[0x1000];
} TaskSlot; // size: 0x1310

// Run-queue entry, one per slot: rebuilt from the in-use slots on every run
typedef struct {
    /* 0x00 */ u128 slot; // TaskSlot*
    /* 0x10 */ u128 next; // TaskQueueNode*, 0 at the bucket tail
} TaskQueueNode;          // size: 0x20

typedef struct {
    u8 pad[0x5C8];
} UnkSize5C8; // size: 0x5C8

extern s32 (*task_create_tbl[TASK_GRUPE_MAX])(TaskThreadFunc fn, s32 unk, s32 bucket);
extern void (*task_run_tbl[TASK_GRUPE_MAX])(void);
extern void (*task_remove_tbl[TASK_GRUPE_MAX])(void);
extern SaTask* task_group[TASK_GRUPE_MAX];

void TaskGroupInit(void) {
    s32 i;

    for (i = 0; i < TASK_GRUPE_MAX; i++) {
        task_group[i] = NULL;
    }
}

s32 TaskCreate(s32 group, TaskThreadFunc fn, s32 unk, s32 bucket) {
    SaTask* task = task_group[group];
    s32 prev_save_area = cur_save_area;
    s32 prev_slot_num = cur_slot_num;
    s32 slot;

    cur_slots = task->slots;
    cur_save_area = task->save_area;
    cur_queue = task->queue;
    cur_slot_num = task->slot_num;

    slot = task_create_asm(fn, unk, bucket);

    cur_save_area = prev_save_area;
    cur_slot_num = prev_slot_num;

    return slot;
}

s32 TaskCreate0(TaskThreadFunc fn, s32 unk, s32 bucket) { return TaskCreate(0, fn, unk, bucket); }

s32 TaskCreate1(TaskThreadFunc fn, s32 unk, s32 bucket) { return TaskCreate(1, fn, unk, bucket); }

s32 TaskCreate2(TaskThreadFunc fn, s32 unk, s32 bucket) { return TaskCreate(2, fn, unk, bucket); }

s32 TaskCreate3(TaskThreadFunc fn, s32 unk, s32 bucket) { return TaskCreate(3, fn, unk, bucket); }

s32 TaskCreate4(TaskThreadFunc fn, s32 unk, s32 bucket) { return TaskCreate(4, fn, unk, bucket); }

s32 TaskCreate5(TaskThreadFunc fn, s32 unk, s32 bucket) { return TaskCreate(5, fn, unk, bucket); }

s32 TaskCreate6(TaskThreadFunc fn, s32 unk, s32 bucket) { return TaskCreate(6, fn, unk, bucket); }

s32 TaskCreate7(TaskThreadFunc fn, s32 unk, s32 bucket) { return TaskCreate(7, fn, unk, bucket); }

void TaskRun(s32 group) {
    s32 prev_save_area = cur_save_area;
    s32 prev_slot_num = cur_slot_num;

    cur_task = task_group[group];
    cur_slots = cur_task->slots;
    cur_save_area = cur_task->save_area;
    cur_queue = cur_task->queue;
    cur_slot_num = cur_task->slot_num;
    cur_task->ticks++;

    task_run_asm();

    cur_save_area = prev_save_area;
    cur_slot_num = prev_slot_num;
}

void TaskRun0(void) { TaskRun(0); }

void TaskRun1(void) { TaskRun(1); }

void TaskRun2(void) { TaskRun(2); }

void TaskRun3(void) { TaskRun(3); }

void TaskRun4(void) { TaskRun(4); }

void TaskRun5(void) { TaskRun(5); }

void TaskRun6(void) { TaskRun(6); }

void TaskRun7(void) { TaskRun(7); }

void TaskRemove(s32 group) {
    SaTask** slot = &task_group[group];
    SaTask* task = *slot;

    if (task->owned != 0) {
        func_0011D590(task, 10);
    }
    *slot = NULL;
}

void TaskRemove0(void) { TaskRemove(0); }

void TaskRemove1(void) { TaskRemove(1); }

void TaskRemove2(void) { TaskRemove(2); }

void TaskRemove3(void) { TaskRemove(3); }

void TaskRemove4(void) { TaskRemove(4); }

void TaskRemove5(void) { TaskRemove(5); }

void TaskRemove6(void) { TaskRemove(6); }

void TaskRemove7(void) { TaskRemove(7); }

s32 TaskBuffCreate(SaTask* task, TaskThreadFunc fn, s32 unk, s32 bucket) {
    s32 prev_save_area = cur_save_area;
    s32 prev_slot_num = cur_slot_num;
    s32 slot;

    cur_slots = task->slots;
    cur_save_area = task->save_area;
    cur_queue = task->queue;
    cur_slot_num = task->slot_num;

    slot = task_create_asm(fn, unk, bucket);
    cur_save_area = prev_save_area;
    cur_slot_num = prev_slot_num;
    return slot;
}

s32 func_00171A68(TaskThreadFunc fn, s32 unk, s32 bucket) { return task_create_asm(fn, unk, bucket); }

// Returns size needed for a task with n thread slots
s32 TaskBuffSize(s32 n) { return ALIGN_64(sizeof(UnkSize5C8) + n * sizeof(TaskSlot) + n * sizeof(TaskQueueNode)); }

SaTask* TaskBuffInit(s32 n, SaTask* task, s32 owned) {
    s32 prev_save_area = cur_save_area;
    s32 prev_slot_num = cur_slot_num;
    s32 slotsSize;
    s32 slots;
    s32 pad;
    s32 i;

    slotsSize = n * sizeof(TaskSlot);
    slots = (s32)(task + 1);

    // round slots up to a multiple of 16
    pad = slots & 0xF;
    if (pad != 0) {
        pad -= 0x10;
        slots -= pad;
    }
    task->slots = slots;

    for (i = 0; i < TASK_GRUPE_MAX; i++) {
        if (task_group[i] == NULL) {
            break;
        }
    }

    if (i == TASK_GRUPE_MAX) {
        dbgAssert("i!=TASK_GRUPE_MAX", "sa_task.c", 254);
    }
    task_group[i] = task;

    task->group = i;

    cur_slots = task->slots = slots;
    cur_save_area = task->save_area = slots + slotsSize;
    cur_queue = task->queue = slots + slotsSize + 0x370;
    cur_slot_num = task->slot_num = n;

    task->create = task_create_tbl[i];
    task->run = task_run_tbl[i];
    task->remove = task_remove_tbl[i];
    task->owned = owned;
    task->ticks = 0;

    task_init_asm();

    cur_save_area = prev_save_area;
    cur_slot_num = prev_slot_num;

    return task;
}

SaTask* TaskBuffAlloc(s32 n) {
    s32 size = TaskBuffSize(n);
    return TaskBuffInit(n, func_0011D370(size, 10, "sa_task.c", 282), 1);
}
