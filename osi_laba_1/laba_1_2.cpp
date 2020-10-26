#include <iostream>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <string.h>

#pragma comment(lib, "winmm.lib")

#define ONEBLOCK 4096 // размер кластера диска

HANDLE inFileHandle, outFileHandle;
OVERLAPPED *ovrIn, *ovrOut;
CHAR** buf;
LONGLONG nRecords, nDoneRead, nDoneWrite;
LARGE_INTEGER fileSize;
INT operatNum, totalBlock;

VOID WINAPI ReadCallback(DWORD error, DWORD numberOfBytes, LPOVERLAPPED pOvr);
VOID WINAPI WriteCallback(DWORD error, DWORD numberOfBytes, LPOVERLAPPED pOvr);

HANDLE CreateF(bool);

using namespace std;

int main()
{
	int blocksNum, ans = 1, key;
	string fileToCopyPath, fileToCopyName, newFileName;
	DWORD start, finish;
	BOOL success;
	
	setlocale(LC_ALL, "Russian");

	while (ans == 1) 
	{
		system("cls");

		cout << endl << "Введите множитель для размера блока (1 блок = 4096 байт): ";
		cin >> blocksNum;
		cout << "Выберите число перекрывающихся операций ввода/вывода (1, 2, 4, 8, 12, 16): ";
		cin >> operatNum;
		cout << endl;

		totalBlock = ONEBLOCK * blocksNum;

		inFileHandle = CreateF(true);
		outFileHandle = CreateF(false);

		if ((inFileHandle != INVALID_HANDLE_VALUE) && (outFileHandle != INVALID_HANDLE_VALUE))
		{
			LARGE_INTEGER curPos;
			buf = new CHAR * [totalBlock];

			for (int k = 0; k < operatNum; k++)
				buf[k] = new CHAR[totalBlock];

			ovrIn = new OVERLAPPED[totalBlock];
			ovrOut = new OVERLAPPED[totalBlock];

			BY_HANDLE_FILE_INFORMATION fileInformation;

			if (!GetFileInformationByHandle(inFileHandle, &fileInformation))
			{
				cout << "Error!";
				return 0;
			}

			fileSize.LowPart = (fileInformation.nFileSizeHigh *(MAXDWORD + 1)) + fileInformation.nFileSizeLow;
			cout << endl << "Размер файла: " << (fileSize.QuadPart / 1024) + 1<< " Kb" << endl;

			nRecords = fileSize.QuadPart / totalBlock + (fileSize.QuadPart % totalBlock > 0 ? 1 : 0);
			cout << "Число записей: " << nRecords << endl;

			curPos.QuadPart = 0;
			success = 0;

			start = timeGetTime();

			for (int i = 0; i < operatNum; i++)
			{
				ovrIn[i].hEvent = (HANDLE)i;
				ovrOut[i].hEvent = (HANDLE)i;
				ovrIn[i].Offset = curPos.LowPart;
				ovrIn[i].OffsetHigh = curPos.HighPart;

				if (curPos.QuadPart < fileSize.QuadPart) {
					key = ReadFileEx(inFileHandle, buf[i], totalBlock, &ovrIn[i], (LPOVERLAPPED_COMPLETION_ROUTINE)ReadCallback);
					if (!key) 
						cout << endl << "Error in main ReadFileEx";
				}
					
				curPos.QuadPart = curPos.QuadPart + (LONGLONG)totalBlock;
			}

			nDoneRead = 0;
			while (nDoneRead < nRecords)
				SleepEx(INFINITE, TRUE);

			LONGLONG nBytes = nRecords * totalBlock;

			SetFilePointer(outFileHandle, -(nBytes - fileSize.QuadPart), NULL, FILE_END);
			SetEndOfFile(outFileHandle);

			finish = timeGetTime();

			cout << endl << "Время копирования в миллисекундах: " << finish - start << endl;

			CloseHandle(inFileHandle);
			CloseHandle(outFileHandle);

			delete[] ovrIn;
			delete[] ovrOut;

			for (int i = 0; i < operatNum; i++)
				delete[] buf[i];

			delete[] buf;
		}
		else
			cout << endl << "Error opening file." << endl;

		cout << endl << "0 - Выйти из программы \n1 - Продолжить \nВаш ответ: ";
		cin >> ans;
	}
	return 0;
}

HANDLE CreateF(bool flag)
{
	char name[MAX_PATH + 1];

	flag ? cout << "Введите имя исходного файла: " : cout << "Введите имя файла результата: ";

	cin >> name;

	HANDLE FileHandle = CreateFileA(name, flag ? GENERIC_READ : GENERIC_WRITE, 0,
		NULL, flag ? OPEN_EXISTING : CREATE_ALWAYS, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);

	return FileHandle;
}

VOID CALLBACK ReadCallback(DWORD error, DWORD numberOfBytes, LPOVERLAPPED pOvr)
{
	LARGE_INTEGER curPosIn;
	DWORD k = (DWORD)(pOvr->hEvent);
	int key;


	curPosIn.LowPart = ovrIn[k].Offset;
	curPosIn.HighPart = ovrIn[k].OffsetHigh;

	ovrOut[k].Offset = ovrIn[k].Offset;
	ovrOut[k].OffsetHigh = ovrIn[k].OffsetHigh;

	key = WriteFileEx(outFileHandle, buf[k], totalBlock, &ovrOut[k], (LPOVERLAPPED_COMPLETION_ROUTINE)WriteCallback);
	
	if (key) {
		curPosIn.QuadPart += totalBlock * (LONGLONG)(operatNum);

		ovrIn[k].Offset = curPosIn.LowPart;
		ovrIn[k].OffsetHigh = curPosIn.HighPart;

		nDoneRead++;
		//cout << "It's okay! in ReadCallback";
	}
	else
		cout << endl << "Error in ReadCallback";
}

VOID CALLBACK WriteCallback(DWORD error, DWORD numberOfBytes, LPOVERLAPPED pOvr)
{
	int key;
	LARGE_INTEGER curPos;
	DWORD k = (DWORD)(pOvr->hEvent);

	curPos.LowPart = ovrIn[k].Offset;
	curPos.HighPart = ovrIn[k].OffsetHigh;

	if (curPos.QuadPart < fileSize.QuadPart) {
		key = ReadFileEx(inFileHandle, buf[k], totalBlock, &ovrIn[k], (LPOVERLAPPED_COMPLETION_ROUTINE)ReadCallback);
		if (key) {
			nDoneWrite++;
			//cout << "It's okay in WriteCallback";
		}
		else
			cout << endl << "Error in WriteCallback";
	}

}