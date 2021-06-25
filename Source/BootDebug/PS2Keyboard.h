#pragma once
#include <stdint.h>

struct InterruptContext;

void SetupPS2Keyboard();
void SetupPS2Mouse();
void KeyboardInterrupt(InterruptContext * OldContext, uintptr_t * Data);
void MouseInterrupt(InterruptContext * OldContext, uintptr_t * Data);