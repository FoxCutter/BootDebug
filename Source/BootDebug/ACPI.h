#include <stdint.h>
#pragma once

class ACPI
{
public:
	ACPI(void);
	~ACPI(void);

	bool Initilize();
	static void Dump(char *Options);
};

