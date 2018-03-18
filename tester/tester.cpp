#include "stdafx.h"
#include <windows.h>
#include "test.h"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

/*функция вызывает*/
std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(cmd, "w");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
		fputs("10", pipe);
		if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
		_pclose(pipe);
      /*  while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }*/
    } catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    return result;
}

//ключевае слова в описании тестов
char in[] = "Вход:\n";
char out[] = "Выход:\n";
char delimeter[] = "---";

/*
 Функция считывает тесты из файла
 in - filename имя файла с тестами
 out - tests массив структур, куда запишутся тесты
 return - кол-во тестов
*/
void readTests(char* filename, test* tests);

/*
 Функция считает кол-во вхождений подстроки substr в строку str
*/
int countOfSubstr(char* str, char* substr);

/*
1. Читать данные из текстового файла с тестами
2. Запускать тестируемый exe и передавать ему данные для теста
3. Считывать результат выполнения и сверять с эталонными результатами
 
*/



/*
	argv[1] - имя тестируемой программы
	argv[2] - имя файла с тестами
*/
int main(int argc, char* argv[])
{	
	//чтобы в консоли отображался русский язык
	system("chcp 1251 > nul");

	char **tests; //тут будут храниться тесты из файла
	
	argv[1] = new char[100];
	argv[2] = new char[100];
	strcpy(argv[1], "sum.exe");
	strcpy(argv[2], "tests.txt");
	
	/* проверяем, что тестируемая программа и файл с тестами присутствуют в текущей директории */
	if (argc == 3)
	{
		//попробовать "открыть" exe
		FILE* exe = fopen(argv[1], "r");

		//если не получилось - сообщить пользователю
		if (exe == NULL)
		{
			puts("exe-файл отсутсвует!");
			fclose(exe);
			return -1;
		}
		else
			puts("exe файл обнаружен!");


		//попробовать открыть файл с тестами
		FILE* testsFile = fopen(argv[2], "r");

		//если не получилось - сообщить пользователю
		if (testsFile == NULL)
		{
			puts("файл с тестами отсутсвует!");
			fclose(testsFile);
			return -1;
		}
		else
			puts("файл с тестами обнаружен!");
	
		/*Считываем данные из файла с тестами в символьный массив*/
		//перемещаем указатель внутри файла в самый конец
		if (fseek (testsFile, 0, SEEK_END) == 0)
		{
			//получаем кол-во байт от начала файла до его конца (по сути - кол-во символов в файле)
			int size = ftell (testsFile);

			//выделяем память под текст тестов
			char * bufForTests = new char[size + 1];

			//возвращаем указатель внутри открытого файла в начало
			fseek (testsFile, 0, SEEK_SET);

			//считываем тесты из файла
			fread(bufForTests, sizeof(char), size, testsFile);
			bufForTests[size] = '\0';

			//закрываем файл с тестами
			fclose(testsFile);

			//преобразуем строку из файла в массив тестов
			int testsCount = countOfSubstr(bufForTests, delimeter);
			printf("кол-во тестов = %d\n", testsCount);
			test* tests = new test[testsCount];
			int currentTest = 0;
			char* ptr = strtok(bufForTests, delimeter);
			while(ptr != NULL)
			{
				if (currentTest != 0)
					ptr++;
				/*разбиваем тест по составляющим: имя, данные, статус*/
				//название
				int huy = strstr(ptr, "\n") - ptr;
				tests[currentTest].name = new char[strstr(ptr, "\n") - ptr];
				
				memcpy(tests[currentTest].name, ptr, strstr(ptr, "\n") - ptr);
				tests[currentTest].name[strstr(ptr, "\n") - ptr] = '\0';

				//входные данные
				char* inPtr = strstr(ptr, in) + strlen(in);
				char* outPtr = strstr(ptr, out);
				int inLength = (int)(outPtr - inPtr - 1); // - 1, чтобы не учитывать перенос на новую строку
				tests[currentTest].testIn = new char[inLength];
				memcpy(tests[currentTest].testIn, inPtr, inLength);
				tests[currentTest].testIn[inLength] = '\0';

				puts(tests[currentTest].name);
				puts(tests[currentTest].testIn);

				//выходные данные
				//сдвигаем указатель на выходные данные, минуя саму надпись и символ двоеточия
				outPtr += strlen(out);
				tests[currentTest].testOut = new char[strlen(outPtr) - 1];// - 1, чтобы не учитывать перенос на новую строку
				strcpy(tests[currentTest].testOut, outPtr);
				tests[currentTest].testOut[strlen(tests[currentTest].testOut) - 1] = '\0';
				puts(tests[currentTest].testOut);
				puts("-------Конец теста--------");
				//переход к следующему тесту
				ptr = strtok(NULL, delimeter);
				currentTest++;
			}

			//собственно, тестирование
			//идем по всем тестам
			for (int i = 0; i < testsCount; i++)
			{
				char* result;
				//вызываем тестируемый exe с входными данными очередного теста и сохраняем пришедший результат
				runTest(argv[1], tests[i].testIn);

				//если пришедший результат соответствует ожидаемому
				if (strcmp(result, tests[i].testOut) != 0)
				{
					//вывести название теста
					printf("%s - ",tests[i].name);

					//изменить цвет текста на зеленый
					HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
					SetConsoleTextAttribute(handle, FOREGROUND_GREEN);

					//вывести слово "пройден"
					printf("ПРОЙДЕН\n");

					//вернуть цвет обратно - белый
					SetConsoleTextAttribute(handle, FOREGROUND_WHITE);
				}
				//иначе (если тест не пройден)
				else
				{
					puts("-------------");
					//вывести название теста
					printf("%s - ",tests[i].name);

					//изменить цвет текста на красный
					HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
					SetConsoleTextAttribute(handle, FOREGROUND_RED);

					//вывести слово "не пройден"
					printf("НЕ ПРОЙДЕН\n");
					SetConsoleTextAttribute(handle, FOREGROUND_WHITE);

					//вывести, что ожидалось
					puts("ОЖИДАЛОСЬ:");
					puts(tests[i].testOut);

					//вывести, что пришло
					puts("ПРИШЛО ОТ ТЕСТЕРА:");
					puts(result);
					
					puts("-------------");
					
				}
				
			}
					
		}
		else
			puts("ошибка чтения файла");
	}
	//else
	{
		puts("Вы не указали необходимые аргументы командной строки!");
		puts("Вызывайте программу так: tester.exe \"имя тестируемого exe\" \"имя файла с тестами\" ");
	}

	return 0;
}


/*
 Функция считает кол-во вхождений подстроки substr в строку str
*/
int countOfSubstr(char* str, char* substr)
{
	int res = 0;
	char * ptr = strstr(str, substr);
	while(ptr != NULL)
	{
		res++;
		ptr = strstr(ptr + 1, substr);
	}

	return res;
}