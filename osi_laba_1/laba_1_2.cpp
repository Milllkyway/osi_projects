#include <iostream>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <string.h>

#pragma comment(lib, "winmm.lib")

#define ONEBLOCK 4096 // размер кластера диска

/*typedef struct _OVERLAPPED {
	ULONG_PTR Internal;				// используется системой
	ULONG_PTR InternalHigh;			// используется системой
	DWORD Offset;					// смещение байта от начала файла 
	DWORD OffsetHigh;				// !0 когда работаем с файлом
	HANDLE hEvent;					// дескриптор события, которые должно быть установлено в сигнальное состояние, когда операция завершилась
} OVERLAPPED;*/						// ReadFile и WriteFile устанавливают этот дескриптор в несигнальное состояние прежде, чем они начинают I/O

HANDLE inFileHandle, outFileHandle;
OVERLAPPED *ovrIn, *ovrOut; // содержит информацию, используемую в асинхронном I/O
CHAR** buf;
LONGLONG nRecords, nDoneRead, nDoneWrite;
LARGE_INTEGER fileSize;
INT operatNum, totalBlock;

// вызываются функциями, в которых они указаны
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

		totalBlock = ONEBLOCK * blocksNum; // итоговый размер блоков

		inFileHandle = CreateF(true); // открывает существующий файл
		outFileHandle = CreateF(false); // создает новый файл

		if ((inFileHandle != INVALID_HANDLE_VALUE) && (outFileHandle != INVALID_HANDLE_VALUE))
		{
			LARGE_INTEGER curPos;
			buf = new CHAR * [totalBlock];

			for (int k = 0; k < operatNum; k++)
				buf[k] = new CHAR[totalBlock]; //  массив для размещения в нем кодов N символов с указателем buf на его начало

			ovrIn = new OVERLAPPED[totalBlock];
			ovrOut = new OVERLAPPED[totalBlock];

			BY_HANDLE_FILE_INFORMATION fileInformation;

			// заполняем fileInformation для вычисления fileSize
			if (!GetFileInformationByHandle(inFileHandle, &fileInformation)){
				cout << "Error!";
				return 0;
			}

			fileSize.LowPart = (fileInformation.nFileSizeHigh *(MAXDWORD + 1)) + fileInformation.nFileSizeLow;
			cout << endl << "Размер файла: " << (fileSize.QuadPart / 1024) + 1<< " Kb" << endl;
			
			// QuadPart	oпределяет 64-х битное знаковое целое для large_integer
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
				SleepEx(INFINITE, TRUE); // приостанавливает исполнение текущего потока до тех пор, пока не закончится I/O колбэка

			LONGLONG nBytes = nRecords * totalBlock; // общий размер блоков (всех вместе)
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