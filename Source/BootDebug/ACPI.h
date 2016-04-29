#include <stdint.h>
#include "InterruptControler.h"
#pragma once

typedef struct acpi_table_header ACPI_TABLE_HEADER;

class ACPI
{
	bool		m_Loaded;
	bool		m_Enabled;
	uint32_t	m_PICMode;

public:
	ACPI(void);
	~ACPI(void);

	// Start up Stage 1
	bool InitilizeTables();

	// Start up Stage 2
	bool Enable();

	ACPI_TABLE_HEADER *GetTable(char *Table, uint16_t Index = 0);
	bool SetAPICMode(uint32_t Mode = 1);

	void Dump(char *Options);
};

