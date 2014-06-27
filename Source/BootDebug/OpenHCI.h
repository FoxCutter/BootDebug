#include <stdint.h>
#pragma once

class OpenHCI
{
public:
	OpenHCI(void);
	~OpenHCI(void);

	bool StartUp(uint32_t DeviceID);
};

