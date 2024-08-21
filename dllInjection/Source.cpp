#include <Windows.h>
#include <stdio.h>
#include <iostream>

int main() {

	// Get full path of DLL to inject
	const char* path = "C:\\Users\\User\\Downloads\\windows final project\\mydll\\x64\\Debug\\mydll.dll"; //PLEASE INSERT YOUR OWN FULL PATH HERE!!!!
	DWORD pathLen = strlen(path)+1; //for later use

	// Get LoadLibrary function address –
	// the address doesn't change at remote process
	PVOID addrLoadLibrary = (PVOID)GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), "LoadLibraryA"); //get the load library function from windows kernel

	char* in = new char[10]; //input for process ID to run with openProcess
	std::cin >> in;
	int pid = std::atoi(in);
	delete[] in;

	// Open remote process 
	// Get a pointer to memory location in remote process,
	// big enough to store DLL path
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);

	PVOID memAddr = (PVOID)VirtualAllocEx(proc, NULL, pathLen, MEM_COMMIT, PAGE_READWRITE); //allocate memory for the dll
	if (NULL == memAddr) {
		return 0;
	}
	// Write DLL name to remote process memory
	bool check = WriteProcessMemory(proc, memAddr, path, pathLen, NULL);
	if (0 == check) {
		return 0;
	}

	// open the thread that runs the dll
	HANDLE hRemote = CreateRemoteThread(proc, NULL, NULL, (LPTHREAD_START_ROUTINE)addrLoadLibrary, (LPVOID)memAddr, NULL, NULL);
	if (NULL == hRemote) {
		return 0;
	}
	//wait until close
	WaitForSingleObject(hRemote, INFINITE);
	CloseHandle(hRemote);
	return 0;
}
