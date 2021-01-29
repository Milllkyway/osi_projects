#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <locale.h>

using namespace std;

int main()
{
	HANDLE hFile, hMap;
	string file_path, threadDesc, dataToWrite;
	LPVOID adOfMapView;

	setlocale(LC_ALL, "Russian");

	cout << endl << "Работа программы-писателя..." << endl;

	cout << endl << "Введите полный путь к файлу для работы: ";
	cin >> file_path;

	hFile = CreateFileA((LPCSTR)file_path.c_str(),
						 GENERIC_READ | GENERIC_WRITE, 
						 0,								// совместный доступ
						 NULL,							// дескриптор защиты (NULL - десриптор по умолч.)
						 CREATE_ALWAYS,					
						 0,								// атрибуты и флаги файла
						 NULL);							// должен быть NULL, иначе ошибка
	if (hFile) {
		cout << "Файл успешно открылся. " << endl;
		cout << endl << "Введите дескриптор потока: ";
		cin >> threadDesc;

		hMap = CreateFileMapping(hFile,							// дескриптор файла
								 0,								// указатель на структуру SECURITY_ATTRIBUTES (0 - не может быть унаследован)
								 PAGE_READWRITE,				// атрибуты защиты
								 0,								// старшее слово максимального размера проецируемого файла
								 4096,							// младшее слово (если тут 0 и в предыдущем, то размер = размеру hFile)
								 (LPCWSTR)threadDesc.c_str());	// имя проецируемого файла
		if (hMap) {
			cout << endl << "Введите данные, которые хотите записать в файл: " << endl;
			cin.ignore();
			getline(cin, dataToWrite);
			
			//cout << "My data: " << dataToWrite;

			adOfMapView = MapViewOfFile(hMap,				// дескриптор проецируемого файла
										FILE_MAP_WRITE,		// режим доступа
										0,					// старшее DWORD смещения
										0,					// младшее
										NULL);				// число отображаемых байтов (0 значит весь файл)
			if (adOfMapView) {
				CopyMemory(adOfMapView, (char*)dataToWrite.c_str(), dataToWrite.length() * sizeof(char));
				cout << endl;
				
				system("pause");
				UnmapViewOfFile(adOfMapView);
			}
			else
				cout << endl << "Ошибка в MapViewOfFile. Код ошибки: " << GetLastError();
		}
		else
			cout << endl << "Ошибка в CreateFileMapping. Код ошибки: " << GetLastError();
		CloseHandle(hMap);
	}
	else
		cout << "Ошибка в CreateFileA. Код ошибки: " << GetLastError();

	CloseHandle(hFile);

	return 0;
}