#include "stdafx.h"
#include <windows.h>
#include <string.h>
#include "test.h"


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
	strcpy(argv[1], "tester1.exe");
	strcpy(argv[2], "tests.txt");
	/* ���������, ��� ����������� ��������� � ���� � ������� ������������ � ������� ���������� */
	//if (argc == 3)
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
			while(ptr != NULL && testsCount > 0)
			{
				if (currentTest != 0)
					ptr++;
				/*��������� ���� �� ������������: ���, ������, ������*/
				//��������
				int huy= strstr(ptr, "\n") - ptr;
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
				testsCount--;
				
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