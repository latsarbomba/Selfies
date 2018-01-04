#pragma once

class Memory
{
public:
	Memory(HANDLE hProc);
	~Memory();

	DWORD_PTR GetModule(std::wstring modulename);
	DWORD_PTR ResolveRelativePtr(DWORD_PTR Address, DWORD_PTR ofs);
	BOOL DataCompare(BYTE* pData, BYTE* bMask, char * szMask);
	DWORD64 FindPatternEx(HANDLE hProcess, std::wstring modulename, BYTE *bMask, char *szMask, DWORD64 dwAddress);
	
	template<typename TYPE>
	TYPE Read(DWORD_PTR address) {
		TYPE buffer;
		if (ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(buffer), 0) != 0)
			return buffer;
	}

	template<typename TYPE>
	TYPE Read(DWORD_PTR address, SIZE_T length) {
		TYPE buffer;
		if (ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, length, 0) != 0)
			return buffer;
	}

	bool ReadData(DWORD address, SIZE_T length, char buffer[])
	{
		if (ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, length, 0) == 0)
			return false;
		else
			return true;
	}

	bool ReadData(DWORD address, SIZE_T length, wchar_t* buffer[])
	{
		if (ReadProcessMemory(hProcess, (LPVOID)address, &buffer, length, 0) == 0)
			return false;
		else
			return true;
	}

	template<typename TYPE>
	void Write(DWORD address, TYPE dataToWrite) {
		TYPE buffer = dataToWrite;
		if (!WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), 0))
			std::cout << "WPM No Work: " << GetLastError() << std::endl;
	}

private:
	HANDLE hProcess;
	MODULEINFO modinfo;
};

DWORD_PTR Memory::ResolveRelativePtr(DWORD_PTR Address, DWORD_PTR ofs)
{
	if (Address)
	{
		Address += ofs;
		DWORD tRead;
		ReadProcessMemory(hProcess, (void*)(Address + 3), &tRead, sizeof(tRead), NULL); // .text:000000014000AE54                 mov     rcx, cs:142384108h
		if (tRead) return (DWORD_PTR)(Address + tRead + sizeof(DWORD) + 3);
	}
	return NULL;
}

BOOL Memory::DataCompare(BYTE* pData, BYTE* bMask, char * szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return FALSE;

	return (*szMask == NULL);
}

DWORD64 Memory::FindPatternEx(HANDLE hProcess, std::wstring modulename, BYTE *bMask, char *szMask, DWORD64 dwAddress)
{
	GetModuleInformation(hProcess, GetModuleHandle(modulename.c_str()), &modinfo, sizeof(MODULEINFO)); // Get module information such as size of PE

	DWORD64 dwReturn = 0;
	DWORD64 dwDataLength = strlen(szMask);
	BYTE *pData = new BYTE[dwDataLength + 1];
	SIZE_T dwRead;

	for (DWORD64 i = 0; i < modinfo.SizeOfImage; i++)
	{
		DWORD64 dwCurAddr = dwAddress + i;
		bool bSuccess;
		bSuccess = ReadProcessMemory(hProcess, (LPCVOID)dwCurAddr, pData, dwDataLength, &dwRead);

		if (!bSuccess || dwRead == 0)
		{
			continue;
		}

		if (DataCompare(pData, bMask, szMask))
		{
			dwReturn = dwAddress + i;
			break;
		}
	}

	delete[] pData;
	return dwReturn;
}

DWORD_PTR Memory::GetModule(std::wstring modulename)
{
	HMODULE *hModules = NULL;
	wchar_t szBuf[50];
	DWORD cModules = 0;

	EnumProcessModules(hProcess, hModules, 0, &cModules);
	hModules = new HMODULE[cModules / sizeof(HMODULE)];

	if (EnumProcessModules(hProcess, hModules, cModules / sizeof(HMODULE), &cModules)) {
		for (int i = 0; i < cModules / sizeof(HMODULE); i++) {
			if (GetModuleBaseName(hProcess, hModules[i], szBuf, sizeof(szBuf))) {
				if (modulename.compare(szBuf) == 0)
				{
					return (DWORD_PTR)hModules[i];
				}
			}
		}
	}

	return 0;
}

Memory::Memory(HANDLE hProc)
{
	hProcess = hProc;
}

//TODO: Clean up later.
Memory::~Memory()
{
}

Memory* mem;