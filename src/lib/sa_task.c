#include "common.h"

#include "extern.h"
#include "lib/sa_task.h"
#include "mem.h"

#define TASK_GRUPE_MAX 8

typedef struct {
    u8 pad[0x1310];
} UnkSize1310; // size: 0x1310

typedef struct {
    u8 pad[0x20];
} UnkSize20; // size: 0x20

typedef struct {
    u8 pad[0x5C8];
} UnkSize5C8; // size: 0x5C8

extern s32 (*task_spawn_tbl[TASK_GRUPE_MAX])(void (*fn)(void), s32, s32);
extern void (*task_run_tbl[TASK_GRUPE_MAX])(void);
extern void (*task_remove_tbl[TASK_GRUPE_MAX])(void);
extern char D_0056BD90[]; // "sa_task.c"
extern SaTask* task_group[TASK_GRUPE_MAX];

void TaskGroupInit(void) {
    s32 i;

    for (i = 0; i < TASK_GRUPE_MAX; i++) {
        task_group[i] = NULL;
    }
}

s32 TaskSpawn(s32 group, s32 arg1, s32 arg2, s32 arg3) {
    SaTask* task = task_group[group];
    s32 d0057C718 = D_0057C718;
    s32 d0057C720 = D_0057C720;
    s32 result;

    D_0057C724 = task->slots;
    D_0057C718 = task->save_area;
    D_0057C728 = task->unk14;
    D_0057C720 = task->slot_num;

    result = func_00170FE4(arg1, arg2, arg3);

    D_0057C718 = d0057C718;
    D_0057C720 = d0057C720;

    return result;
}

s32 TaskSpawn0(s32 arg0, s32 arg1, s32 arg2) { return TaskSpawn(0, arg0, arg1, arg2); }

s32 TaskSpawn1(s32 arg0, s32 arg1, s32 arg2) { return TaskSpawn(1, arg0, arg1, arg2); }

s32 TaskSpawn2(s32 arg0, s32 arg1, s32 arg2) { return TaskSpawn(2, arg0, arg1, arg2); }

s32 TaskSpawn3(s32 arg0, s32 arg1, s32 arg2) { return TaskSpawn(3, arg0, arg1, arg2); }

s32 TaskSpawn4(s32 arg0, s32 arg1, s32 arg2) { return TaskSpawn(4, arg0, arg1, arg2); }

s32 TaskSpawn5(s32 arg0, s32 arg1, s32 arg2) { return TaskSpawn(5, arg0, arg1, arg2); }

s32 TaskSpawn6(s32 arg0, s32 arg1, s32 arg2) { return TaskSpawn(6, arg0, arg1, arg2); }

s32 TaskSpawn7(s32 arg0, s32 arg1, s32 arg2) { return TaskSpawn(7, arg0, arg1, arg2); }

void TaskRun(s32 group) {
    s32 d0057C718 = D_0057C718;
    s32 d0057C720 = D_0057C720;

    cur_task = task_group[group];
    D_0057C724 = cur_task->slots;
    D_0057C718 = cur_task->save_area;
    D_0057C728 = cur_task->unk14;
    D_0057C720 = cur_task->slot_num;
    cur_task->ticks++;

    func_001710B4();

    D_0057C718 = d0057C718;
    D_0057C720 = d0057C720;
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

void TaskRemove0() { TaskRemove(0); }

void TaskRemove1() { TaskRemove(1); }

void TaskRemove2() { TaskRemove(2); }

void TaskRemove3() { TaskRemove(3); }

void TaskRemove4() { TaskRemove(4); }

void TaskRemove5() { TaskRemove(5); }

void TaskRemove6() { TaskRemove(6); }

void TaskRemove7() { TaskRemove(7); }

s32 TaskBuffSpawn(SaTask* task, s32 arg1, s32 arg2, s32 arg3) {
    s32 d0057C718 = D_0057C718;
    s32 d0057C720 = D_0057C720;
    s32 result;

    D_0057C724 = task->slots;
    D_0057C718 = task->save_area;
    D_0057C728 = task->unk14;
    D_0057C720 = task->slot_num;

    result = func_00170FE4(arg1, arg2, arg3);
    D_0057C718 = d0057C718;
    D_0057C720 = d0057C720;
    return result;
}

s32 func_00171A68(s32 arg0, s32 arg1, s32 arg2) { return func_00170FE4(arg0, arg1, arg2); }

// Returns size needed for a task with n thread slots
s32 TaskBuffSize(s32 n) { return ALIGN_64(sizeof(UnkSize5C8) + n * sizeof(UnkSize1310) + n * sizeof(UnkSize20)); }

SaTask* TaskBuffInit(s32 n, SaTask* task, s32 owned) {
    s32 saved718 = D_0057C718;
    s32 saved720 = D_0057C720;
    s32 slotsSize;
    s32 slots;
    s32 pad;
    s32 i;

    slotsSize = n * sizeof(UnkSize1310);
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
        dbgAssert("i!=TASK_GRUPE_MAX", D_0056BD90, 0xFE);
    }
    task_group[i] = task;

    task->group = i;

    D_0057C724 = task->slots = slots;
    D_0057C718 = task->save_area = slots + slotsSize;
    D_0057C728 = task->unk14 = slots + slotsSize + 0x370;
    D_0057C720 = task->slot_num = n;

    task->spawn = task_spawn_tbl[i];
    task->run = task_run_tbl[i];
    task->remove = task_remove_tbl[i];
    task->owned = owned;
    task->ticks = 0;

    func_00170F9C();

    D_0057C718 = saved718;
    D_0057C720 = saved720;

    return task;
}

