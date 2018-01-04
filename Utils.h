#pragma once

#include <random>
#include <atlstr.h>
#include "menu/imgui.h"

class Utils {
public:

	static bool File_Exsists(const std::string& name) {
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}

	template<typename ... Args>
	static std::string string_format(const std::string& format, Args ... args)
	{
		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}

	static int getKeycode(const char* in)
	{
		std::string temp = in;
		std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

		if (temp == "lshift" || temp == "shift")
		{
			return VK_LSHIFT;
		}
		else if (temp == "lcontrol" || temp == "lctr")
		{
			return VK_LCONTROL;
		}
		else if (temp == "lmenu" || temp == "lalt" || temp == "alt")
		{
			return VK_LMENU;
		}
		else if (temp == "lshift")
		{
			return VK_LSHIFT;
		}
		else if (temp == "lmouse" || temp == "mouse1")
		{
			return VK_LBUTTON;
		}
		else if (temp == "rmouse" || temp == "mouse2")
		{
			return VK_RBUTTON;
		}
		else if (temp == "sidebtn" || temp == "mousex1")
		{
			return VK_XBUTTON1;
		}
		else if (temp == "sidebtn2" || temp == "mousex2")
		{
			return VK_XBUTTON2;
		}
		else if (temp == "mmouse" || temp == "mouse3")
		{
			return VK_MBUTTON;
		}
		else if (temp == "cap" || temp == "caps")
		{
			return VK_CAPITAL;
		}
		else if (temp == "tab")
		{
			return VK_TAB;
		}

		if (in[0] >= '0' && in[0] <= '9')
			return (int)in;
		if (in[0] >= 'a' && in[0] <= 'z')
			return toupper(in[0]);
		if (in[0] >= 'A' && in[0] <= 'Z')
			return (int)in;
	}

	static void GetDesktopResolution(int& width, int& height)
	{
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		width = desktop.right;
		height = desktop.bottom;
	}

