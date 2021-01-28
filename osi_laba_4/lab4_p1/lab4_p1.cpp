#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int PC_NUM = 10;
const int PAGE_NUM = 18;
const int PG_SIZE = 4096;

int main() {
	PROCESS_INFORMATION WriterInfo[PC_NUM];
	PROCESS_INFORMATION ReaderInfo[PC_NUM];
	HANDLE mtxLogFileArr[PAGE_NUM];

	string s2 = "wSemaphore";
	string s3 = "rSemaphore";

	HANDLE wSemaphore = CreateSemaphoreA(NULL, PAGE_NUM, PAGE_NUM, s2.c_str());
	if (!wSemaphore) cout << "Error create writer semaphore. Error code: " << GetLastError();

	HANDLE rSemaphore = CreateSemaphoreA(NULL, 0, PAGE_NUM, s3.c_str());
	if (!rSemaphore) cout << "Error create reader semaphore. Error code: " << GetLastError();

	for (int i = 0; i < PAGE_NUM; i++) {
		mtxLogFileArr[i] = CreateMutexA(NULL, false, ("mtx" + to_string(i)).c_str());
		if (!mtxLogFileArr[i]) cout << "Error create " << i << "mutex. Error code: " << GetLastError() << endl;
	}

	HANDLE sharedFile = CreateFileA("shareFile.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (sharedFile) cout << "File created." << endl;

	HANDLE mappedHandle;
	mappedHandle = CreateFileMappingA(sharedFile, NULL, PAGE_READWRITE, 0, PAGE_NUM * PG_SIZE, "fileMapped");
	if (!mappedHandle) cout << "Error create file mapping. Error code: " << GetLastError() << endl;

	void* mapFile;
	mapFile = MapViewOfFile(mappedHandle, FILE_MAP_ALL_ACCESS, 0, 0, PAGE_NUM * PG_SIZE);
	if (!mapFile) cout << "Error map view of file code " << GetLastError();

	ZeroMemory(mapFile, PAGE_NUM * PG_SIZE);
	bool createWithError = false;
	for (int i = 0; i < PC_NUM; i++) {
		STARTUPINFOA writerStartInfo = { sizeof(writerStartInfo) };
		STARTUPINFOA readerStartInfo = { sizeof(writerStartInfo) };
		if (!CreateProcessA((LPCSTR)"C:\\os_4\\lab4_p1\\Debug\\reader.exe", NULL, NULL, NULL,
			TRUE, 0, NULL, NULL, &writerStartInfo, &WriterInfo[i]))
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
		WaitForSingleObject(WriterInfo[i].hProcess, INFINITE);
		WaitForSingleObject(ReaderInfo[i].hProcess, INFINITE);
	}

	UnmapViewOfFile(mapFile);
	CloseHandle(mappedHandle);
	cout << "All readers and writers finished their work." << endl;

	Sleep(1000);
	return 0;
}