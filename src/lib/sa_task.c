#include "common.h"

#include "extern.h"
#include "lib/sa_task.h"
#include "mem.h"

#define TASK_GRUPE_MAX 8

extern SaTask* D_005979B8[TASK_GRUPE_MAX]; // SaTask* g_TaskBuffer[8]

void func_00171628(void) {
    s32 i;

    for (i = 0; i < TASK_GRUPE_MAX; i++) {
        D_005979B8[i] = NULL;
    }
}

s32 func_00171660(s32 index, s32 arg1, s32 arg2, s32 arg3) {
    SaTask* task = D_005979B8[index];
    s32 d0057C718 = D_0057C718;
    s32 d0057C720 = D_0057C720;
    s32 result;

    D_0057C724 = task->unkC;
    D_0057C718 = task->unk10;
    D_0057C728 = task->unk14;
    D_0057C720 = task->unk18;

    result = func_00170FE4(arg1, arg2, arg3);

    D_0057C718 = d0057C718;
    D_0057C720 = d0057C720;

    return result;
}

s32 func_001716F8(s32 arg0, s32 arg1, s32 arg2) { return func_00171660(0, arg0, arg1, arg2); }

s32 func_00171728(s32 arg0, s32 arg1, s32 arg2) { return func_00171660(1, arg0, arg1, arg2); }

s32 func_00171758(s32 arg0, s32 arg1, s32 arg2) { return func_00171660(2, arg0, arg1, arg2); }

s32 func_00171788(s32 arg0, s32 arg1, s32 arg2) { return func_00171660(3, arg0, arg1, arg2); }

s32 func_001717B8(s32 arg0, s32 arg1, s32 arg2) { return func_00171660(4, arg0, arg1, arg2); }

s32 func_001717E8(s32 arg0, s32 arg1, s32 arg2) { return func_00171660(5, arg0, arg1, arg2); }

s32 func_00171818(s32 arg0, s32 arg1, s32 arg2) { return func_00171660(6, arg0, arg1, arg2); }

s32 func_00171848(s32 arg0, s32 arg1, s32 arg2) { return func_00171660(7, arg0, arg1, arg2); }

void func_00171878(s32 arg0) {
    s32 d0057C718 = D_0057C718;
    s32 d0057C720 = D_0057C720;

    D_0057C71C = D_005979B8[arg0];
    D_0057C724 = D_0057C71C->unkC;
    D_0057C718 = D_0057C71C->unk10;
    D_0057C728 = D_0057C71C->unk14;
    D_0057C720 = D_0057C71C->unk18;
    D_0057C71C->unk1C++;

    func_001710B4();

    D_0057C718 = d0057C718;
    D_0057C720 = d0057C720;
}

void func_00171918(void) { func_00171878(0); }

void func_00171920(void) { func_00171878(1); }

void func_00171928(void) { func_00171878(2); }

void func_00171930(void) { func_00171878(3); }

void func_00171938(void) { func_00171878(4); }

void func_00171940(void) { func_00171878(5); }

void func_00171948(void) { func_00171878(6); }

void func_00171950(void) { func_00171878(7); }

void func_00171958(s32 index) {
    SaTask** slot = &D_005979B8[index];
    SaTask* task = *slot;

    if (task->unk24 != 0) {
        func_0011D590(task, 10);
    }
    *slot = NULL;
}

void func_001719A0() { func_00171958(0); }

void func_001719A8() { func_00171958(1); }

void func_001719B0() { func_00171958(2); }

void func_001719B8() { func_00171958(3); }

void func_001719C0() { func_00171958(4); }

void func_001719C8() { func_00171958(5); }

void func_001719D0() { func_00171958(6); }

void func_001719D8() { func_00171958(7); }

s32 func_001719E0(SaTask* task, s32 arg1, s32 arg2, s32 arg3) {
    s32 d0057C718 = D_0057C718;
    s32 d0057C720 = D_0057C720;
    s32 result;

    D_0057C724 = task->unkC;
    D_0057C718 = task->unk10;
    D_0057C728 = task->unk14;
    D_0057C720 = task->unk18;

    result = func_00170FE4(arg1, arg2, arg3);
    D_0057C718 = d0057C718;
    D_0057C720 = d0057C720;
    return result;
}

s32 func_00171A68(s32 arg0, s32 arg1, s32 arg2) { return func_00170FE4(arg0, arg1, arg2); }

typedef struct {
    u8 pad[0x1310];
} UnkSize1310; // size: 0x1310

typedef struct {
    u8 pad[0x20];
} UnkSize20; // size: 0x20

typedef struct {
    u8 pad[0x5C8];
} UnkSize5C8; // size: 0x5C8

// Returns size needed for a task with n thread slots
s32 func_00171A88(s32 n) { return ALIGN_64(sizeof(UnkSize5C8) + n * sizeof(UnkSize1310) + n * sizeof(UnkSize20)); }

void func_00170F9C();
extern s32 (*D_0030E548[TASK_GRUPE_MAX])(void (*fn)(void), s32, s32);
extern void (*D_0030E568[TASK_GRUPE_MAX])(void);
extern void (*D_0030E588[TASK_GRUPE_MAX])(void);
extern char D_0056BD90[]; // "sa_task.c"

SaTask* func_00171AB0(s32 n, SaTask* task, s32 owned) {
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
    task->unkC = slots;

    for (i = 0; i < TASK_GRUPE_MAX; i++) {
        if (D_005979B8[i] == NULL) {
            break;
        }
    }

    if (i == TASK_GRUPE_MAX) {
        dbgAssert("i!=TASK_GRUPE_MAX", D_0056BD90, 0xFE);
    }
    D_005979B8[i] = task;

    task->unk20 = i;

    D_0057C724 = task->unkC = slots;
    D_0057C718 = task->unk10 = slots + slotsSize;
    D_0057C728 = task->unk14 = slots + slotsSize + 0x370;
    D_0057C720 = task->unk18 = n;

    task->unk8 = D_0030E548[i];
    task->unk0 = D_0030E568[i];
    task->unk4 = D_0030E588[i];
    task->unk24 = owned;
    task->unk1C = 0;

    func_00170F9C();

    D_0057C718 = saved718;
    D_0057C720 = saved720;

    return task;
}

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171C78);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171CC8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171CD0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171CD8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171CE0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171CE8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171CF0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171CF8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D00);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D08);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D10);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D18);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D20);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D28);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D30);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D38);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D40);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D48);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D50);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D58);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D60);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D68);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D70);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D78);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D80);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D88);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D90);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171D98);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DA0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DA8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DB0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DB8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DC0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DC8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DD0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DD8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DE0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DE8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DF0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171DF8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E00);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E08);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E10);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E18);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E20);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E28);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E30);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E38);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E40);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E48);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E50);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E58);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E60);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E68);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E70);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E78);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E80);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E88);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E90);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171E98);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171EA0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171EA8);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171EB0);

INCLUDE_ASM("asm/jp_intl/nonmatchings/lib/sa_task", func_00171EB8);

INCLUDE_RODATA("asm/jp_intl/nonmatchings/lib/sa_task", D_0056BD90);
