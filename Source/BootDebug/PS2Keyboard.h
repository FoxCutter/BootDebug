#pragma once
#include <stdint.h>

struct InterruptContext;

void SetupPS2Keyboard();
void KeyboardInterrupt(InterruptContext * OldContext, uintptr_t * Data);