#include <windows.h>
#include <iostream>
#include <locale.h>

using namespace std;

int main()
{
	HANDLE hMap;
	string file_path, threadDesc, dataToWrite;
	LPVOID adOfMapView;

	setlocale(LC_ALL, "Russian");

	cout << endl << "Работа программы-читателя..." << endl;


	cout << endl << "Введите дескриптор потока (такой же, как и в программе-писателе): ";
	cin >> threadDesc;

	hMap = OpenFileMapping(FILE_MAP_READ, false, (LPCWSTR)threadDesc.c_str());
	if (hMap) {
		adOfMapView = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
		if (adOfMapView) {
			cout << endl << "Данные из файла: \n"<< (char*)adOfMapView;
			cout << endl << endl;

			system("pause");
			UnmapViewOfFile(adOfMapView);
		}
		else
			cout << endl << "Ошибка в OpenFileMapping. Код ошибки: " << GetLastError();
	}
	else
		cout << endl << "Ошибка в CreateFileMapping. Код ошибки: " << GetLastError();

	CloseHandle(hMap);

	return 0;
}