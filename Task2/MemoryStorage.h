#pragma once

class Link;

class MemoryBox
{
public:
	MemoryBox(char* _addr, int _size, MemoryBox* _next, MemoryBox* _prev, bool _free);
	~MemoryBox();

	char* GetAddr();
	int GetSize() const;
	MemoryBox* GetPrev();
	MemoryBox* GetNext();
	bool IsFree() const;

	void SetSize(int s);
	void AddLink(Link* link);
	void RemoveLink(Link* link);
	void SetPrev(MemoryBox*);
	void SetNext(MemoryBox*);
	void SetFree(bool b);

	MemoryBox* next;
	MemoryBox* prev;
	
	std::list<Link*> links;

private:
	MemoryBox();

	char* addr;
	int size;
	bool free;
	
};

class Link
{
public:
	Link(MemoryBox*, char* _name);
	Link(const Link& other);
	~Link();

	bool ifName(char* _name) const;
	bool LinkTo(MemoryBox* _box);
	
	MemoryBox* box;

private:
	Link();

	char* name;
	
};



class MemoryStorage
{
public:
	MemoryStorage(int total);
	~MemoryStorage();
	
	bool Do_operation(int type, char* left, char* right);

private:
	int TOTAL_MEM; // 1 mb = 1048576 b

	std::vector<Link*> links;

	std::list<MemoryBox*> boxesFree;
	std::list<MemoryBox*> boxesBusy;

	char* memory;

	int alloc(char* name, int size);
	int dealloc(char* name);
	int assignment(char* left, char* right);
	void association(std::list<MemoryBox*>& list, MemoryBox* box);
};

