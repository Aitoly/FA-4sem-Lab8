#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <list>
#include <cctype>

#include "System2Storage.h"


///////////////////////////////////////////////
//////////			MemoryBox		///////////
///////////////////////////////////////////////

MemoryBox::MemoryBox(char* _addr, int _size, int _sizeTotal, Position _position) :
addr(_addr), size(_size), sizeTotal(_sizeTotal), name(nullptr), visible(true), position(_position)
{}

MemoryBox::~MemoryBox()
{
	delete name;
}



char* MemoryBox::GetAddr()
{
	return addr;
}

int MemoryBox::GetSize() const
{
	return size;
}

int MemoryBox::GetSizeTotal() const
{
	return sizeTotal;
}

const char* MemoryBox::GetName() const
{
	return name;
}

bool MemoryBox::IsVisible() const
{
	return visible;
}

Position MemoryBox::GetPosition() const
{
	return position;
}


void MemoryBox::SetAddr(char* a)
{
	addr = a;
}

void MemoryBox::SetSize(int s)
{
	size = s;
}

void MemoryBox::SetSizeTotal(int s)
{
	sizeTotal = s;
}

void MemoryBox::SetName(char* s)
{
	delete name;

	if (s)
	{
		name = new char[strlen(s) + 1];
		strcpy(name, s);
	}
	else
	{
		name = nullptr;
	}
}

void MemoryBox::SetVisible(bool b)
{
	visible = b;
}

void MemoryBox::SetPosition(Position p)
{
	position = p;
}

///////////////////////////////////////////////
//////////		System2Storage		///////////
///////////////////////////////////////////////

System2Storage::System2Storage(int total)
{
	memory = new char[pow(2, total)];

	boxes.push_back(new MemoryBox(memory, (int)pow(2, total), total, Position::CENTER));
}

System2Storage::~System2Storage() 
{
	delete memory;
}




MemoryBox* System2Storage::alloc(char* name, int size)
{
	if (size == 0)
	{
		return nullptr;
	}

	for (auto it = boxes.begin(); it != boxes.end(); ++it)
	{
		if((*it)->GetName())
		if (!strcmp((*it)->GetName(), name))
		{
			(*it)->SetVisible(false);
		}
	}

	int needSizeTotal = 1;

	//находим размер нужного блока
	while (pow(2, needSizeTotal) < size)
	{
		++needSizeTotal;
	}

	bool inseart = false;
	std::list<MemoryBox*>::iterator bestPlace = boxes.end();//наиболее подходящий блок


	for (auto it = boxes.begin(); it != boxes.end(); ++it)
	{
		if (!(*it)->GetName())
		{
			if ((*it)->GetSizeTotal() == needSizeTotal)//если размер точь в точь то он подходит
			{
				bestPlace = it;
			}
			else if ((*it)->GetSizeTotal() > needSizeTotal)//если размер больше то
			{
				if (bestPlace == boxes.end())//если раньше ниче не подходило, то берем
				{
					bestPlace = it;
				}
				else if ((*it)->GetSizeTotal() < (*bestPlace)->GetSizeTotal())
				{//если раньше что то подходило, то проверям этот лучше или нет
					bestPlace = it;
				}
			}
		}
	}


	if (bestPlace == boxes.end())
		return nullptr;

	//дробление блоков
	while ((*bestPlace)->GetSizeTotal() > needSizeTotal)
	{
		char* mem = (*bestPlace)->GetAddr();
		int totSize = (*bestPlace)->GetSizeTotal();

		delete (*bestPlace);
		(*bestPlace) = new MemoryBox(mem + (int)pow(2, totSize - 1), (int)pow(2, totSize - 1), totSize - 1, Position::RIGHT);

		bestPlace = boxes.insert(bestPlace, new MemoryBox(mem, (int)pow(2, totSize - 1), totSize - 1, Position::LEFT));
	}

	(*bestPlace)->SetName(name);
	(*bestPlace)->SetSize(size);

	return (*bestPlace);
}

void System2Storage::print()
{
	for(auto it = boxes.begin(); it != boxes.end(); ++it)
	{
		if ((*it)->GetName())
		{
			printf("%s::%d, ", (*it)->GetName(), (int)pow(2, (*it)->GetSizeTotal()));
		}
		else
		{
			printf("FREE::%d, ", (int)pow(2, (*it)->GetSizeTotal()));
		}
	}

	printf("\n");

	for (auto it = boxes.begin(); it != boxes.end(); ++it)
	{
		if ((*it)->IsVisible() && (*it)->GetName())
		{
			printf("%s::%d, ", (*it)->GetName(), (*it)->GetSize());
		}
	}

	printf("\n");
}

