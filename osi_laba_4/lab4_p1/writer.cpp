#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <random>
#include <ctime>

#pragma comment(lib, "winmm.lib")

using namespace std;

const char* text = "HelloThisIsBigData";
const int PAGE_NUM = 18;
const int PG_SIZE = 4096;

int main() {
	string s1 = "fileMapped";
	string s2 = "wSemaphore";
	string s3 = "rSemaphore";

	srand(time(NULL));
	HANDLE mappedHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, s1.c_str());

	std::fstream log;
	string logerName = "D:\\LOG_WRITER.txt";
	log.open(logerName, std::fstream::out | std::fstream::app);

	LPVOID mapFile = MapViewOfFile(mappedHandle, FILE_MAP_ALL_ACCESS, 0, 0, PAGE_NUM * PG_SIZE);
	if (!mapFile) cout << "error opening write file";
	HANDLE wSemaphore = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, s2.c_str());
	if (wSemaphore == 0) cout << "semaphore_error";
	HANDLE rSemaphore = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, s3.c_str());
	if (rSemaphore == 0) cout << "read_semaphore_error";
	HANDLE mtxLogFileArr[PAGE_NUM];

	for (int i = 0; i < PAGE_NUM; i++) {
		mtxLogFileArr[i] = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, ("mtx" + to_string(i)).c_str());
		if (mtxLogFileArr[i] == 0) cout << "writer_mutex_error " << i;
	}

	log << "Time: " << GetTickCount() << " || Writer thread ID: " << GetCurrentThreadId() << " || start " << endl;

	VirtualLock(mapFile, PG_SIZE * PAGE_NUM);

	for (int i = 0; i < 3; i++) {
		log << "Time: " << GetTickCount() << " || Writer thread ID: " << GetCurrentThreadId() << " || waiting for semaphore " << endl;
		
		if (wSemaphore != 0) WaitForSingleObject(wSemaphore, INFINITE);
		else cout << " write error";
		int index = WaitForMultipleObjects(PAGE_NUM, mtxLogFileArr, FALSE, INFINITE) - WAIT_OBJECT_0;
	
		void* start = (void*)((char*)mapFile + index * PG_SIZE);
		CopyMemory(start, text, PG_SIZE);
		
		log << "Time: " << GetTickCount() << " || Writer thread ID: " << GetCurrentThreadId() << " || writing page (num of page =  " << index << ") || DATA = " << text << endl;

		unsigned int pause = (rand() % 1000) + 500;
		Sleep((DWORD)pause);
		
		log << "Time: " << GetTickCount() << " || Writer thread ID: " << GetCurrentThreadId() << " || releasing sources" << endl;

		ReleaseMutex(mtxLogFileArr[index]);
		ReleaseSemaphore(rSemaphore, 1, NULL);
	}
	VirtualUnlock(mapFile, PG_SIZE * PAGE_NUM);

	for (int i = 0; i < PAGE_NUM; i++)
		CloseHandle(mtxLogFileArr[i]);
	log << "Time: " << GetTickCount() << " || Writer thread ID: " << GetCurrentThreadId() << " || exit" << endl;

	UnmapViewOfFile(mapFile);
	CloseHandle(mappedHandle);
	CloseHandle(wSemaphore);
	CloseHandle(rSemaphore);

	return 0;
}