	static std::string ws2s(const std::wstring& wstr)
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}

	static std::wstring StringToWString(const std::string &s)
	{
		std::wstring wsTmp(s.begin(), s.end());

		return wsTmp;
	}

	static std::string random_string(size_t length)
	{
		auto randchar = []() -> char
		{
			const char charset[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
			const size_t max_index = (sizeof(charset) - 1);
			return charset[rand() % max_index];
		};
		std::string str(length, 0);
		std::generate_n(str.begin(), length, randchar);
		return str;
	}

	static int rangeRandomAlg2(int min, int max) {
		int n = max - min + 1;
		int remainder = RAND_MAX % n;
		int x;
		do {
			x = rand();
		} while (x >= RAND_MAX - remainder);
		return min + x % n;
	}

	static bool toBool(std::string str)
	{
		if (str == "0" || str == "false")
			return false;
		else
			return true;
	}

	static ImVec4 toColor(std::string str)
	{
		std::vector<std::string> colors;
		split(str, ",", colors);

		float r = 0, g = 0, b = 0, a = 1.0f;
		r = std::stof(colors[0]);
		g = std::stof(colors[1]);
		b = std::stof(colors[2]);
		
		return ImVec4(r, g, b, a);
	}
	
	static std::string saveColor(ImVec4 SkelColorIMGUI)
	{
		const std::string devider = ",";
		//R,G,B,A
		return std::to_string(SkelColorIMGUI.x) + devider + std::to_string(SkelColorIMGUI.y) + devider + std::to_string(SkelColorIMGUI.z) + devider + std::to_string(255);
	}

	template<typename TYPE>
	static void ReverseVector(std::vector<TYPE> Vecin)
	{
		std::reverse(Vecin.begin(), Vecin.end());
	}

	static DWORD GetPhysicalDriveSerialNumber(UINT nDriveNumber, CString& strSerialNumber)
	{
		DWORD dwResult = NO_ERROR;
		strSerialNumber.Empty();

		CString strDrivePath;
		strDrivePath.Format(_T("\\\\.\\PhysicalDrive%u"), nDriveNumber);

		HANDLE hDevice = ::CreateFile(strDrivePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);

		if (INVALID_HANDLE_VALUE == hDevice)
			return ::GetLastError();

		STORAGE_PROPERTY_QUERY storagePropertyQuery;
		ZeroMemory(&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
		storagePropertyQuery.PropertyId = StorageDeviceProperty;
		storagePropertyQuery.QueryType = PropertyStandardQuery;

		STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = { 0 };
		DWORD dwBytesReturned = 0;
		if (!::DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
			&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
			&storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER),
			&dwBytesReturned, NULL))
		{
			dwResult = ::GetLastError();
			::CloseHandle(hDevice);
			return dwResult;
		}

		const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
		BYTE* pOutBuffer = new BYTE[dwOutBufferSize];
		ZeroMemory(pOutBuffer, dwOutBufferSize);

		if (!::DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
			&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
			pOutBuffer, dwOutBufferSize,
			&dwBytesReturned, NULL))
		{
			dwResult = ::GetLastError();
			delete[]pOutBuffer;
			::CloseHandle(hDevice);
			return dwResult;
		}

		STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pOutBuffer;
		const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
		if (dwSerialNumberOffset != 0)
		{
			strSerialNumber = CString(pOutBuffer + dwSerialNumberOffset);
		}

		delete[]pOutBuffer;
		::CloseHandle(hDevice);
		return dwResult;
	}

	static CString GetHdSerial()
	{
		UINT nDriveNumber = 0;
		CString strSerialNumber;
		DWORD dwResult = GetPhysicalDriveSerialNumber(nDriveNumber, strSerialNumber);
		CString strReport;

		if (NO_ERROR == dwResult)
		{
			//Strip white space
			return strSerialNumber;
		}
		else
		{
			return "ERROR";
		}
	}
	static char* getMAC()
	{
		PIP_ADAPTER_INFO AdapterInfo;
		DWORD dwBufLen = sizeof(AdapterInfo);
		char *mac_addr = (char*)malloc(17);

		AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
		if (AdapterInfo == NULL) {
			return "Error";

		}

		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {

			AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
			if (AdapterInfo == NULL) {
				return "Error";
			}
		}

		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
			do {
				sprintf(mac_addr, /*%02X:%02X:%02X:%02X:%02X:%02X*/XorStr<0x0A, 30, 0x540976EF>("\x2F\x3B\x3E\x55\x34\x2A\x20\x23\x4A\x29\x31\x25\x24\x4F\x22\x3C\x2A\x29\x44\x27\x3B\x2F\x12\x79\x18\x06\x14\x17\x7E" + 0x540976EF).s,
					pAdapterInfo->Address[0], pAdapterInfo->Address[1],
					pAdapterInfo->Address[2], pAdapterInfo->Address[3],
					pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
				return mac_addr;

				pAdapterInfo = pAdapterInfo->Next;
			} while (pAdapterInfo);
		}
		else {
			return "ERROR";
		}

		free(AdapterInfo);
	}

	static BYTE* DoPostRequest(std::wstring ip, std::wstring path, std::string data) //Utilss::DoPostRequestHTTPS
	{
		HINTERNET hHttpSession, hHttpRequest, hIntSession;
		DWORD timeout;
		DWORD dwRead;

		hIntSession = InternetOpen(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64)", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

		if (!hIntSession)
			return 0;

		timeout = 300 * 1000;
		if (!InternetSetOption(hIntSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout)) ||
			!InternetSetOption(hIntSession, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout)) ||
			!InternetSetOption(hIntSession, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout)))
			return 0;

		hHttpSession = InternetConnect(hIntSession, ip.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
		if (!hHttpSession)
			return 0;

		hHttpRequest = HttpOpenRequest(
			hHttpSession,
			L"POST",
			path.c_str(),
			0, 0, NULL,
			INTERNET_FLAG_SECURE, 0);

		if (!hHttpRequest)
			return 0;

		std::string szHeaders = "Content-Type: application/x-www-form-urlencoded;\r\nContent-Length: "; // "Content-Type: application/x-www-form-urlencoded;\r\nContent-Length: ";
		szHeaders += std::to_string(data.length());

		if (!HttpSendRequestA(hHttpRequest, szHeaders.c_str(), szHeaders.length(), (LPVOID)data.c_str(), data.length()))
			return 0;

		//payload
		const int BUFFERSIZE = 7 * 1000 * 1000; //7mb
		dwRead = 0;
		CHAR* szAsciiBuffer = new CHAR[BUFFERSIZE];
		memset(szAsciiBuffer, 0, BUFFERSIZE);
		InternetReadFile(hHttpRequest, szAsciiBuffer, BUFFERSIZE, &dwRead);

		InternetCloseHandle(hHttpRequest);
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hIntSession);

		return (BYTE*)szAsciiBuffer;
	}

	static void split(std::string& s, std::string delim, std::vector<std::string>& v)
	{
		auto i = 0;
		auto pos = s.find(delim);
		while (pos != std::string::npos)
		{
			v.push_back(s.substr(i, pos - i));
			pos += delim.length();
			i = pos;
			pos = s.find(delim, pos);
			if (pos == std::string::npos)
				v.push_back(s.substr(i, s.length()));
		}
	}

	static std::wstring GetPcName()
	{
		const DWORD bufSize = MAX_COMPUTERNAME_LENGTH + 1;
		wchar_t pcName[bufSize];
		GetComputerName(pcName, (PDWORD)&bufSize);

		return std::wstring(pcName);
	}

	static void ErasePEHeaderFromMemory()
	{
		DWORD OldProtect = 0;

		char *pBaseAddr = (char*)GetModuleHandle(NULL);

		VirtualProtect(pBaseAddr, 4096, // Assume x86 page size
			PAGE_READWRITE, &OldProtect);

		ZeroMemory(pBaseAddr, 4096);
	}

	void static Selfdestruct()
	{
		// temporary .bat file
		static char templ[] =
			":Repeat\r\n"
			"del \"%s\"\r\n"
			"if exist \"%s\" goto Repeat\r\n"
			"rmdir \"%s\"\r\n"
			"del \"%s\"";

		char modulename[_MAX_PATH];    // absolute path of calling .exe file
		char folder[_MAX_PATH];

		char sztempPath[_MAX_PATH];
		GetTempPathA(_MAX_PATH, sztempPath);

		std::string BatName = Utils::random_string(15);

		BatName += ".bat";

		strcat(sztempPath, BatName.c_str());

		GetModuleFileNameA(NULL, modulename, MAX_PATH);
		strcpy(folder, modulename);
		char *pb = strrchr(folder, '\\');
		if (pb != NULL)
			*pb = 0;

		HANDLE hf;

		hf = CreateFileA(sztempPath, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hf != INVALID_HANDLE_VALUE)
		{
			DWORD len;
			char *bat;

			bat = (char*)alloca(strlen(templ) +
				strlen(modulename) * 2 + strlen(sztempPath) + 20);

			wsprintfA(bat, templ, modulename, modulename, folder, sztempPath);

			WriteFile(hf, bat, strlen(bat), &len, NULL);
			CloseHandle(hf);

			ShellExecuteA(NULL, "open", sztempPath, NULL, NULL, SW_HIDE);
		}
	}

	

};
