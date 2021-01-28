#include <windows.h>
#include <iostream>

#pragma comment(lib, "winmm.lib")
using namespace std;

void WINAPI ReadCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
	cout << "get message - done." << endl;
}


int main() {
	CHAR mes[256];
	bool flag = false;
	LPCTSTR name = TEXT("\\\\.\\pipe\\pipeName");
	OVERLAPPED overlapped = OVERLAPPED(), olPipe = OVERLAPPED();
	HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE newPipe = CreateNamedPipe(name, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 256, 256, 0, NULL);

	int ans = 10;


	if (event != INVALID_HANDLE_VALUE && newPipe != INVALID_HANDLE_VALUE) {
		while (ans != 0) {
			cout << "1 - Connect to pipe" << endl;
			cout << "2 - Send message" << endl;
			cout << "3 - Disconnect from pipe" << endl;
			cout << "0 - Exit" << endl;
			cout << endl <<"Your answer: ";
			cin >> ans;

			switch (ans) {
			case 1:
				olPipe.hEvent = event;

				flag = ConnectNamedPipe(newPipe, &olPipe);
				WaitForSingleObject(event, INFINITE);

				if (flag) cout << "You are connected." << endl;
				else cout << "Error in connecting to the pipe." << endl;
				break;
			case 2:
				if (flag) {
					cout << "Enter the message:";
					cin >> mes;

					overlapped.hEvent = event;
					flag = WriteFile(newPipe, (LPCVOID)mes, 256, NULL, &overlapped);

					if (WaitForSingleObject(event, 20000) == WAIT_OBJECT_0 && flag)
						cout << "Writing - DONE" << endl;
					else
						cout << "Error in writing. Error code: " << GetLastError() << endl;
				}
				else cout << "Lost connection." << endl;
				cout << endl;
				break;
			case 3:
				if (!DisconnectNamedPipe(newPipe)) cout << "Error while disconnecting. Error code: " << endl;
				else cout << "You was disconnected." << endl;
				flag = FALSE;
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
	else
		cout << "Error in creating pipe. Error code: " << GetLastError() << endl;
	if (newPipe != INVALID_HANDLE_VALUE)
		CloseHandle(newPipe);
	if (event != INVALID_HANDLE_VALUE)
		CloseHandle(event);
	return 0;
}