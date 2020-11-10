#include <iostream>
#include <windows.h>
#include <locale.h>
#include <bitset>
#include <string>
#include <tchar.h>

#define MEMSIZE 4000

using namespace std;

int menu();

// 1 - получение информации о вычислительной системе
void myGetSysInfo();

// 2 - определение статуса виртуальной памяти
void myGlMemStatus();

// 3 - Определение состояния конкретного участка памяти по адресу
void myVirtQuery();

// 4 - резервирование региона
void myVirtAllocReserve();

// 5 - резервирование региона и передача ему физической памяти
void myVirtAllocCommit();

// 6 - запись данных в ячейки памяти по заданным адресам
void MyCopyMemory(TCHAR* buf, TCHAR* pbData, SIZE_T cbData, SIZE_T bufsize);
void myMemWrite();

// 7 - установка защиты доступа для заданного региона памяти
void kindOfProtect(DWORD prot);
void myVirtProtect();

// 8 - возврат физической памяти и освобождение региона адресного пространства
void myVirtFree();


int main() 
{
    setlocale(LC_ALL, "Russian");

    int key = -1;

    while (key != 0) {
        key = menu();
        system("cls");
        switch (key) {
            case 1:
                myGetSysInfo();
                system("pause");
                break;
            case 2:
                myGlMemStatus();
                system("pause");
                break;
            case 3:
                myVirtQuery();
                system("pause");
                break;
            case 4:
                myVirtAllocReserve();
                system("pause");
                break;
            case 5:
                myVirtAllocCommit();
                system("pause");
                break;
            case 6:
                myMemWrite();
                system("pause");
                break;
            case 7:
                myVirtProtect();
                system("pause");
                break;
            case 8:
                myVirtFree();
                system("pause");
                break;
            case 0:
                cout << endl << "До встречи!" << endl;
                break;
            default:
                cout << endl << "Неправильный ключ. Попробуйте еще раз." << endl;
                system("pause");
                break;
        }
    }

	return 0;
}

int menu() {
    int key;
    system("cls");
    cout << endl << "Выберите пункт меню для работы:" << endl;
    cout << "1 - Получение информации о вычислительной системе" << endl;
    cout << "2 - Определение статуса виртуальной памяти" << endl;
    cout << "3 - Определение состояния конкретного участка памяти по адресу" << endl;
    cout << "4 - Резервирование региона" << endl;
    cout << "5 - Резервирование региона и передача ему физической памяти" << endl;
    cout << "6 - Запись данных в ячейки памяти по заданным адресам" << endl;
    cout << "7 - Установка защиты доступа для заданного региона памяти" << endl;
    cout << "8 - Возврат физической памяти и освобождение региона адресного пространства" << endl;
    cout << "0 - Выход" << endl;

    cout << "Ваш выбор: ";
    cin >> key;

    return key;
}

// 1 - получение информации о вычислительной системе
void myGetSysInfo() {
    SYSTEM_INFO myInfo;

    GetSystemInfo(&myInfo);
    bitset<31> MyBs(myInfo.dwActiveProcessorMask);

    cout << endl << "Работа функции GetSystemInfo:" << endl;

    cout << endl << "   Архитектура системы процессора: ";
    switch (myInfo.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64:
        cout << "x64 (AMD/Intel)";
        break;
    case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
        cout << "WOW64";
        break;
    case PROCESSOR_ARCHITECTURE_IA64:
        cout << "Intel Itanium Processor Family (IPF)";
        break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        cout << "x86";
        break;
    case PROCESSOR_ARCHITECTURE_UNKNOWN:
        cout << "unknown";
        break;
    default:
        cout << "Error!";
        break;
    }

    cout << endl << "   Размер страницы и гранулярность страничной защиты и обязательства: " << myInfo.dwPageSize;
    cout << endl << "   Указатель на младший адрес памяти, доступный приложениям и DLL: " << myInfo.lpMinimumApplicationAddress;
    cout << endl << "   Указатель на старший адрес памяти, доступный приложениям и DLL: " << myInfo.lpMaximumApplicationAddress;
    cout << endl << "   Маска, представляющая набор процессоров, сконфигурированных в системе: " << myInfo.dwActiveProcessorMask
        << " (" << MyBs << ")";
    cout << endl << "   Количество процессоров в системе: " << myInfo.dwNumberOfProcessors;
    cout << endl << "   Гранулярность для начального адреса (для виртуальной памяти): " << myInfo.dwAllocationGranularity;
    cout << endl << "   Уровень архитектурно-зависимого прицессора системы: " << myInfo.wProcessorLevel;
    cout << endl << "   Ревизия архитектурно-зависимого процессора: " << myInfo.wProcessorRevision;

    cout << endl << endl;
}

