#ifndef READ_DEBUG_H
#define READ_DEBUG_H

#include "common.h"

void FileNameChgInit(void);
const char* HostFileNameGet(s32);
s32 HostFileSizeGet(s32);
void HostIndexLoad(void);
u8* HostSizeTblLoad(void);
void* HostModuleSizeTblLoad(void);
void HostModeSet(void);

#endif
