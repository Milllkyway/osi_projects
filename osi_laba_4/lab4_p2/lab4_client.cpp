#include <iostream>
#include <windows.h>
#include <clocale>

using namespace std;

void WINAPI ReadCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
	cout << "Get message - DONE" << endl;
}

int main() {
	CHAR msg[256];
	LPCTSTR name = TEXT("\\\\.\\pipe\\pipeName");

	HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE newPipe = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	OVERLAPPED overlapped = OVERLAPPED();
	
	int ans = 10;

	if (event != INVALID_HANDLE_VALUE && newPipe != INVALID_HANDLE_VALUE) {
		while(ans != 0) {
			cout << "1 - Get message" << endl;
			cout << "2 - Disconnect pipe" << endl;
			cout << "0 - Exit" << endl;
			cout << endl << "Your answer: ";
			cin >> ans;

			switch (ans) {
			case 1:
				overlapped.hEvent = event;
			
				if (!ReadFileEx(newPipe, (LPVOID)msg, 256, &overlapped, ReadCallback))
					cout << "Error in reading. Error code: " << GetLastError() << endl;
				else
					cout << msg << endl;
				break;

			case 2:
				if (!CloseHandle(newPipe)) cout << "Error while disconnecting. Error code: " << GetLastError() << endl;
				else cout << "You was disconnected from pipe" << endl;
				
				break;
			case 0:
				cout << "Goodbye!" << endl;
				break;
			default:
				cout << "Wrong key!" << endl;
				break;
			}
			system("pause");
			system("cls");
		}
	}
	else {
		cout << "Error in creating pipe. Error code: " << GetLastError() << endl;
		system("pause");
	}

	
	return 0;
}