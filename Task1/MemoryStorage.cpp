#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <list>accountssettingspaneinterop.h

#include "MemoryStorage.h"

///////////////////////////////////////////////
//////////			MemoryBox		///////////
///////////////////////////////////////////////

MemoryBox::MemoryBox(char* _addr, int _size) : addr(_addr), size(_size)
{}

MemoryBox::~MemoryBox()
{}

char* MemoryBox::GetAddr()
{
	return addr;
}

int MemoryBox::GetSize() const
{
	return size;
}

void MemoryBox::SetSize(int s)
{
	size = s;
}

void MemoryBox::AddLink(Link* link)
{
	links.push_back(link);
}

void MemoryBox::RemoveLink(Link* link)
{
	links.remove(link);
}

///////////////////////////////////////////////
//////////			Link			///////////
///////////////////////////////////////////////

Link::Link(MemoryBox* _box, char* _name) : box(_box)
{
	if (_name)
	{
		name = new char[strlen(_name) + 1];
		strcpy(name, _name);
	}
	else
	{
		name = nullptr;
	}

	if(box)
		box->AddLink(this);
}

Link::Link(const Link& other) : box(other.box)
{
	if (other.name)
	{
		name = new char[strlen(other.name) + 1];
		strcpy(name, other.name);
	}
	else
	{
		name = nullptr;
	}
}

Link::~Link()
{
	delete[] name;
}

bool Link::ifName(char* _name) const 
{
	return !strcmp(name, _name);
}

bool Link::LinkTo(MemoryBox* _box)
{
	bool badMem = false;

	if (box)
	{
		box->RemoveLink(this);

		if (box->links.size() == 0)
		{
			badMem = true;
		}
	}

	box = _box;

	if (_box)
	{
		_box->AddLink(this);
	}

	return !badMem;
}

///////////////////////////////////////////////
//////////		MEMORY STORAGE		///////////
///////////////////////////////////////////////

MemoryStorage::MemoryStorage(int total) : TOTAL_MEM(total)
{
	memory = new char[TOTAL_MEM];

	boxesFree.push_front(new MemoryBox(memory, TOTAL_MEM));

	printf("Now Free: ");
	for (auto it = boxesFree.begin(); it != boxesFree.end(); ++it)
	{
		printf("%d ", (*it)->GetSize());
	}
	printf("\n");

	printf("Now Busy: ");
	for (auto it = boxesBusy.begin(); it != boxesBusy.end(); ++it)
	{
		printf("%d ", (*it)->GetSize());
	}
	printf("\n\n");
}

MemoryStorage::~MemoryStorage()
{
	int sizeLinks = links.size();
	for (int i = 0; i < sizeLinks; ++i)
	{
		delete links[i];
	}

	while (boxesFree.size())
	{
		delete *boxesFree.begin();
	}

	while (boxesBusy.size())
	{
		delete* boxesBusy.begin();
	}

	delete[] memory;
}

bool MemoryStorage::Do_operation(int type, char* left, char* right)
{
	//printf("LEFT = %s\n", left);
	//printf("RIGHT = %s\n", right);

	switch (type)
	{
	case 1: 
	{
		switch (assignment(left, right))
		{
		case 0: printf("I linked %s to %s`s mem\n", left, right);
			break;
		case 1: printf("I CAN`T link %s to %s`s mem (%s not found)\n", left, right, right);
			break;
		case 2: printf("I linked %s to %s`mem, but it`s bad memory\n", left, right);
			break;
		case 3: printf("You try to read/write in NULL\n", left, right);
			return false;
		}
	}
		break;
	case 2: 
	{
		switch (dealloc(right))
		{
		case 0:	printf("I dealloced %s\n", right);
			break;
		case 1:	printf("I CAN`T dealloc %s (Not found)\n", right);
			break;
		case 2:	printf("I dealloced %s, but it`s bad memory\n", right);
			break;
		case 3: printf("You try to read/write in NULL\n", left, right);
			return false;
		}
	}
		break;
	case 3: 
	{
		int size = atoi(right);

		switch (alloc(left, size))
		{
		case 0: printf("I alloced %d bytes for var %s\n", size, left);
			break;
		case 1: printf("I CAN`T alloc %d bytes for var %s (No memory)\n", size, left);
			break;
		case 2: printf("I alloced %d bytes for var %s, but it`s bad memory\n", size, left);
			break;
		case 3: printf("You try to read/write in NULL\n", left, right);
			return false;
		}
	}
		break;
	}

	printf("Now Free: ");
	for (auto it = boxesFree.begin(); it != boxesFree.end(); ++it)
	{
		printf("%d ", (*it)->GetSize());
	}
	printf("\n");

	printf("Now Busy: ");
	for (auto it = boxesBusy.begin(); it != boxesBusy.end(); ++it)
	{
		printf("%d ", (*it)->GetSize());
	}
	printf("\n");

	printf("\n");

	return true;
}

