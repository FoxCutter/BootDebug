#include <stdint.h>
#include "ListNode.h"
#pragma once

namespace Handle
{
	typedef uint32_t (* HandleFunction)(Object *HandleObject);

	enum Functions
	{
		CloseObject = 0,		// (Object *HandleObject); Closes the passed in object
		CreateObject = 1,		// (Object *HandleObject); Creates a new object in the passed in handle which will be preallocated
		CloneObject = 2,		// (Object *HandleObject, Object *DestinationObject); Close the passed in object to the new object
		
		ReadObject = 3,			// (Object *HandleObject, Buffer Data, Event Signal); Reads data using the object, signaling the event when done
		WriteObject = 4,		// (Object *HandleObject, Buffer Data, Event Signal); Writes data using the object, signaling the event when done
		
		ControlObject = 5,		// (Object *HandleObject, uint32_t Operation, void *Data); // Control Functions using the object
	};

	struct MetaData : ListNodeBase<MetaData>
	{
		uint8_t Type[4];		// Handle Type
		uint16_t Size;			// Sive of the object including header data
		uint16_t Reserved;		// Must be 0
		uint32_t Flags;		

		MetaData * Next;		
		MetaData * Prev;

		HandleFunction Function[27];
	};

	struct Object : ListNodeBase<Object>
	{
		MetaData * Meta;			// Pointer to the meta data for this object
		uint16_t Size;				// Size of the object data (including header)
		uint16_t AllocatedSize;		// How big the full object is, with padding
		uint32_t Flags;				// Flags

		Object * Next;
		Object * Prev;

		uint8_t Data[];
	};
}