SaTask* TaskBuffAlloc(s32 n) {
    s32 size = TaskBuffSize(n);
    return TaskBuffInit(n, func_0011D370(size, 10, D_0056BD90, 0x11A), 1);
}

s32 func_00171CC8(void) { return 0xF; }

s32 func_00171CD0(void) { return 0x117; }

s32 func_00171CD8(void) { return 0x118; }

s32 func_00171CE0(void) { return 0x158; }

s32 func_00171CE8(void) { return 0x20000; }

s32 func_00171CF0(void) { return 0x30; }

s32 func_00171CF8(void) { return 0x82; }

s32 func_00171D00(void) { return 0x2; }

s32 func_00171D08(void) { return 0x10; }

s32 func_00171D10(void) { return 0xF; }

s32 func_00171D18(void) { return 0x117; }

s32 func_00171D20(void) { return 0x118; }

s32 func_00171D28(void) { return 0x158; }

s32 func_00171D30(void) { return 0x20000; }

s32 func_00171D38(void) { return 0x30; }

s32 func_00171D40(void) { return 0x82; }

s32 func_00171D48(void) { return 0x2; }

s32 func_00171D50(void) { return 0x10; }

s32 func_00171D58(void) { return 0xF; }

s32 func_00171D60(void) { return 0x117; }

s32 func_00171D68(void) { return 0x118; }

s32 func_00171D70(void) { return 0x158; }

s32 func_00171D78(void) { return 0x20000; }

s32 func_00171D80(void) { return 0x30; }

s32 func_00171D88(void) { return 0x82; }

s32 func_00171D90(void) { return 0x2; }

s32 func_00171D98(void) { return 0x10; }

s32 func_00171DA0(void) { return 0xF; }

s32 func_00171DA8(void) { return 0x117; }

s32 func_00171DB0(void) { return 0x118; }

s32 func_00171DB8(void) { return 0x158; }

s32 func_00171DC0(void) { return 0x20000; }

s32 func_00171DC8(void) { return 0x30; }

s32 func_00171DD0(void) { return 0x82; }

s32 func_00171DD8(void) { return 0x2; }

s32 func_00171DE0(void) { return 0x10; }

s32 func_00171DE8(void) { return 0xF; }

s32 func_00171DF0(void) { return 0x117; }

s32 func_00171DF8(void) { return 0x118; }

s32 func_00171E00(void) { return 0x158; }

s32 func_00171E08(void) { return 0x20000; }

s32 func_00171E10(void) { return 0x30; }

s32 func_00171E18(void) { return 0x82; }

s32 func_00171E20(void) { return 0x2; }

s32 func_00171E28(void) { return 0x10; }

s32 func_00171E30(void) { return 0xF; }

s32 func_00171E38(void) { return 0x117; }

s32 func_00171E40(void) { return 0x118; }

s32 func_00171E48(void) { return 0x158; }

s32 func_00171E50(void) { return 0x20000; }

s32 func_00171E58(void) { return 0x30; }

s32 func_00171E60(void) { return 0x82; }

s32 func_00171E68(void) { return 0x2; }

s32 func_00171E70(void) { return 0x10; }

s32 func_00171E78(void) { return 0xF; }

s32 func_00171E80(void) { return 0x117; }

s32 func_00171E88(void) { return 0x118; }

s32 func_00171E90(void) { return 0x158; }

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E98);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171EA0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171EA8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171EB0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171EB8);

INCLUDE_RODATA("asm/jp_intl/nonmatchings/lib/sa_task", D_0056BD90);
