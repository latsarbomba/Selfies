#pragma once

class Globals {
public: 

	DWORD_PTR pUWorld = 0;
	DWORD_PTR pULevel = 0;

	DWORD_PTR pLocalPlayer; // LocalPlayer Pointer
	HWND hWnd = 0; // Overlay Window handle
	HANDLE hProcess; // Process handle
	DWORD_PTR baseaddress; // Base address Tslgame.exe

	//DWORD_PTR UWorld;
};

Globals* global;