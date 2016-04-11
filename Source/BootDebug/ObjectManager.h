#pragma once

#include "RawMemory.h"
#include "ObjectCallback.h"
#include "ListNode.h"

class ObjectManager : public ObjectCallback
{
	struct ObjectEntry : ListNode<ObjectEntry>
	{
		char Name[32];
		ObjectCallback * CallBack;
	};

	RawMemory *m_Heap;

	ObjectEntry *m_ObjectRoot;

public:
	ObjectManager(void);
	~ObjectManager(void);

	void Setup();
	void AddObject(char *Name, char cbLength, ObjectCallback *Callback);

	void DisplayObjects(char * Command, char *Param);

	static ObjectManager * Current();

private:
	void DisplayObject(char * Command, char *Param);

	ObjectEntry * NewNode(char *Name, char cbLength, ObjectCallback *Callback);

};

