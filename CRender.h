#pragma once
#include <d3d9.h>
#include <cstdio>
#include <cstdarg>
#include <stdarg.h>
#include <vector>
#include <d3dx9.h>

#pragma comment(lib, "d3dx9")
#pragma comment(lib, "d3d9")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#define CColor DWORD

#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

#define PI 3.14159265358f

class CVertexList
{
public:
	FLOAT X, Y, Z;
	DWORD dColor;
};

class CRender
{
public:
	CRender(HWND hWnd, bool fullScreen, int w, int h);

	VOID CreateFonts(float Multiplier, LPCWSTR FontName);
	BOOL isCreated();
	VOID Created();

	VOID Release();

	VOID Begin();
	VOID End();
	VOID Present();
	VOID Clear();

	// Drawring functions
	VOID DrawFontText(INT x, INT y, D3DCOLOR col, PCHAR szString, ...);
	VOID DrawFontText(INT x, INT y, INT size, D3DCOLOR col, PCHAR szString, ...);
	VOID DrawOutlinedFontText(INT X, INT Y, CColor dColor, int type, PCHAR szString, ...);

	VOID DrawRect(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor);

	VOID DrawSprite(LPDIRECT3DTEXTURE9 tex, FLOAT x, FLOAT y, FLOAT resolution, FLOAT scale, FLOAT rotation);
	VOID DrawTexture(INT x, INT y, LPDIRECT3DTEXTURE9 dTexture, LPD3DXSPRITE sprite);
	VOID DrawImage(INT x, INT y, LPDIRECT3DTEXTURE9 dTexture, LPD3DXSPRITE sprite, std::vector<byte> MyImage);

	VOID DrawLine(FLOAT startx, FLOAT starty, FLOAT endx, FLOAT endy, FLOAT width, CColor dCcolor);

	VOID DrawFilledCircle(FLOAT x, FLOAT y, FLOAT r, FLOAT smoothness, CColor dColor);
	VOID DrawCircle(FLOAT x, FLOAT y, FLOAT r, FLOAT smoothness, CColor dColor);
	VOID DrawOutlinedRect(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, CColor dColor);

	VOID Draw2dBox(int X, int Y, int W, int H, int linewidth, DWORD inlinecolor);

	VOID DrawCornerBox(int X, int Y, int W, int H, int linewidth, DWORD inlinecolor);

	LPDIRECT3DDEVICE9 GetDevice();
private:
	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 pDevice;

	LPD3DXFONT pFont;
	LPD3DXFONT pFontSmaller;
	LPD3DXFONT pFontSmallerSmaller;
	LPD3DXFONT pFontScale = NULL;

	LPD3DXSPRITE pSprite;
	LPD3DXLINE line;

	BOOL bisCreated = false;

	DWORD dwOldFVF;
};
