// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

int main()
{
	CreateFile(
		L"C:\\1.txt",
		FILE_ALL_ACCESS,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
    return 0;
}

