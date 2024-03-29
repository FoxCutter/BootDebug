#include "ObjectManager.h"
#include "CoreComplex.h"
#include "KernalLib.h"

/*

  Objects are an easy to assign names to objects in the system. It's a way to provide short cuts to internal objects without having
  to go through so many extra layers of the system. It might expand into a larger part, but for right now it's pretty simple.

*/


ObjectManager::ObjectManager(void) : m_Heap(nullptr)
{
}

ObjectManager::~ObjectManager(void)
{
}

ObjectManager * ObjectManager::Current()
{
	return &CoreComplexObj::GetComplex()->ObjectComplex;
}

void ObjectManager::Setup()
{
	m_Heap = &CoreComplexObj::GetComplex()->KernalHeap;

	m_ObjectRoot = NewNode("ObjMan", 6, this);
}

void ObjectManager::AddObject(char *Name, char cbLength, ObjectCallback *Callback)
{
	ObjectEntry *Entry = NewNode(Name, cbLength, Callback);
	m_ObjectRoot->InsertLast(Entry);
}

ObjectManager::ObjectEntry * ObjectManager::NewNode(char *Name, char cbLength, ObjectCallback *Callback)
{
	ObjectEntry *Entry = reinterpret_cast<ObjectEntry *>(m_Heap->malloc(sizeof(ObjectEntry), false));
	Entry->Next = Entry->Prev = nullptr;
	memcpy(Entry->Name, Name, 32);
	Entry->Name[cbLength] = 0;
	Entry->CallBack = Callback;	

	return Entry;
}

void ObjectManager::DisplayObjects(uint32_t ArgCount, char *ArgData[])
{	
	if(ArgCount == 0)
	{
		KernalPrintf( "Object Lst: \n");
	}

	ObjectEntry *Current = m_ObjectRoot->First();
	while(Current != nullptr)
	{
		if(ArgCount == 0)
		{
			KernalPrintf("  %s\n", Current->Name);
		}
		else
		{
			if(_stricmp(Current->Name, ArgData[0]) == 0)
			{
				Current->CallBack->DisplayObject(ArgCount, ArgData);
				break;
			}

		}
		Current = Current->Next;
	};

	if(ArgCount != 0 && Current == nullptr)
	{
		KernalPrintf(" Object Name Missing\n");	
	}
}

void ObjectManager::DisplayObject(uint32_t ArgCount, char *ArgData[])
{
	KernalPrintf("  Object Root: %08X\n", m_ObjectRoot); 
}