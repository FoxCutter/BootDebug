#pragma once
#include <stdint.h>

namespace Stack
{
	inline void Push(uint32_t &StackPointer, uint32_t Value)
	{
		uint32_t * Stack = reinterpret_cast<uint32_t *>(StackPointer);
		Stack--;
		*Stack = Value;
		StackPointer -= sizeof(uint32_t);
	}

	inline uint32_t Pop(uint32_t &StackPointer)
	{
		uint32_t * Stack = reinterpret_cast<uint32_t *>(StackPointer);
		uint32_t Value = *Stack;
		Stack++;
		StackPointer += sizeof(uint32_t);
	}
}