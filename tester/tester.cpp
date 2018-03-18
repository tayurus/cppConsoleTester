#include "stdafx.h"
#include <windows.h>
#include "test.h"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

/*������� ��������*/
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

//�������� ����� � �������� ������
char in[] = "����:\n";
char out[] = "�����:\n";
char delimeter[] = "---";

/*
 ������� ��������� ����� �� �����
 in - filename ��� ����� � �������
 out - tests ������ ��������, ���� ��������� �����
 return - ���-�� ������
*/
void readTests(char* filename, test* tests);

/*
 ������� ������� ���-�� ��������� ��������� substr � ������ str
*/
int countOfSubstr(char* str, char* substr);

/*
1. ������ ������ �� ���������� ����� � �������
2. ��������� ����������� exe � ���������� ��� ������ ��� �����
3. ��������� ��������� ���������� � ������� � ���������� ������������
 
*/



/*
	argv[1] - ��� ����������� ���������
	argv[2] - ��� ����� � �������
*/
int main(int argc, char* argv[])
{	
	//����� � ������� ����������� ������� ����
	system("chcp 1251 > nul");

	char **tests; //��� ����� ��������� ����� �� �����
	
	argv[1] = new char[100];
	argv[2] = new char[100];
	strcpy(argv[1], "sum.exe");
	strcpy(argv[2], "tests.txt");
	
	/* ���������, ��� ����������� ��������� � ���� � ������� ������������ � ������� ���������� */
	if (argc == 3)
	{
		//����������� "�������" exe
		FILE* exe = fopen(argv[1], "r");

		//���� �� ���������� - �������� ������������
		if (exe == NULL)
		{
			puts("exe-���� ����������!");
			fclose(exe);
			return -1;
		}
		else
			puts("exe ���� ���������!");


		//����������� ������� ���� � �������
		FILE* testsFile = fopen(argv[2], "r");

		//���� �� ���������� - �������� ������������
		if (testsFile == NULL)
		{
			puts("���� � ������� ����������!");
			fclose(testsFile);
			return -1;
		}
		else
			puts("���� � ������� ���������!");
	
		/*��������� ������ �� ����� � ������� � ���������� ������*/
		//���������� ��������� ������ ����� � ����� �����
		if (fseek (testsFile, 0, SEEK_END) == 0)
		{
			//�������� ���-�� ���� �� ������ ����� �� ��� ����� (�� ���� - ���-�� �������� � �����)
			int size = ftell (testsFile);

			//�������� ������ ��� ����� ������
			char * bufForTests = new char[size + 1];

			//���������� ��������� ������ ��������� ����� � ������
			fseek (testsFile, 0, SEEK_SET);

			//��������� ����� �� �����
			fread(bufForTests, sizeof(char), size, testsFile);
			bufForTests[size] = '\0';

			//��������� ���� � �������
			fclose(testsFile);

			//����������� ������ �� ����� � ������ ������
			int testsCount = countOfSubstr(bufForTests, delimeter);
			printf("���-�� ������ = %d\n", testsCount);
			test* tests = new test[testsCount];
			int currentTest = 0;
			char* ptr = strtok(bufForTests, delimeter);
			while(ptr != NULL)
			{
				if (currentTest != 0)
					ptr++;
				/*��������� ���� �� ������������: ���, ������, ������*/
				//��������
				int huy = strstr(ptr, "\n") - ptr;
				tests[currentTest].name = new char[strstr(ptr, "\n") - ptr];
				
				memcpy(tests[currentTest].name, ptr, strstr(ptr, "\n") - ptr);
				tests[currentTest].name[strstr(ptr, "\n") - ptr] = '\0';

				//������� ������
				char* inPtr = strstr(ptr, in) + strlen(in);
				char* outPtr = strstr(ptr, out);
				int inLength = (int)(outPtr - inPtr - 1); // - 1, ����� �� ��������� ������� �� ����� ������
				tests[currentTest].testIn = new char[inLength];
				memcpy(tests[currentTest].testIn, inPtr, inLength);
				tests[currentTest].testIn[inLength] = '\0';

				puts(tests[currentTest].name);
				puts(tests[currentTest].testIn);

				//�������� ������
				//�������� ��������� �� �������� ������, ����� ���� ������� � ������ ���������
				outPtr += strlen(out);
				tests[currentTest].testOut = new char[strlen(outPtr) - 1];// - 1, ����� �� ��������� ������� �� ����� ������
				strcpy(tests[currentTest].testOut, outPtr);
				tests[currentTest].testOut[strlen(tests[currentTest].testOut) - 1] = '\0';
				puts(tests[currentTest].testOut);
				puts("-------����� �����--------");
				//������� � ���������� �����
				ptr = strtok(NULL, delimeter);
				currentTest++;
			}

			//����������, ������������
			//���� �� ���� ������
			for (int i = 0; i < testsCount; i++)
			{
				char* result;
				//�������� ����������� exe � �������� ������� ���������� ����� � ��������� ��������� ���������
				runTest(argv[1], tests[i].testIn);

				//���� ��������� ��������� ������������� ����������
				if (strcmp(result, tests[i].testOut) != 0)
				{
					//������� �������� �����
					printf("%s - ",tests[i].name);

					//�������� ���� ������ �� �������
					HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
					SetConsoleTextAttribute(handle, FOREGROUND_GREEN);

					//������� ����� "�������"
					printf("�������\n");

					//������� ���� ������� - �����
					SetConsoleTextAttribute(handle, FOREGROUND_WHITE);
				}
				//����� (���� ���� �� �������)
				else
				{
					puts("-------------");
					//������� �������� �����
					printf("%s - ",tests[i].name);

					//�������� ���� ������ �� �������
					HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
					SetConsoleTextAttribute(handle, FOREGROUND_RED);

					//������� ����� "�� �������"
					printf("�� �������\n");
					SetConsoleTextAttribute(handle, FOREGROUND_WHITE);

					//�������, ��� ���������
					puts("���������:");
					puts(tests[i].testOut);

					//�������, ��� ������
					puts("������ �� �������:");
					puts(result);
					
					puts("-------------");
					
				}
				
			}
					
		}
		else
			puts("������ ������ �����");
	}
	//else
	{
		puts("�� �� ������� ����������� ��������� ��������� ������!");
		puts("��������� ��������� ���: tester.exe \"��� ������������ exe\" \"��� ����� � �������\" ");
	}

	return 0;
}


/*
 ������� ������� ���-�� ��������� ��������� substr � ������ str
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