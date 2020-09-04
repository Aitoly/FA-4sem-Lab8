#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<iostream>
#include<ctype.h>
#include <vector>
#include <list>
#include <strstream>

#include "System2Storage.h"



int SizeOf(char* nameOfFile)
{
	FILE* in;
	in = fopen(nameOfFile, "r");
	if (!in)
	{
		printf("I can`t open in.txt\n");
		exit(-1);
	}

	char c;
	int len = 0;

	while (!feof(in))
	{
		c = fgetc(in);
		bool spase = false;

		if (isspace(c))
		{
			if (spase)
			{
				continue;
			}
			else
			{
				spase = true;
				++len;
			}
		}
		else if (c == EOF)
		{
			++len;
			return len;
		}
		else
		{
			++len;
			spase = false;
		}
	}

	return len;
}

void Get_All(FILE* in, char* line, int argc, char** argv)
{
	char* original = line;
	bool include = false;
	char buff[BUFSIZ];
	int iBuff = 0;

	if (!in)
	{
		printf("I can`t open in.txt\n");
		exit(-1);
	}

	char c;

	while (!feof(in))
	{
		c = fgetc(in);
		bool spase = false;

		if (isspace(c))
		{
			if (spase)
			{
				continue;
			}
			else
			{
				spase = true;
				*line++ = ' ';
			}
		}
		else if (c == EOF)
		{
			*line = '\0';
			return;
		}
		else
		{
			*line++ = c;
			*line = '\0';
			spase = false;
		}
	}
}

std::vector<char*> GetWords(char* buff)
{
	std::vector<char*> words;

	if (!buff)
		return words;

	int i = 0;
	int maxI = strlen(buff);
	char locBuff[BUFSIZ];
	char* ptr = locBuff;
	*ptr = '\0';

	for (; i <= maxI; ++i)
	{
		if (buff[i] == '\0' || buff[i] == '\n' || buff[i] == ' ')
		{
			*ptr = '\0';
			if (strlen(locBuff) > 0)
			{
				char* word = nullptr;
				word = new char[strlen(locBuff) + 1];
				strcpy(word, locBuff);
				words.push_back(word);
				ptr = locBuff;
				*ptr = '\0';
			}
		}
		else if (isSpecialSymbol(buff[i]))
		{
			*ptr = '\0';
			if (strlen(locBuff) > 0)
			{
				char* word = nullptr;
				word = new char[strlen(locBuff) + 1];
				strcpy(word, locBuff);
				words.push_back(word);
				ptr = locBuff;
				*ptr = '\0';
			}
			{
				char* word = nullptr;
				word = new char[2];
				word[0] = buff[i];
				word[1] = '\0';
				words.push_back(word);
			}
		}
		else
		{
			*ptr++ = buff[i];
		}
	}

	return words;
}




int main(int argc, char** argv)
{
	if (argc == 1)
		return 1;

	FILE* in;

	in = fopen(argv[1], "r");
	if (!in)
	{
		printf("I can`t open %s\n", argv[1]);
		exit(-1);
	}
	 
	//получение длинны файла
	int sizeLine = 0;
	for (int i = 1; i < argc; ++i)
	{
		sizeLine += SizeOf(argv[i]);
	}

	//объ€вление переменных
	char* line = new char[sizeLine + 1];
	*line = 0;
	
	//чтение файла и разбиение на слова
	Get_All(in, line, argc, argv);
	std::vector<char*> content;
	content = GetWords(line);

	System2Storage system2Storage(10);

	int err = system2Storage.Do(content);

	printf("\n");

	switch (err)
	{
	case 0:
		printf("Exit with code 0\n");
		break;
	case 1:
		printf("Brackets error!\n");
		break;
	case 2:
		printf("Error in command!\n");
		break;
	}
	
	return 0;
}