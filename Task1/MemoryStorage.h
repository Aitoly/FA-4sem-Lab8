#pragma once

class Link;

class MemoryBox
{
public:
	MemoryBox(char* _addr, int _size);
	~MemoryBox();

	char* GetAddr();
	int GetSize() const;

	void SetSize(int s);
	void AddLink(Link* link);
	void RemoveLink(Link* link);
	
	std::list<Link*> links;

private:
	MemoryBox();

	char* addr;
	int size;
	
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
	int TOTAL_MEM;

	std::vector<Link*> links;

	std::list<MemoryBox*> boxesFree;
	std::list<MemoryBox*> boxesBusy;

	char* memory;

	int alloc(char* name, int size);
	int dealloc(char* name);
	int assignment(char* left, char* right);
	void association(std::list<MemoryBox*>& list, std::list<MemoryBox*>::iterator it);
};

