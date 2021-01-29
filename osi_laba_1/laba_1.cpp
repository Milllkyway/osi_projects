#include <iostream>
#include <windows.h>
#include <locale.h>
#include <bitset>
#include <string.h>

using namespace std;

int menu();

// 1 - вывод списка дисков
void myLogicalDrives();
void myDriveString();

// 2 - вывод инфы о диске и размер свободного пространства
void myDriveType(string disk);
void myVolumeInformation(string disk);
void myDiskFreeSpace(string disk);

// 3 - создание и удаление заданных каталогов
void myCreateDirectory();
void myRemoveDirectory();

// 4 - создание файлов в новых каталогах
void myCreateFile();

// 5 - копирование и перемещение файлов между каталогами
void myCopyFile();
void myMoveFile();
void myMoveFileEx();

// 6 - анализ и изменение атрибутов файлов
void myGetFileAt();
void mySetFileAt();
void myGetFileInfo();
void myGetFileTime();
void mySetFileTime();

int main()
{
    setlocale(LC_ALL, "Russian");
    
    int key = -1, ans = -1; 
    string disk;

    while (key != 0) {
        key = menu();
        system("cls");
        switch (key) {
        case 1:
            myLogicalDrives();
            myDriveString();
            system("pause");
            break;
        case 2:
            cout << endl << "Введите имя диска (пример: с:\\) для работы: ";
            cin >> disk;

            myDriveType(disk);
            myVolumeInformation(disk);
            myDiskFreeSpace(disk);
            system("pause");
            break;
        case 3:
            cout << endl << "1 - Создать директорию";
            cout << endl << "2 - Удалить директорию";
            cout << endl << "Ваш ответ: ";
            cin >> ans;

            switch (ans) {
            case 1:
                myCreateDirectory();
                break;
            case 2:
                myRemoveDirectory();
                break;
            default:
                cout << "Неправильный ключ." << endl;
                break;
            }
            system("pause");
            break;
        case 4:
            myCreateFile();
            system("pause");
            break;
        case 5:
            cout << endl << "1 - Скопировать файл";
            cout << endl << "2 - Переместить файл";
            cout << endl << "3 - Переместить файл с доп.параметрами";
            cout << endl << "Ваш ответ: ";
            cin >> ans;

            switch (ans) {
            case 1:
                myCopyFile();
                break;
            case 2:
                myMoveFile();
                break;
            case 3:
                myMoveFileEx();
                break;
            default:
                cout << "Неправильный ключ." << endl;
                break;
            }
            system("pause");
            break;
        case 6:
            cout << endl << "1 - Посмотреть атрибуты";
            cout << endl << "2 - Установить атрибуты";
            cout << endl << "3 - Получить системную информацию о файле";
            cout << endl << "4 - Получить информацию о времени создания и т.п. файла";
            cout << endl << "5 - Установить время файла";
            cout << endl << "Ваш ответ: ";
            cin >> ans;

            switch (ans) {
            case 1:
                myGetFileAt();
                break;
            case 2:
                mySetFileAt();
                break;
            case 3:
                myGetFileInfo();
                break;
            case 4:
                myGetFileTime();
                break;
            case 5:
                mySetFileTime();
                break;
            default:
                cout << "Неправильный ключ." << endl;
                break;
            }
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
    cout << "1 - Вывод списка дисков" << endl;
    cout << "2 - Вывод информации о диске и размер свободного пространства" << endl;
    cout << "3 - Создание и удаление заданных каталогов" << endl;
    cout << "4 - Создание файлов в новых каталогах" << endl;
    cout << "5 - Копирование и перемещение файлов между каталогами" << endl;
    cout << "6 - Анализ и изменение атрибутов файлов" << endl;
    cout << "0 - Выход" << endl;

    cout << "Ваш выбор: ";
    cin >> key;

    return key;
}

// 1 - вывод списка дисков
void myLogicalDrives() {
    cout << endl << "Работа функции GetLogicalDrives:" << endl; // получает список доступных томов
    bitset<26> MyBs(GetLogicalDrives());
    cout << endl << "	" << MyBs << endl;
    cout << "	Доступные диски:" << endl << "	";

    for (int i = 0; i < MyBs.size(); i++) {
        if (MyBs[i] == 1) {
            cout << char(65 + i) << ':' << "\\" << "   ";
        }
    }
    cout << endl << endl;
}

void myDriveString() {
    wchar_t disks[256];
    wchar_t* disk;
    DWORD sizebuf = 256;

    cout << endl << "Работа функции GetLogicalDriveStrings:" << endl;
    GetLogicalDriveStrings(sizebuf, disks);
    disk = disks;
    cout << endl << "	Доступные диски:  " << endl << "	";
    while (*disk)
    {
        wprintf(L"%s   ", disk);
        disk = disk + wcslen(disk) + 1;
    }
    cout << endl << endl;
}

// 2 - вывод инфы о диске и размер свободного пространства
void myDriveType(string disk) {
    int driveType;

    cout << endl << "Работа функции GetDriveType:" << endl;
    driveType = GetDriveTypeA((LPCSTR)disk.c_str()); // получает инфу о диске
    switch (driveType) {
    case 0:
        cout << endl << "	Тип устройства определить не удалось" << endl;
        break;
    case 1:
        cout << endl << "	Диска не существует" << endl;
        break;
    case 2:
        cout << endl << "	Диск является сменным диском" << endl;
        break;
    case 3:
        cout << endl << "	Диск является жестким диском" << endl;
        break;
    case 4:
        cout << endl << "	Диск является сетевым диском" << endl;
        break;
    case 5:
        cout << endl << "	Диск является CD-ROM диском" << endl;
        break;
    case 6:
        cout << endl << "	Диск является RAM-диском" << endl;
        break;
    }
    cout << endl;
}

void myVolumeInformation(string disk) {
    char VolumeNameBuffer[100];
    char FileSystemNameBuffer[100];
    unsigned long VolumeSerialNumber;
    DWORD VolumeFlags;

    cout << endl << "Работа функции GetVolumeInformationA:" << endl;
    BOOL GetVolumeInformationFlag = GetVolumeInformationA(
        (LPCSTR)disk.c_str(),     // Корневой каталог диска, например "c:\" или "a:\"
        VolumeNameBuffer,         // Буфер, который получает имя указанного диска
        100,                      // Длина буфера в символах
        &VolumeSerialNumber,      // Указатель на переменную, которая получает серийный номер диска
        NULL,                     // Переменная, которая получает максимальную длину в символах компонента имени файла, поддержанного указанной файловой системой
        &VolumeFlags,             // Переменная, которая получает флажки, связанные с указанной файловой системой. 
        FileSystemNameBuffer,     // Указатель на буфер,который получает название файловой системы 
        100                       // Определяет длину в символах буфера для имени файловых систем
    );

    if (GetVolumeInformationFlag != 0)
    {
        cout << endl << "	Имя диска:  " << VolumeNameBuffer << endl;
        cout << "	Серийный номер диска:  " << VolumeSerialNumber << endl;
        cout << "	Файловая система:  " << FileSystemNameBuffer << endl;

        cout << endl << "	Флаги файловой системы: ";
        if (VolumeFlags & FS_CASE_IS_PRESERVED)
            cout << endl << "	При записи на диск сохраняется регистр букв в имени файла";
        if (VolumeFlags & FS_CASE_SENSITIVE)
            cout << endl << "	Файловая система поддерживает поиск файлов с учетом регистра букв в именах";
        if (VolumeFlags & FS_UNICODE_STORED_ON_DISK)
            cout << endl << "	Файловая система поддерживает хранение на диске имен файлов в UNICODE";
        if (VolumeFlags & FS_PERSISTENT_ACLS)
            cout << endl << "	Файловая система способна оперировать со списками контроля доступа(ACL).";
        if (VolumeFlags & FS_FILE_COMPRESSION)
            cout << endl << "	Файловая система поддерживает сжатие файлов";
        if (VolumeFlags & FS_VOL_IS_COMPRESSED)
            cout << endl << "	Том был подвергнут сжатию";
        if (VolumeFlags & FILE_NAMED_STREAMS)
            cout << endl << "	Файловая система поддерживает именованные потоки";
        if (VolumeFlags & FILE_SUPPORTS_ENCRYPTION)
            cout << endl << "	Файловая система поддерживает Encrypted File System (EFS)";
        if (VolumeFlags & FILE_SUPPORTS_OBJECT_IDS)
            cout << endl << "	Файловая система поддерживает идентификаторы объектов";
        if (VolumeFlags & FILE_SUPPORTS_REPARSE_POINTS)
            cout << endl << "	Файловая система поддерживает точки повторного разбора";
        if (VolumeFlags & FILE_SUPPORTS_SPARSE_FILES)
            cout << endl << "	Файловая система поддерживает разреженные файлы";
        if (VolumeFlags & FILE_VOLUME_QUOTAS)
            cout << endl << "	Файловая система поддерживает дисковые квоты";
    }
    else cout << endl << "	Данный диск отсутствует" << endl;
    cout << endl;
}

void myDiskFreeSpace(string disk) {
    unsigned __int64 FreeBytesAvailable;
    unsigned __int64 TotalNumberOfBytes;
    unsigned __int64 TotalNumberOfFreeBytes;

    cout << endl << "Работа функции GetDiskFreeSpace:" << endl;

    // возвращает информацию относительно количества места на диске
    BOOL GetDiskFreeSpaceFlag = GetDiskFreeSpaceExA(
        (LPCSTR)disk.c_str(),					  // directory name
        (PULARGE_INTEGER)&FreeBytesAvailable,     // bytes available to caller
        (PULARGE_INTEGER)&TotalNumberOfBytes,     // bytes on disk
        (PULARGE_INTEGER)&TotalNumberOfFreeBytes  // free bytes on disk
    );

    if (GetDiskFreeSpaceFlag != 0)
    {
        cout << endl << "	Общее число байтов = " << (long long)TotalNumberOfBytes
            << " (" << long double(long long(TotalNumberOfBytes)) / 1024 / 1024 / 1024
            << " Gb)" << endl;
        cout << "	Число свободных байтов = " << (long long)TotalNumberOfFreeBytes
            << " (" << long double(long long(TotalNumberOfFreeBytes)) / 1024 / 1024 / 1024
            << " Gb)" << endl;
    }
    else cout << endl << "	Данный диск отсутствует" << endl;
    cout << endl;
}

// 3 - создание и удаление заданных каталогов
void myCreateDirectory() {
    string disk, dir, name;

    cout << endl << "Работа функции CreateDirectory:" << endl;

    cout << endl << "	Введите имя диска (пример: с:\\), в котором будет создан каталог: ";
    cin >> disk;

    cout << "	Введите имя каталога, который вы хотите создать: ";
    cin >> dir;

    name = disk + dir;
    
    // второй параметр - атрибуты безопасности (только для Windows NT !NULL)
    if (CreateDirectoryA((LPCSTR)name.c_str(), NULL))
        cout << endl << "	Каталог " << name << " создан." << endl;
    else
        cout << endl << "	Ошибка. Каталог не был создан." << endl;
    cout << endl;
}

void myRemoveDirectory() {
    string name;

    cout << endl << "Работа функции RemoveDirectory:" << endl;
    cout << endl << "	Введите полный путь к файлу для удаления: ";
    cin >> name;

    if (RemoveDirectoryA((LPCSTR)name.c_str()))
        cout << endl << "	Каталог " << name << " был удален." << endl;
    else
        cout << endl << "	Ошибка. Каталог не был удален." << endl;
    cout << endl;
}

// 4 - создание файлов в новых каталогах
void myCreateFile() {
    HANDLE hfile;
    string my_path, my_name, file_path;

    cout << endl << "Работа функции CreateFile:" << endl;
    cout << endl << "	Введите полный путь к нужной директории: ";
    cin >> my_path;

    cout << "	Введите название нового файла: ";
    cin >> my_name;

    file_path = my_path + my_name;

    hfile = CreateFileA((LPCSTR)file_path.c_str(),
                        GENERIC_READ | GENERIC_WRITE, // режим доступа
                        0,                            // совместный доступ
                        NULL,                         // дескрипторы защиты
                        CREATE_NEW,                   // как действовать
                        0,                            // атрибуты файла
                        NULL);                        // дескрипторы шаблона файла (должен быть NULL)
    if (hfile)
        cout << endl << "	Файл " << file_path << " был создан." << endl;
    else
        cout << endl << "	Ошибка. Файл не был создан." << endl;

    CloseHandle(hfile);

    cout << endl;
}

// 5 - копирование и перемещение файлов между каталогами
void myCopyFile() {
    string old_name, new_name;

    cout << endl << "Работа функции CopyFile:" << endl;
    cout << endl << "	Введите полный путь к файлу, который нужно скопировать: ";
    cin >> old_name;

    cout << "	Введите новое имя и путь к файлу, который нужно создать: ";
    cin >> new_name;

    if (CopyFileA((LPCSTR)old_name.c_str(), (LPCSTR)new_name.c_str(), 0)) // если 1, то если новый файл уже существует, то ошибка; если 0, то файл записывается поверх существующего
        cout << endl << "	Файл " << old_name << " был скопирован." << endl;
    else {
        if (GetLastError() == 183)
            cout << endl << "	Элемент с таким именем уже существует." << endl;
        else
            cout << endl << "	Ошибка. Копирование не было выполнено." << endl;
    }
    cout << endl;
}

void myMoveFile() {
    string old_path, new_path, name;

    cout << endl << "Работа функции MoveFile:" << endl;
    cout << endl << "	Введите полный путь к файлу/каталогу, который нужно скопировать: ";
    cin >> old_path;

    cout << "	Введите новое имя и путь к файлу, куда нужно переместить файл/каталог: ";
    cin >> new_path;

    if (MoveFileA((LPCSTR)old_path.c_str(), (LPCSTR)new_path.c_str()))
        cout << endl << "	Элемент " << old_path << " был перемещен." << endl;
    else {
        if (GetLastError() == 183)
            cout << endl << "	Элемент с таким именем уже существует." << endl;
        else
            cout << endl << "	Ошибка. Перемещение не было выполнено." << endl;
    }
    cout << endl;
}

void myMoveFileEx() {
    string old_path, new_path;

    cout << endl << "Работа функции MoveFileEX:" << endl;
    cout << endl << "	Введите полный путь к файлу/каталогу, который нужно скопировать: ";
    cin >> old_path;

    cout << "	Введите новое имя и путь к файлу, куда нужно переместить файл/каталог: ";
    cin >> new_path;


    if (MoveFileExA((LPCSTR)old_path.c_str(),
        (LPCSTR)new_path.c_str(),
        MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) // параметры перемещения (1 - имитирует копирование при перемещении в др том, 2 - замещает существующий файл)
        cout << endl << "	Элемент " << old_path << " был перемещен." << endl;
    else
        cout << endl << "	Ошибка. Перемещение не было выполнено." << endl;
    cout << endl;
}

// 6 - анализ и изменение атрибутов файлов
void myGetFileAt() {
    string file_path;
    long FileAttributes;

    cout << endl << "Работа функции GetFileAttributes:" << endl;
    cout << endl << "	Введите полный путь к файлу/каталогу, чьи атрибуты нужно посмотреть: ";
    cin >> file_path;

    FileAttributes = GetFileAttributesA((LPCSTR)file_path.c_str());

    if (FileAttributes == -1)
        cout << endl << "	Файл " << file_path << " не найден.";
    else {
        if (FileAttributes & FILE_ATTRIBUTE_ARCHIVE)
            cout << endl << "	Файл " << file_path << " - архивный.";
        if (FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            cout << endl << "	Файл " << file_path << " - директория.";
        if (FileAttributes & FILE_ATTRIBUTE_READONLY)
            cout << endl << "	Файл " << file_path << " - только для чтения.";
        if (FileAttributes & FILE_ATTRIBUTE_SYSTEM)
            cout << endl << "	Файл " << file_path << " - системный.";
        if (FileAttributes & FILE_ATTRIBUTE_COMPRESSED)
            cout << endl << "	Файл " << file_path << " - сжатый.";
        if (FileAttributes & FILE_ATTRIBUTE_HIDDEN)
            cout << endl << "	Файл " << file_path << " - скрытый.";
        if (FileAttributes & FILE_ATTRIBUTE_NORMAL)
            cout << endl << "	Файл " << file_path << " не имеет таких атрибутов.";
        if (FileAttributes == 0)
            cout << endl << "	Ошибка.";
    }
    cout << endl << endl;
}

void mySetFileAt() {
    string file_path;
    int ans;
    long FileAttributes;

    cout << endl << "Работа функции SetFileAttributes:" << endl;
    cout << endl << "	Введите полный путь к файлу/каталогу, чьи атрибуты нужно установить: ";
    cin >> file_path;

    cout << endl << "	1 - Сбросить все атрибуты ";
    cout << endl << "	2 - Сделать скрытым файлом";
    cout << endl << "	3 - Сделать архивным файлом";
    cout << endl << "	4 - Сделать файлом 'только для чтения'";
    cout << endl << "	5 - Сделать системным файлом";
    cout << endl << "	Ваш ответ: ";
    cin >> ans;

    FileAttributes = GetFileAttributesA((LPCSTR)file_path.c_str());

    switch (ans) {
    case 1:
        FileAttributes = FILE_ATTRIBUTE_NORMAL;
        break;
    case 2:
        FileAttributes |= FILE_ATTRIBUTE_HIDDEN;
        break;
    case 3:
        FileAttributes |= FILE_ATTRIBUTE_ARCHIVE;
        break;
    case 4:
        FileAttributes |= FILE_ATTRIBUTE_READONLY;
        break;
    case 5:
        FileAttributes |= FILE_ATTRIBUTE_SYSTEM;
        break;
    default:
        cout << endl << "	Неверно введенный ключ." << endl;
        break;
    }

    if (ans > 0 && ans < 6) {
        if (SetFileAttributesA((LPCSTR)file_path.c_str(), FileAttributes))
            cout << endl << "	Атрибуты успешно установлены." << endl;
        else
            cout << endl << "   Ошибка при установке атрибутов." << endl;
    }
    cout << endl;
}

void myGetFileInfo() {
    long retval, FileAttributes;
    BY_HANDLE_FILE_INFORMATION file_info;
    string file_path;
    HANDLE hfile;
    SYSTEMTIME UTCtime, localTime;


    cout << endl << "Работа функции GetFileInformationByHandle:" << endl;
    cout << endl << "	Введите полный путь к файлу для работы: ";
    cin >> file_path;

    hfile = CreateFileA((LPCSTR)file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    retval = GetFileInformationByHandle(hfile, &file_info);
    if (!retval)
        cout << endl << "	Ошибка." << endl;
    else {
        FileAttributes = file_info.dwFileAttributes;
        if (FileAttributes == -1)
            cout << endl << "	Файл " << file_path << " не найден.";
        else {
            if (FileAttributes & FILE_ATTRIBUTE_ARCHIVE)
                cout << endl << "	Файл " << file_path << " - архивный.";
            if (FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                cout << endl << "	Файл " << file_path << " - директория.";
            if (FileAttributes & FILE_ATTRIBUTE_READONLY)
                cout << endl << "	Файл " << file_path << " - только для чтения.";
            if (FileAttributes & FILE_ATTRIBUTE_SYSTEM)
                cout << endl << "	Файл " << file_path << " - системный.";
            if (FileAttributes & FILE_ATTRIBUTE_COMPRESSED)
                cout << endl << "	Файл " << file_path << " - сжатый.";
            if (FileAttributes & FILE_ATTRIBUTE_HIDDEN)
                cout << endl << "	Файл " << file_path << " - скрытый.";
            if (FileAttributes & FILE_ATTRIBUTE_NORMAL)
                cout << endl << "	Файл " << file_path << " не имеет таких атрибутов.";
            if (FileAttributes == 0)
                cout << endl << "	Ошибка.";
        }

        FileTimeToSystemTime(&file_info.ftCreationTime, &UTCtime);
        SystemTimeToTzSpecificLocalTime(NULL, &UTCtime, &localTime);
        cout << endl << "	Время создания файла: " << localTime.wDay << "." << localTime.wMonth << "." << localTime.wYear;
        printf(" %02d:%02d\n", localTime.wHour, localTime.wMinute);

        FileTimeToSystemTime(&file_info.ftLastAccessTime, &UTCtime);
        SystemTimeToTzSpecificLocalTime(NULL, &UTCtime, &localTime);
        cout << "	Время последнего доступа к файлу: " << localTime.wDay << "." << localTime.wMonth << "." << localTime.wYear;
        printf(" %02d:%02d\n", localTime.wHour, localTime.wMinute);

        FileTimeToSystemTime(&file_info.ftLastWriteTime, &UTCtime);
        SystemTimeToTzSpecificLocalTime(NULL, &UTCtime, &localTime);
        cout << "	Время последней записи в файл: " << localTime.wDay << "." << localTime.wMonth << "." << localTime.wYear;
        printf(" %02d:%02d\n", localTime.wHour, localTime.wMinute);

        cout << "	Порядковый номер тома, который содержит файл: " << file_info.dwVolumeSerialNumber << endl;

        cout << "	Старшая часть размера файла: " << file_info.nFileSizeHigh << endl;

        cout << "	Младшая часть размера файла: " << file_info.nFileSizeLow << endl;

        cout << "	Число связей с этим файлом: " << file_info.nNumberOfLinks << endl;

        cout << "	Старшая часть уникального идентификатора связи с файлом: " << file_info.nFileIndexHigh << endl;

        cout << "	Младшая часть уникального идентификатора связи с файлом: " << file_info.nFileIndexLow << endl;
    }
    retval = CloseHandle(hfile);
    cout << endl;
}

void myGetFileTime() {
    string file_path;
    HANDLE hfile;
    FILETIME creat_time, acc_time, wr_time;
    SYSTEMTIME UTCtime, localTime;


    cout << endl << "Работа функции GetFileTime:" << endl;
    cout << endl << "	Введите полный путь к файлу для работы: ";
    cin >> file_path;

    hfile = CreateFileA((LPCSTR)file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING | CREATE_NEW, 0, NULL);
    if (!hfile)
        cout << endl << "	Ошибка." << endl;
    else {
        //  извлекает данные о дате и времени, когда файл был создан, последнего доступа и последнего изменения.
        GetFileTime(hfile, &creat_time, &acc_time, &wr_time);

        FileTimeToSystemTime(&creat_time, &UTCtime); // FILETIME to SYSTEMFILE
        SystemTimeToTzSpecificLocalTime(NULL, &UTCtime, &localTime); // UTC to local time
        printf("	Время создания файла: %02d.%02d.%02d %02d:%02d\n", localTime.wDay, localTime.wMonth, localTime.wYear,
            localTime.wHour, localTime.wMinute);

        FileTimeToSystemTime(&acc_time, &UTCtime);
        SystemTimeToTzSpecificLocalTime(NULL, &UTCtime, &localTime);
        printf("	Время последнего доступа к файлу: %02d.%02d.%02d %02d:%02d\n", localTime.wDay, localTime.wMonth, localTime.wYear,
            localTime.wHour, localTime.wMinute);

        FileTimeToSystemTime(&wr_time, &UTCtime);
        SystemTimeToTzSpecificLocalTime(NULL, &UTCtime, &localTime);
        printf("	Время последней записи в файл: %02d.%02d.%02d %02d:%02d\n", localTime.wDay, localTime.wMonth, localTime.wYear,
            localTime.wHour, localTime.wMinute);
    }
    CloseHandle(hfile);
    cout << endl;
}

void mySetFileTime() {
    string file_path;
    HANDLE hfile;
    FILETIME new_time, creat_time, acc_time, wr_time;
    SYSTEMTIME UTCtime, localTime;


    cout << endl << "Работа функции SetFileTime:" << endl;
    cout << endl << "	Введите полный путь к файлу для работы: ";
    cin >> file_path;

    hfile = CreateFileA((LPCSTR)file_path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING | FILE_WRITE_ATTRIBUTES, 0, NULL);
    if (!hfile)
        cout << endl << "	Ошибка." << endl;
    else {
        GetFileTime(hfile, &creat_time, &acc_time, &wr_time);
        GetSystemTimeAsFileTime(&acc_time);

        SetFileTime(hfile, &creat_time, &acc_time, &wr_time);
        FileTimeToSystemTime(&acc_time, &UTCtime);
        SystemTimeToTzSpecificLocalTime(NULL, &UTCtime, &localTime);

        printf("	Установим в качестве времени последнего доступа текущее время: %02d.%02d.%02d %02d:%02d\n", localTime.wDay,
            localTime.wMonth, localTime.wYear, localTime.wHour, localTime.wMinute);
    }
    CloseHandle(hfile);
    cout << endl;
}