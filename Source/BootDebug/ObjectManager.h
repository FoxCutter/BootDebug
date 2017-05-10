#pragma once

#include "RawMemory.h"
#include "ObjectCallback.h"
#include "ListNode.h"

class ObjectManager : public ObjectCallback
{
	struct ObjectEntry : ListNodeBase<ObjectEntry>
	{
		char Name[32];
		ObjectCallback * CallBack;
		
		ListEntry Prev;
		ListEntry Next;
	};

	RawMemory *m_Heap;

	ObjectEntry *m_ObjectRoot;

public:
	ObjectManager(void);
	~ObjectManager(void);

	void Setup();
	void AddObject(char *Name, char cbLength, ObjectCallback *Callback);

	void DisplayObjects(uint32_t ArgCount, char *ArgData[]);

	static ObjectManager * Current();

private:
	void DisplayObject(uint32_t ArgCount, char *ArgData[]);

	ObjectEntry * NewNode(char *Name, char cbLength, ObjectCallback *Callback);

};

