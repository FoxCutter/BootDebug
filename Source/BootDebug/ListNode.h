#pragma once

// Lists seem to be everywhere, so lets just make a simple template to handle them and the most common functions
template<typename T>
struct ListNode
{	
	typedef typename T * ListEntry;
	
	ListEntry Prev;
	ListEntry Next;

	// Get the first node in this list
	inline ListEntry First()
	{
		ListEntry Ret = static_cast<ListEntry>(this);
		while(Ret->Prev != nullptr)
			Ret = Ret->Prev;

		return Ret;
	}

	// Get the last node in this list
	inline ListEntry Last()
	{
		ListEntry Ret = static_cast<ListEntry>(this);
		while(Ret->Next != nullptr)
			Ret = Ret->Next;

		return Ret;
	}

	// Add the new node to the end of 'this' list.
	inline void InsertLast(ListEntry NewNode)
	{
		ListEntry End = this->Last();

		NewNode->Next = nullptr;
		NewNode->Prev = End;
		End->Next = NewNode;
	}

	// Add the new node to the start of 'this' list.
	inline void InsertFirst(ListEntry NewNode)
	{
		ListEntry First = this->First();

		NewNode->Next = First;
		NewNode->Prev = nullptr;
		First->Prev = NewNode;
	}

	// Add the new node before 'this' node
	inline void InsertBefore(ListEntry NewNode)
	{
		ListEntry Myself = static_cast<ListEntry>(this);
		NewNode->Next = Myself;
		NewNode->Prev = Myself->Prev;

		Myself->Prev = NewNode;
		if(NewNode->Prev != nullptr)
			NewNode->Prev->Next = NewNode;
	}

	// Add the new node after 'this' node
	inline void InsertAfter(ListEntry NewNode)
	{
		ListEntry Myself = static_cast<ListEntry>(this);
		NewNode->Next = Myself->Next;
		NewNode->Prev = Myself;

		Myself->Next = NewNode;
		if(NewNode->Next != nullptr)
			NewNode->Next->Prev = NewNode;
	}

	// Remove this node from the list, and return the next node in the list.
	// This way you can do Head = Head.RemoveFromList();
	inline ListEntry RemoveFromList()
	{
		// We always return the next node in the list.
		ListEntry Ret = Next;
		
		if(Prev == nullptr && Next == nullptr)
			return Ret;

		if(Prev == nullptr)
		{
			// We are at the start of our list, so cut out the back pointer on the next block in the chain
			Next->Prev = nullptr;
		}
		else if(Next == nullptr)
		{
			// We're at the end of the chain, so just drop ourselves out.
			Prev->Next = nullptr;
		}
		else
		{
			// Stuck in the middle, so slice ourselves out.
			Prev->Next = Next;
			Next->Prev = Prev;
		}

		Next = nullptr;
		Prev = nullptr;

		return Ret;
	}
};