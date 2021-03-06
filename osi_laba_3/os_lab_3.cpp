﻿#include <iostream>
#include <windows.h>
#include <locale.h>
#include <mutex>
#include <cassert>
#include <iomanip>

#pragma comment(lib, "winmm.lib")

 // данные, которые будут меняться разными потоками
struct commonData {
    long double piCounted;
    unsigned long offset;
};
typedef struct commonData comData;

// класс для создания мьютекса
class CAutoMutex
{
    HANDLE hMutex;

public:
    CAutoMutex() {
        hMutex = CreateMutex(NULL,      // атрибут безопастности
                             FALSE,     // флаг начального владельца
                             NULL);     // имя объекта
        assert(hMutex); // используется для дебага (сравнивает аргумент с нулем)
    }

    ~CAutoMutex() { CloseHandle(hMutex); }

    HANDLE get() { return hMutex; }
};

// класс для автоматического захвата и освобождения мьютекса
class CMutexLock
{
    HANDLE m_mutex;

public:
    // занимаем мютекс при конструировании объекта
    CMutexLock(HANDLE mutex) : m_mutex(mutex) // m_mutex(mutex) инициализирует члены класса перед выполнением тела конструктора
    {
        const DWORD res = WaitForSingleObject(m_mutex, INFINITE);
        assert(res == WAIT_OBJECT_0);
    }
    // освобождаем мютекс при удалении объекта
    ~CMutexLock()
    {
        const BOOL res = ReleaseMutex(m_mutex);
        assert(res);
    }
};

// функция, выполняемая потоками
void WINAPI ThreadCountPi(LPVOID lpParam);

// макрос, занимающий мютекс до конца области действия
#define SCOPE_LOCK_MUTEX(hMutex) CMutexLock _tmp_mtx_capt(hMutex);

static CAutoMutex g_mutex;
comData sharedInfo;
long double PI = 3.1415926535897932;
const int blockSize = 8306090;
const int N = 100000000;
HANDLE* hThreads;

using namespace std;

int main()
{
    DWORD start, finish;
    int threadsNum;
    int err;

    setlocale(LC_ALL, "Russian");
    
    cout << "Введите количество потоков: ";
    cin >> threadsNum;
    hThreads = new HANDLE[threadsNum];
    
    // потому что каждый поток изначально получает свой блок
    sharedInfo.offset = threadsNum * blockSize;

    for (int i = 0; i < threadsNum; i++) {
        hThreads[i] = CreateThread(NULL,                                        // дескриптор защиты (0 - не мб унаследован)
                                   0,                                           // начальный размер стека (0 - по умолчанию размер стека исполняемой программы)
                                   (LPTHREAD_START_ROUTINE)ThreadCountPi,       // указатель на LPTHREAD_START_ROUTINE, код которой исполняется потоком и обозначает начальный адрес потока
                                   (LPVOID)i,                                   // параметр потока
                                   CREATE_SUSPENDED,                            // опции создания (CREATE_SUSPENDED создается поток в состоянии ожидания, ждет ResumeThread)
                                   NULL);                                       // идентификатор потока
        assert(hThreads[i]);
    }

    //---------------------------------------------------------
    start = timeGetTime();

    for (int i = 0; i < threadsNum; i++) {
        err = ResumeThread(hThreads[i]);
        assert(err != -1);
    }

    WaitForMultipleObjects(threadsNum,          // число объектов в массиве hThreads
                           hThreads,            // указатель на массив дескрипторов
                           TRUE,                // флаг, указывающий надо ли дожидаться всех объектов или достаточно одного
                           INFINITE);           // таймаут
    
    sharedInfo.piCounted /= N;
    
    finish = timeGetTime();
    //---------------------------------------------------------

    cout << "Полученный результат: " << setprecision(15) << sharedInfo.piCounted << endl;
    cout << "Эталонное значение: " << PI << endl;
    cout << endl << "Время выполнения (мс): " << finish - start << endl;

    for (int i = 0; i < threadsNum; i++) {
        err = CloseHandle(hThreads[i]);
        assert(err);
    }

    delete[] hThreads;
    hThreads = nullptr;

    ExitProcess(0);
}

void WINAPI ThreadCountPi(LPVOID lpParam) {
    long double iterSum = 0.;
    long double x = 0.;
    unsigned long iterNum = (unsigned long)lpParam * blockSize;
    
    while (iterNum < N) {
        for (int i = iterNum; i < min(N, iterNum + blockSize); i++)
        {
            x = (i + 0.5) * 1./N;
            iterSum += (4. / (1. + x * x));
        }
        iterNum = sharedInfo.offset;
        SCOPE_LOCK_MUTEX(g_mutex.get()); 
        {
            sharedInfo.offset += blockSize;
            sharedInfo.piCounted += iterSum;
        }
        iterSum = 0.;
    }
}