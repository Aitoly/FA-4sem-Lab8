#pragma once

enum Position
{
	CENTER,
	LEFT,
	RIGHT
};

class MemoryBox
{
public:
	MemoryBox(char* _addr, int _size, int _sizeTotal, Position position);
	~MemoryBox();

	char* GetAddr();
	int GetSize() const;
	int GetSizeTotal() const;
	const char* GetName() const;
	bool IsVisible() const;
	Position GetPosition() const;

	void SetAddr(char* a);
	void SetSize(int s);
	void SetSizeTotal(int s);
	void SetName(char* s);
	void SetVisible(bool b);
	void SetPosition(Position p);

private:
	MemoryBox();

	char* addr;
	int size;
	int sizeTotal;
	char* name;
	bool visible;
	Position position;
};



class System2Storage
{
public:
	System2Storage(int total);
	~System2Storage();

	int Do(std::vector<char*>& content);

private:
	std::list<MemoryBox*> boxes;
	std::vector<std::vector<MemoryBox*>> visibleVars;
	char* memory;

	MemoryBox* alloc(char* name, int size);
	void print();
	void GBCollector();
};

////////////
//	Специальные символы для строк
bool isEndOfCommand(char* s);

bool isOpenRound(char* s);

bool isCloseRound(char* s);

bool isOpenFigure(char* s);

bool isCloseFigure(char* s);

bool isEqual(char* s);

bool isSpecialSymbol(char* s);

////////////
//	Специальные символы для чаров
bool isEndOfCommand(char s);

bool isOpenRound(char s);

bool isCloseRound(char s);

bool isOpenFigure(char s);

bool isCloseFigure(char s);

bool isEqual(char s);

bool isSpecialSymbol(char s);

////////////
//	Проверка на число
bool isNum(char* s);