// 2 - определение статуса виртуальной памяти
void myGlMemStatus() {
    MEMORYSTATUSEX statex;
    long retval;

    statex.dwLength = sizeof(statex);

    cout << endl << "Работа функции GlobalMemoryStatusEx: " << endl;
    retval = GlobalMemoryStatusEx(&statex);
    
    cout << endl << "   Процент используемой памяти: " << statex.dwMemoryLoad << "%";
    cout << endl << "   Общий объем физической памяти: " << statex.ullTotalPhys << " ("
        << long double(statex.ullTotalPhys) / 1024 / 1024 / 1024 << " Gb)";
    cout << endl << "   Объем доступной физической памяти: " << statex.ullAvailPhys << " ("
        << long double(statex.ullAvailPhys) / 1024 / 1024 / 1024 << " Gb)";
    cout << endl << "   Размер файла подкачки в байтах: " << statex.ullTotalPageFile << " ("
        << long double(statex.ullTotalPageFile) / 1024 / 1024 / 1024 << " Gb)";
    cout << endl << "   Доступный объем байтов в файле подкачки: " << statex.ullAvailPageFile << " ("
        << long double(statex.ullAvailPageFile) / 1024 / 1024 / 1024 << " Gb)";
    cout << endl << "   Общий объем виртуальной памяти в байтах: " << statex.ullTotalVirtual << " ("
        << long double(statex.ullTotalVirtual) / 1024 / 1024 / 1024 << " Gb)";
    cout << endl << "   Объем доступной виртуальной памяти: " << statex.ullAvailVirtual << " ("
        << long double(statex.ullAvailVirtual) / 1024 / 1024 / 1024 << " Gb)";
    cout << endl << "   Объем незарезервированной памяти в расширенной части виртуальной памяти в байтах: " << statex.ullAvailExtendedVirtual;

    cout << endl << endl;
}

// 3 - определение состояния конкретного участка памяти по адресу
void myVirtQuery() {
    MEMORY_BASIC_INFORMATION myMem;
    int orig;
    long retval;

    cout << endl << "Работа функции VirtualQuery: " << endl;
    cout << endl << "   Введите адрес для работы (HEX): ";
    cin >> hex >> orig;

    retval = VirtualQuery((LPCVOID)orig, &myMem, sizeof(myMem));
    if (!retval) cout << endl <<"   Programm fails with code " << GetLastError();
    else {
        cout << endl <<"   Указатель на базовый адрес региона страниц: " << myMem.BaseAddress << endl;
        cout << "   Указатель на базовый адрес диапазона страниц, выделенных пользователем: " << myMem.AllocationBase << endl;
        cout << "   Доступна ли память при первоначальном выделении области (да / нет): ";
        (myMem.AllocationProtect == 0) ? cout << "нет" : cout << "да"; 
        cout << endl;
        cout << "   Размер региона, начинающейся с базового адреса с одинаковыми атрибутами защиты: " << myMem.RegionSize << " байт " 
            << " (" << long double(myMem.RegionSize) / 1024 / 1024  << " Mb)" << endl;

        if (myMem.State & MEM_COMMIT)
        {
            cout << endl << "   Состояние страниц в регионе: ";
            cout << endl << "      Зафиксированная страница (MEM_COMMIT)";
        }
        if (myMem.State & MEM_RESERVE) {
            cout << endl << "   Состояние страниц в регионе: ";
            cout << endl << "      Зарезервированные страницы, на которых зарезервирован диапазон виртуального адресного "
                 << endl << "      пространства процесса без выделения какого - либо физического хранилища (MEM_RESERVE)";
        }
        if (myMem.State & MEM_FREE) {
            cout << endl << "   Состояние страниц в регионе: ";
            cout << endl << "      Свободные страницы, недоступные для вызывающего процесса и доступные для выделения (MEM_FREE)";
        }
        if (myMem.Type & MEM_IMAGE) {
            cout << endl << endl << "   Тип физической памяти, связанной с группой смежных страниц: ";
            cout << endl << "      Страницы памяти в пределах региона отображаются в виде раздела изображения";
        }
        if (myMem.Type & MEM_MAPPED) {
            cout << endl << endl << "   Тип физической памяти, связанной с группой смежных страниц: ";
            cout << endl << "      Страницы памяти в пределах региона сопоставляются с представлением раздела";
        }
        if (myMem.Type & MEM_PRIVATE) {
            cout << endl << endl << "   Тип физической памяти, связанной с группой смежных страниц: ";
            cout << endl << "      Страницы памяти в пределах региона являются частными";
        }
    }
    cout << endl << endl;
}

