#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <random>
#include <ctime>

#pragma comment(lib, "winmm.lib")

using namespace std;

const int PG_SIZE = 4096;
const int PAGE_NUM = 18;

int main() {

	string s1 = "fileMapped";
	string s2 = "wSemaphore";
	string s3 = "rSemaphore";
	char text[PG_SIZE];

	srand(time(NULL));
	HANDLE mappedHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, s1.c_str());

	fstream log;
	string logerName = "D:\\LOG_READER.txt";
	log.open(logerName, std::fstream::app | std::fstream::out);

	void* mapFile;
	mapFile = MapViewOfFile(mappedHandle, FILE_MAP_ALL_ACCESS, 0, 0, PAGE_NUM * PG_SIZE);
	if (!mapFile) cout << "error opening file";
	HANDLE wSemaphore = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, s2.c_str());
	if (wSemaphore == 0) cout << "semaphore error";
	HANDLE rSemaphore = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, s3.c_str());
	if (rSemaphore == 0) cout << "semaphore read error";
	HANDLE mtxLogFileArr[PAGE_NUM];

	for (int i = 0; i < PAGE_NUM; i++) {
		mtxLogFileArr[i] = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, ("mtx" + to_string(i)).c_str());
		if (mtxLogFileArr[i] == 0) cout << "reader_mutex_error " << i;
	}
	
	log << "Time: " << GetTickCount() << " || Reader thread ID: " << GetCurrentThreadId() << " || start" << endl;

	VirtualLock(mapFile, PG_SIZE * PAGE_NUM);

	for (int i = 0; i < 3; i++) {
		log << "Time: " << GetTickCount() << " || Reader thread ID: " << GetCurrentThreadId() << " || waiting for semaphore " << endl;
		if (rSemaphore != 0) WaitForSingleObject(rSemaphore, INFINITE);
		else cout << "read error";
		int num = WaitForMultipleObjects(PAGE_NUM, mtxLogFileArr, FALSE, INFINITE) - WAIT_OBJECT_0;

		void* start = (void*)((char*)mapFile + num * PG_SIZE);
		char data[PG_SIZE];
		CopyMemory(data, start, PG_SIZE);
		log << "Time: " << GetTickCount() << " || Reader thread ID: " << GetCurrentThreadId() << " || reading page (num of page =  " << num << ") "<< endl;
		
		unsigned int pause = (rand() % 1000) + 500;
		Sleep((DWORD)pause);

		log << "Time: " << GetTickCount() << " || Reader thread ID: " << GetCurrentThreadId() << " || releasing sources" << endl;
		
		ReleaseMutex(mtxLogFileArr[num]);
		ReleaseSemaphore(wSemaphore, 1, NULL);
		Sleep(10);
	}
	VirtualUnlock(mapFile, PG_SIZE * PAGE_NUM);
	for (int i = 0; i < PAGE_NUM; i++)
		CloseHandle(mtxLogFileArr[i]);
	
	log << "Time: " << GetTickCount() << " || Reader thread ID: " << GetCurrentThreadId() << " || exit" << endl;

	UnmapViewOfFile(mapFile);
	CloseHandle(mappedHandle);
	CloseHandle(wSemaphore);
	CloseHandle(rSemaphore);

	log.close();
	return 0;
}