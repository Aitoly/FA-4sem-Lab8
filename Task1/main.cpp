#pragma warning(disable:4996)

#include<stdio.h>
#include<iostream>
#include<ctype.h>
#include <vector>
#include <list>accountssettingspaneinterop.h

#include "MemoryStorage.h"

int char_to_int(char c)
{
	if (isalpha(c))
	{
		return toupper(c) - 'A' + 10;
	}
	else if (isdigit(c))
	{
		return c - '0';
	}
	else
	{
		return -1;
	}
}

int string_to_int(char* old_str, int old_base)
{
	if (old_str == NULL)
	{
		return 0;
	}
	int old_num = 0;
	for (int i = 0; i < strlen(old_str); i++)
	{
		old_num += char_to_int(old_str[strlen(old_str) - i - 1]) * pow(old_base, i);
	}
	return old_num;
}

int Get_Info(FILE* in, char* left, char* right)
{
	/*
	0 - ошибка
	1 - a=b;
	2 - dealloc(a);
	3 - a = alloc(10);
	*/

	*left = 0; *right - 0;

	if (!in)
	{
		printf("File not opend!\n");
		return 0;
	}

	char c;
	int mode = 1;

	//1 - собираем left
	//2 - собираем rght
	//3 - нашли "dealloc(" в левой части, ищем что будем освобождать
	//4 - ищем ;
	//5 - нашли "alloc(" в правой части, ищем размрер

	char buf[BUFSIZ] = { 0 };
	char* pbuf = buf;

	while (!feof(in))
	{
		c = fgetc(in);
		bool correct = (isdigit(c) || isalpha(c) || c == '=' || isspace(c) || c == '\n' || c == EOF ||
			c == '(' || c == ')' || c == ';');
		if (!correct)
		{
			printf("Unexpected symbol %c !\n", c);
			return 0;
		}

		if (isspace(c) || c == '\n' || c == EOF)
		{
			continue;
		}

		if (mode == 1)
		{
			if (c == ';')
			{
				printf("I find only left part! (%s)\n", buf);
				return 0;
			}
			else if (c == '=')
			{
				strcpy(left, buf);
				pbuf = buf;
				*pbuf = '\0';
				mode++;
			}
			else
			{
				*pbuf++ = c;
				*pbuf = '\0';
				if (!strcmp("dealloc(", buf))
				{
					strcpy(left, buf);
					mode = 3;
					pbuf = buf;
					*pbuf = '\0';
				}
			}
		}
		else if (mode == 2)
		{
			if (c == ';')
			{
				*pbuf = '\0';
				strcpy(right, buf);
				return 1;
			}
			else if (c == '=')
			{
				printf("I find operation %c beside right operand!\n", c);
				return 0;
			}
			else
			{
				*pbuf++ = c;
				*pbuf = '\0';
				if (!strcmp("alloc(", buf))
				{
					mode = 5;
					pbuf = buf;
					*pbuf = '\0';
				}
			}
		}
		else if (mode == 3)
		{
			if (c == ')')
			{
				if (*buf == '\0')
				{
					printf("dealloc() has no args!\n");
					return 0;
				}
				else
				{
					*pbuf = '\0';
					strcpy(right, buf);
					mode = 4;
				}
			}
			else if (c == '=')
			{
				printf("The operation %c beside ) in dealloc()!\n", c);
				return 0;
			}
			else
			{
				*pbuf++ = c;
			}
		}
		else if (mode == 4)
		{
			if (c == ';')
			{
				return 2;
			}
		}
		else if (mode == 5)
		{
			if (c == ')')
			{
				if (*buf == '\0')
				{
					printf("new() has no args!\n");
					return 0;
				}
				else
				{
					*pbuf = '\0';
					strcpy(right, buf);
					mode = 6;
				}
			}
			else if (c == '=')
			{
				printf("The operation %c beside ) in alloc()!\n", c);
				return 0;
			}
			else
			{
				*pbuf++ = c;
			}
		}
		else if (mode == 6)
		{
			if (c == ';')
			{
				return 3;
			}
		}
	}

	return 0;
}



int main()
{
	FILE* in;
	in = fopen("in.txt", "r");
	if (!in)
	{
		printf("I can`t open input file\n");
		system("pause");
		exit(-1);
	}

	MemoryStorage memory(45);

	////////////////////////////////////////////////////
	////////////////////////////////////////////////////

	char DEF_LEFT[BUFSIZ] = { 0 };
	char DEF_RIGHRT[BUFSIZ] = { 0 };

	while (!feof(in))
	{
		int type = Get_Info(in, DEF_LEFT, DEF_RIGHRT);

		if (!type)
		{
			fclose(in);
			exit(1);
		}
		else
		{
			if (!memory.Do_operation(type, DEF_LEFT, DEF_RIGHRT))
			{
				fclose(in);
				exit(2);
			}
		}
	}

	fclose(in);

	return 0;
}