// 4 - резервирование региона
void myVirtAllocReserve() {
    int ans, orig;
    void* p = NULL;

    cout << endl << "Работа функции VirtualAlloc: " << endl;

    cout << endl << "   Выберите какой тип резервирования памяти будет использоваться: " ;
    cout << endl << "   1 - автоматический \n   2 - режим ввода адреса начала региона" << endl;
    cout << "   Ваш выбор: ";
    cin >> ans;
    
    switch (ans)
    {
        case 1:
            p = VirtualAlloc(NULL, MEMSIZE, MEM_RESERVE | MEM_TOP_DOWN, PAGE_READONLY);
            break;
        case 2:
            cout << endl << "   Введите адрес для работы (HEX): ";
            cin >> hex >> orig;
            p = VirtualAlloc((LPVOID)orig, MEMSIZE, MEM_RESERVE, PAGE_READONLY);
            break;
        default:
            cout << "Wrong key!";
            break;
    }

    if (p)
        cout << endl << "   Базовый адрес зарезервированной памяти: " << p;
    else 
        cout << endl << "   Ошибка резервирования памяти. ";

    cout << endl << endl;
}

// 5 - резервирование региона и передача ему физической памяти
void myVirtAllocCommit() {
    int ans, orig;
    void* p = NULL;

    cout << endl << "Работа функции VirtualAlloc: " << endl;

    cout << endl << "   Выберите какой тип резервирования памяти будет использоваться: ";
    cout << endl << "   1 - автоматический \n   2 - режим ввода адреса начала региона" << endl;
    cout << "   Ваш выбор: ";
    cin >> ans;

    switch (ans)
    {
    case 1:
        p = VirtualAlloc(NULL, MEMSIZE, MEM_RESERVE | MEM_TOP_DOWN | MEM_COMMIT, PAGE_READONLY);
        break;
    case 2:
        cout << endl << "   Введите адрес для работы (HEX): ";
        cin >> hex >> orig;
        p = VirtualAlloc((LPVOID)orig, MEMSIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READONLY);
        break;
    default:
        cout << "Wrong key!";
        break;
    }

    if (p)
        cout << endl << "   Виртуальный адрес зарезервированного региона, которому была передана физическая память: " << p;
    else
        cout << endl << "   Ошибка резервирования памяти. ";

    cout << endl << endl;
}

// 6 - запись данных в ячейки памяти по заданным адресам
void myMemWrite() {
    PVOID orig;
    string dataToWrite;
    MEMORY_BASIC_INFORMATION myMem;

    cout << endl << "   Введите адрес для работы (HEX): ";
    cin >> hex >> orig;


    VirtualQuery((LPCVOID)orig, &myMem, sizeof(myMem));
    if (!((myMem.Protect & PAGE_READONLY) || (myMem.Protect & PAGE_NOACCESS))) {
        cout << endl << "   Введите данные, которые хотите записать по адресу (press 'Enter' to finish):\n   ";
        cin.ignore();
        getline(cin, dataToWrite);
        CopyMemory(orig, (char*)dataToWrite.c_str(), dataToWrite.length() * sizeof(char));

        cout << endl << "   Данные, записанные по заданному адресу: \n   " << (char*)orig;
    }
    else
       cout << "   У программы нет доступа к этому адресу или запись запрещена." << endl;

    cout << endl << endl;
}