void System2Storage::GBCollector()
{
	for (auto it = boxes.begin(); it != boxes.end(); ++it)
	{
		if ((*it)->GetName() && !(*it)->IsVisible())
		{
			(*it)->SetName(nullptr);
			(*it)->SetVisible(true);
		}
	}

	for (auto it = boxes.begin(); it != boxes.end();)
	{
		if (!(*it)->GetName())
		{
			std::list<MemoryBox*>::iterator next = it;
			++next;

			if ((*it)->GetPosition() == Position::LEFT && !(*next)->GetName())
			{
				boxes.erase(next);
				(*it)->SetSizeTotal((*it)->GetSizeTotal() + 1);

				if (boxes.size() == 1)
				{
					(*it)->SetPosition(Position::CENTER);
				}
				else
				{
					(*it)->SetPosition(Position::RIGHT);
					it = boxes.begin();
					continue;
				}
			}
		}

		++it;
	}
}




int System2Storage::Do(std::vector<char*>& content)
{
	/*
	0 - ok
	1 - нарушена вложенность скобок
	2 - ошибка в комманде
	*/



	int size = content.size();

	for (int i = 0; i < size; ++i)
	{
		if (isOpenFigure(content[i]))
		{
			visibleVars.push_back(std::vector<MemoryBox*>());
			//printf("Now I find {\n");
		}
		else if(isCloseFigure(content[i]))
		{
			if (visibleVars.size())
			{
				int currentLevl = visibleVars.size() - 1;

				while (visibleVars[currentLevl].size())
				{
					int currentBox = visibleVars[currentLevl].size() - 1;
					visibleVars[currentLevl][currentBox]->SetVisible(false);
					visibleVars[currentLevl].pop_back();
				}

				visibleVars.pop_back();
				//printf("Now I find }\n");
			}
			else
			{
				return 1;
			}
		}
		else if (!strcmp(content[i], "ShowVar"))
		{
			if (i + 1 < size && isEndOfCommand(content[i + 1]))
			{
				printf("ShowVar:\n");
				print();
				printf("\n");
				++i;
			}
			else
			{
				return 2;
			}
		}
		else if (!isSpecialSymbol(content[i]))
		{
			if (i + 6 < size && isEqual(content[i + 1]) && !strcmp(content[i + 2], "new") &&
				isOpenRound(content[i + 3]), isNum(content[i + 4]) && isCloseRound(content[i + 5]) &&
				isEndOfCommand(content[i + 6]))
			{
				int sizeOfVar = atoi(content[i + 4]);
				if (sizeOfVar > 0)
				{
					int currentLevl = visibleVars.size() - 1;
					MemoryBox* inseartedBox = alloc(content[i], sizeOfVar);

					if (inseartedBox)
					{
						visibleVars[currentLevl].push_back(inseartedBox);
						printf("I alloced %d bytes for var %s\n", sizeOfVar, content[i]);
					}
					else
					{
						GBCollector();

						inseartedBox = alloc(content[i], sizeOfVar);
						if (inseartedBox)
						{
							visibleVars[currentLevl].push_back(inseartedBox);
							printf("I alloced %d bytes for var %s (GBC)\n", sizeOfVar, content[i]);
						}
						else
						{
							printf("I CAN`T alloc %d bytes for var %s\n", sizeOfVar, content[i]);
						}
					}
				}
				else
				{
					printf("I skipped var %s\n", content[i]);
				}

					i += 6;

					//print();
					printf("\n");
			}
			else
			{
				return 2;
			}
		}
	}

	if (visibleVars.size())
		return 1;

	return 0;
}








////////////
//	Специальные символы для строк
bool isEndOfCommand(char* s)
{
	return !strcmp(s, ";");
}

bool isOpenRound(char* s)
{
	return !strcmp(s, "(");
}

bool isCloseRound(char* s)
{
	return !strcmp(s, ")");
}

bool isOpenFigure(char* s)
{
	return !strcmp(s, "{");
}

bool isCloseFigure(char* s)
{
	return !strcmp(s, "}");
}

bool isEqual(char* s)
{
	return !strcmp(s, "=");
}

bool isSpecialSymbol(char* s)
{
	return isEndOfCommand(s) || isOpenRound(s) || isCloseRound(s) || isOpenFigure(s) || isCloseFigure(s) ||
		isEqual(s);
}

////////////
//	Специальные символы для чаров
bool isEndOfCommand(char s)
{
	return (s == ';');
}

bool isOpenRound(char s)
{
	return (s == '(');
}

bool isCloseRound(char s)
{
	return (s == ')');
}

bool isOpenFigure(char s)
{
	return (s == '{');
}

bool isCloseFigure(char s)
{
	return (s == '}');
}

bool isEqual(char s)
{
	return (s == '=');
}

bool isSpecialSymbol(char s)
{
	return isEndOfCommand(s) || isOpenRound(s) || isCloseRound(s) || isOpenFigure(s) || isCloseFigure(s) ||
		isEqual(s);
}

////////////
//	Проверка на число
bool isNum(char* s)
{
	int size = strlen(s);
	for (int i = 0; i < size; ++i)
	{
		if (!isdigit(s[i])) return false;
	}

	return true;
}