int MemoryStorage::alloc(char* name, int size)
{
	/*
	0 - ok
	1 - нет памяти
	2 - потеря памяти
	*/

	Link* link = nullptr;

	for (int i = 0; i < links.size() && !link; ++i)
	{
		if (links[i]->ifName(name))
		{
			link = links[i];
		}
	}

	if (size == 0)
	{
		if (link)
		{
			if (link->LinkTo(nullptr))
				return 0;
			return 2;
		}
		else
		{
			links.push_back(new Link(nullptr, name));
			return 0;
		}
	}

	bool insert = false;
	for (auto it = boxesFree.begin(); it != boxesFree.end(); ++it)
	{
		MemoryBox* box = *it;

		if(box->GetSize() == size)
		{
			boxesBusy.push_back(box);
			boxesFree.erase(it);
			
			insert = true;

			if (link)
			{
				if (link->LinkTo(box))
					return 0;
				return 2;
			}
			else
			{
				links.push_back(new Link(box, name));
				return 0;
			}
		}
		else if(box->GetSize() > size)
		{
			char* beginOfFree = box->GetAddr();
			int sizeOfFree = box->GetSize();

			delete box;

			(*it) = new MemoryBox(beginOfFree + size, sizeOfFree - size);
			MemoryBox* newBox = new MemoryBox(beginOfFree, size);

			boxesBusy.push_back(newBox);

			insert = true;

			if (link)
			{
				if (link->LinkTo(newBox))
					return 0;
				return 2;
			}
			else
			{
				links.push_back(new Link(newBox, name));
				return 0;
			}
		}
	}

	if (!insert)
	{
		return 1;
	}
}

int MemoryStorage::dealloc(char* name)
{
	/*
	0 - ок
	1 - переменная не найдена
	2 - ok, потеря памяти
	3 - обращение к NULL
	*/

	int sizeLinks = links.size();
	Link* link = nullptr;
	bool badMem = false;

	int i;
	for (i = 0; i < sizeLinks && !link; ++i)
	{
		if (links[i]->ifName(name))
		{
			link = links[i];
		}
	}

	if (!link)
	{
		return 1;
	}

	if (!link->box)
	{
		return 0;
	}

	if (link->box->links.size() > 1)
		badMem = true;

	for (auto it_links = link->box->links.begin(); it_links != link->box->links.end(); ++it_links)
	{
		if((*it_links) != link)
			(*it_links)->box = nullptr;
	}
	link->box->links.clear();

	boxesBusy.remove(link->box);
	auto it = boxesFree.begin();
	for (; it != boxesFree.end(); ++it)
	{
		if ((*it)->GetAddr() > link->box->GetAddr())
		{
			auto it_ = boxesFree.insert(it, link->box);
			association(boxesFree, it_);
			it = boxesFree.begin();
			break;
		}
	}

	if (it == boxesFree.end())
	{
		boxesFree.push_back(link->box);
		auto iterator = boxesFree.end();
		--iterator;
		association(boxesFree, iterator);
	}

	std::vector<Link*> timeLinks;
	for (int j = 0; j < sizeLinks; ++j)
	{
		if (links[j] != link)
			timeLinks.push_back(new Link(*links[j]));
	}

	delete link;
	links = timeLinks;

	if (badMem)
		return 2;
	return 0;
}

int MemoryStorage::assignment(char* left, char* right)
{
	/*
	0 - ok
	1 - правая переменная не найденна
	2 - потеря памяти
	3 - обращение к NULL
	*/

	int sizeLinks = links.size();
	Link* linkLeft = nullptr;
	Link* linkRight = nullptr;

	int i;
	for (i = 0; i < sizeLinks && (!linkLeft || !linkRight); ++i)
	{
		if (!linkLeft && links[i]->ifName(left))
		{
			linkLeft = links[i];
		}
		if (!linkRight && links[i]->ifName(right))
		{
			linkRight = links[i];
		}
	}

	if (!linkRight)
		return 1;

	if (!linkRight->box)
	{
		return 3;
	}

	if (linkLeft == linkRight)
		return 0;

	if (!linkLeft)
	{
		links.push_back(new Link(linkRight->box, left));
		return 0;
	}
	else
	{
		if (linkLeft->LinkTo(linkRight->box))
			return 0;
		return 2;
	}
}

void MemoryStorage::association(std::list<MemoryBox*>& list, std::list<MemoryBox*>::iterator it)
{
	if (it != list.begin())
	{
		std::list<MemoryBox*>::iterator it_prev = it;
		--it_prev;

		if ((*it_prev)->GetAddr() + (*it_prev)->GetSize() == (*it)->GetAddr())
		{
			int plusSize = (*it)->GetSize();
			list.erase(it);
			it = it_prev;
			(*it)->SetSize((*it)->GetSize() + plusSize);
		}
	}

	std::list<MemoryBox*>::iterator it_end = list.end();
	--it_end;

	if (it != it_end)
	{
		std::list<MemoryBox*>::iterator it_next = it;
		++it_next;

		if ((*it)->GetAddr() + (*it)->GetSize() == (*it_next)->GetAddr())
		{
			int plusSize = (*it_next)->GetSize();
			list.erase(it_next);
			(*it)->SetSize((*it)->GetSize() + plusSize);
		}
	}
}