#include <iostream>
#include <windows.h>
#include <locale.h>
#include <omp.h>
#include <iomanip>

#pragma comment(lib, "winmm.lib")

using namespace std;

int main()
{
    long double PI = 3.1415926535897932;
    const int blockSize = 8306090;
    const int N = 100000000;
    long double piCounted = 0.;
    int threadsNum;
    DWORD start, finish;

    setlocale(LC_ALL, "Russian");

    cout << "Введите количество потоков: ";
    cin >> threadsNum;

    // reduction необходима для предоставления частных копий piCounted для каждого потока
    // schedule необходима для динамического планирования по блокам нужного размера
    omp_set_num_threads(threadsNum);
    
    start = timeGetTime();

    #pragma omp parallel for schedule(dynamic,blockSize) reduction (+:piCounted)
    for (int i = 0; i < N; i++) {
        // x должна быть частной, поэтому объявляеться локально
        long double x = (i + 0.5) * 1. / N;
        piCounted += (4. / (1. + x * x));
    }
    piCounted /= N;

    finish = timeGetTime();


    cout << "Полученный результат: " << setprecision(15) << piCounted << endl;
    cout << "Эталонное значение: " << PI << endl;
    cout << endl << "Время выполнения (мс): " << finish - start << endl;

    ExitProcess(0);

    return 0;
}
