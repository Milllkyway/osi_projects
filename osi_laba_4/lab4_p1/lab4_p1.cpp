#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int PC_NUM = 10;
const int PAGE_NUM = 18;
const int PG_SIZE = 4096;

int main() {
	PROCESS_INFORMATION WriterInfo[PC_NUM]; // заполняется функцией CreateProcess с информацией о недавно созданном процессе и его первичном потоке.
	PROCESS_INFORMATION ReaderInfo[PC_NUM];
	HANDLE mtxLogFileArr[PAGE_NUM];

	string s2 = "wSemaphore";
	string s3 = "rSemaphore";

	HANDLE wSemaphore = CreateSemaphoreA(NULL,				// Указатель на структуру SECURITY_ATTRIBUTES (0 - не мб наследован)
										 PAGE_NUM,			// Начальное число счетчика для объекта семафора (-- когда функция ожидания освобождает поток, ++ при release)
										 PAGE_NUM,			// Максимальное число для объекта семафора
										 s2.c_str());
	if (!wSemaphore) cout << "Error create writer semaphore. Error code: " << GetLastError();

	HANDLE rSemaphore = CreateSemaphoreA(NULL, 0, PAGE_NUM, s3.c_str());
	if (!rSemaphore) cout << "Error create reader semaphore. Error code: " << GetLastError();

	// создаем массив мьютексов для работы со страницами
	for (int i = 0; i < PAGE_NUM; i++) {
		mtxLogFileArr[i] = CreateMutexA(NULL, false, ("mtx" + to_string(i)).c_str()); // не мб унаследован, вызывающий поток не получает права собствнности на мьютех
		if (!mtxLogFileArr[i]) cout << "Error create " << i << "mutex. Error code: " << GetLastError() << endl;
	}

	HANDLE sharedFile = CreateFileA("shareFile.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // 1 - не мб унаследован, 2 - дескриптор шаблона файла (дб NULL)
	if (sharedFile) cout << "File created." << endl;

	HANDLE mappedHandle;
	mappedHandle = CreateFileMappingA(sharedFile, NULL, PAGE_READWRITE, 0, PAGE_NUM * PG_SIZE, "fileMapped"); // 1 - не мб унаследован, 2 - старшее слово max размера
	if (!mappedHandle) cout << "Error create file mapping. Error code: " << GetLastError() << endl;

	void* mapFile;
	mapFile = MapViewOfFile(mappedHandle, FILE_MAP_ALL_ACCESS, 0, 0, PAGE_NUM * PG_SIZE); // старшее и младшее слово смещения, последнее - число отображаемых байтов
	if (!mapFile) cout << "Error map view of file code " << GetLastError();

	ZeroMemory(mapFile, PAGE_NUM * PG_SIZE); // заполняем ячейки памяти нулями
	bool createWithError = false;
	for (int i = 0; i < PC_NUM; i++) {
		STARTUPINFOA writerStartInfo = { sizeof(writerStartInfo) };
		STARTUPINFOA readerStartInfo = { sizeof(writerStartInfo) };
		if (!CreateProcessA((LPCSTR)"C:\\os_4\\lab4_p1\\Debug\\reader.exe",
							 NULL,			   //  определяет командную строку для выполнения (0 - из 1 параметра)
							 NULL,  
							 NULL,  
				  			 TRUE,			   // каждый наследуемый дескриптор в вызывающем процессе наследуется новым процессом
							 NULL,
							 NULL,			   // новый процесс использует конфигурацию вызывающего процесса.
							 &writerStartInfo, // информация предустановки
							 &WriterInfo[i]))  // принимает идентифицирующую информацию о новом процессе
							 createWithError = true;
		if (!CreateProcessA((LPCSTR)"C:\\os_4\\lab4_p1\\Debug\\writer.exe", NULL, NULL, NULL,
			TRUE, 0, NULL, NULL, &readerStartInfo, &ReaderInfo[i]))
			createWithError = true;
	}

	if (createWithError)
		cout << "Process create error: " << GetLastError() << endl;
	else
		cout << "Readers and writers created successfully." << endl;
	cout << "Start work of readers and writers." << endl;
	for (int i = 0; i < PC_NUM; i++) {
		WaitForSingleObject(WriterInfo[i].hProcess, INFINITE); // приостанавливает поток до перехода объекта в сигнальное состояние
		WaitForSingleObject(ReaderInfo[i].hProcess, INFINITE);
	}

	UnmapViewOfFile(mapFile);
	CloseHandle(mappedHandle);
	cout << "All readers and writers finished their work." << endl;

	Sleep(1000);
	return 0;
}