// 7 - установка защиты доступа для заданного региона памяти
void kindOfProtect(DWORD prot) {
    switch (prot)
    {
        case PAGE_GUARD:
            cout << "PAGE_GUARD";
            break;
        case PAGE_NOCACHE:
            cout << "PAGE_NOCACHE";
            break;
    }
    switch (prot)
    {
        case PAGE_READWRITE:
            cout << "PAGE_READWRITE";
            break;
        case PAGE_READONLY:
            cout << "PAGE_READONLY";
            break;
        case PAGE_EXECUTE:
            cout << "PAGE_EXECUTE";
            break;
        case PAGE_EXECUTE_READ:
            cout << "PAGE_EXECUTE_READ";
            break;
        case PAGE_EXECUTE_READWRITE:
            cout << "PAGE_EXECUTE_READWRITE";
            break;
        case PAGE_NOACCESS:
            cout << "PAGE_NOACCESS";
            break;
    }
}

void myVirtProtect() {
    int ans, orig;
    DWORD myOldProtect, myNewProtect = 0;
    void* p = NULL;

    cout << endl << "Работа функции VirtualProtect: " << endl;
    cout << endl << "   Введите адрес для работы (HEX): ";
    cin >> hex >> orig;

    cout << endl << "   Выберите новый тип разрешенного доступа: " << endl;
    cout << endl << "   1 - Чтение и запись (PAGE_READWRITE)";
    cout << endl << "   2 - Только чтение (PAGE_READONLY)";
    cout << endl << "   3 - Только исполнение программного кода (PAGE_EXECUTE)";
    cout << endl << "   4 - Исполнение и чтение (PAGE_EXECUTE_READ)";
    cout << endl << "   5 - Исполнение, чтение и запись (PAGE_EXECUTE_READWRITE)";
    cout << endl << "   6 - Запрещен любой вид доступа (PAGE_NOACCESS)";
    cout << endl << "   7 - Сигнализация доступа к старнице (PAGE_GUARD)";
    cout << endl << "   8 - Отмена кэширования для страницы памяти (PAGE_NOCACHE)";
    cout << endl << "   Ваш ответ: ";
    cin >> ans;

    switch (ans)
    {
        case 1:
            myNewProtect = PAGE_READWRITE;
            break;
        case 2:
            myNewProtect = PAGE_READONLY;
            break;
        case 3:
            myNewProtect = PAGE_EXECUTE;
            break;
        case 4:
            myNewProtect = PAGE_EXECUTE_READ;
            break;
        case 5:
            myNewProtect = PAGE_EXECUTE_READWRITE;
            break;
        case 6:
            myNewProtect = PAGE_NOACCESS;
            break;
        case 7:
            myNewProtect |= PAGE_GUARD;
            break;
        case 8:
            myNewProtect |= PAGE_NOCACHE;
            break;
    }

    VirtualProtect((LPVOID)orig, MEMSIZE, myNewProtect, &myOldProtect);

    cout << endl << endl << "   Старый разрешенный тип доступа: ";
    kindOfProtect(myOldProtect);

    cout << endl << endl << "   Новый разрешенный тип доступа: ";
    kindOfProtect(myNewProtect);

    cout << endl << endl;
}

// 8 - возврат физической памяти и освобождение региона адресного пространства
void myVirtFree() {
    bool ok;
    int orig;
    MEMORY_BASIC_INFORMATION myMem;

    cout << endl << "Работа функции VirtualFree: " << endl;
    cout << endl << "   Введите адрес для работы (HEX): ";
    cin >> hex >> orig;


    VirtualQuery((LPCVOID)orig, &myMem, sizeof(myMem));
    if (!(myMem.Protect & PAGE_NOACCESS))
    {
        if (VirtualFree((LPVOID)orig, 0, MEM_RELEASE))
            cout << endl << "   Память была освобождена." << endl;
        else
            cout << endl << "   Ошибка! Код ошибки: " << GetLastError();
    }
    else
        cout << endl << "    У программы нет доступа к этому адресу." << endl;

    cout << endl << endl;
}