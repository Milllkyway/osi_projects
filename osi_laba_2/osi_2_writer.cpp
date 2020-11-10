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

	hFile = CreateFileA((LPCSTR)file_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile) {
		cout << "Файл успешно открылся. " << endl;
		cout << endl << "Введите дескриптор потока: ";
		cin >> threadDesc;

		hMap = CreateFileMapping(hFile, 0, PAGE_READWRITE, 0, 4096, (LPCWSTR)threadDesc.c_str());
		if (hMap) {
			cout << endl << "Введите данные, которые хотите записать в файл: " << endl;
			cin.ignore();
			getline(cin, dataToWrite);
			
			//cout << "My data: " << dataToWrite;

			adOfMapView = MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, NULL);
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