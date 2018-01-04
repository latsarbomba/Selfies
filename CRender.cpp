#include "CRender.h" 
#include <time.h>
#include <string> 
#include "menu/imgui.h"

CRender::CRender(HWND hWnd, bool Windowed, int w, int h)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (d3d == nullptr)
		exit(1);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = Windowed;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; //D3DFMT_X8R8G8B8; // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = w;    // set the width of the buffer
	d3dpp.BackBufferHeight = h;    // set the height of the buffer

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	//D3DPRESENT_INTERVAL_ONE With VSync
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//New
	//d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;

	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, //D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&pDevice);

	if (pDevice == nullptr)
		exit(1);

	D3DXCreateSprite(pDevice, &pSprite);
	D3DXCreateLine(pDevice, &line);
}

VOID CRender::DrawCornerBox(int X, int Y, int W, int H, int linewidth, DWORD inlinecolor) {
	float lineW = (W / 5);
	float lineH = (H / 6);
	float lineT = 1;

	//outline
	DrawLine(X - lineT, Y - lineT, X + lineW, Y - lineT, linewidth, inlinecolor); //top left
	DrawLine(X - lineT, Y - lineT, X - lineT, Y + lineH, linewidth, inlinecolor);
	DrawLine(X - lineT, Y + H - lineH, X - lineT, Y + H + lineT, linewidth, inlinecolor); //bot left
	DrawLine(X - lineT, Y + H + lineT, X + lineW, Y + H + lineT, linewidth, inlinecolor);
	DrawLine(X + W - lineW, Y - lineT, X + W + lineT, Y - lineT, linewidth, inlinecolor); // top right
	DrawLine(X + W + lineT, Y - lineT, X + W + lineT, Y + lineH, linewidth, inlinecolor);
	DrawLine(X + W + lineT, Y + H - lineH, X + W + lineT, Y + H + lineT, linewidth, inlinecolor); // bot right
	DrawLine(X + W - lineW, Y + H + lineT, X + W + lineT, Y + H + lineT, linewidth, inlinecolor);
}

VOID CRender::Draw2dBox(int X, int Y, int W, int H, int linewidth, DWORD inlinecolor)
{
	DrawLine(X, Y, X + W, Y, linewidth, inlinecolor); // Top
	DrawLine(X, Y + H, X + W, Y + H, linewidth, inlinecolor); // Bottom

	DrawLine(X, Y, X, Y + H, linewidth, inlinecolor); // Left
	DrawLine(X + W, Y, X + W, Y + H, linewidth, inlinecolor); // Right
}

BOOL CRender::isCreated()
{
	return bisCreated;
}

VOID CRender::Created()
{
	bisCreated = TRUE;
}

/* Clean up */
VOID CRender::Release()
{
	if (pDevice)
		pDevice->Release();

	if (d3d)
		d3d->Release();

	if (pSprite)
		pSprite->Release();

	if (pFont)
		pFont->Release();

	if (pFontSmaller)
		pFontSmaller->Release();

	if (pFontSmallerSmaller)
		pFontSmallerSmaller->Release();

	if (line)
		line->Release();
}

/* Backup old vortext and replace it with ours. */
VOID CRender::Begin()
{
	pDevice->GetFVF(&dwOldFVF);
	pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pDevice->BeginScene();
}

/* Restore old vortex */
VOID CRender::End()
{
	pDevice->SetFVF(dwOldFVF);
	pDevice->EndScene();
}

VOID CRender::Present()
{
	pDevice->Present(0, 0, 0, 0);
}

VOID CRender::Clear()
{
	pDevice->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	//pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
}

LPDIRECT3DDEVICE9 CRender::GetDevice()
{
	return pDevice;
}

VOID CRender::CreateFonts(float Multiplier, LPCWSTR FontName)
{
	D3DXCreateFont(pDevice, (int)(11.5 * Multiplier), 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, FontName, &pFont);
	D3DXCreateFont(pDevice, (int)(10 * Multiplier), 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, FontName, &pFontSmaller);
	D3DXCreateFont(pDevice, (int)(9.5 * Multiplier), 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, FontName, &pFontSmallerSmaller);
}

VOID CRender::DrawFontText(INT x, INT y, D3DCOLOR col, PCHAR szString, ...)
{
	CHAR szBuffer[101] = "";

	va_list ArgumentList;
	va_start(ArgumentList, szString);
	vsnprintf_s(szBuffer, sizeof(szBuffer), sizeof(szBuffer) - strlen(szBuffer), szString, ArgumentList);
	va_end(ArgumentList);

	RECT FontRect = { x, y, x + 500, y + 50 };
	pFont->DrawTextA(NULL, szBuffer, -1, &FontRect, DT_LEFT | DT_WORDBREAK, col);
}

VOID CRender::DrawFontText(INT x, INT y, INT size, D3DCOLOR col, PCHAR szString, ...)
{
	static LPD3DXFONT temFont = NULL;

	if (temFont == NULL || temFont != pFontScale)
	{
		D3DXCreateFont(pDevice, size, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tahoma", &pFontScale);
		temFont = pFontScale;
	}

	CHAR szBuffer[101] = "";

	va_list ArgumentList;
	va_start(ArgumentList, szString);
	vsnprintf_s(szBuffer, sizeof(szBuffer), sizeof(szBuffer) - strlen(szBuffer), szString, ArgumentList);
	va_end(ArgumentList);

	RECT FontRect = { x, y, x + 500, y + 50 };
	pFontScale->DrawTextA(NULL, szBuffer, -1, &FontRect, DT_LEFT | DT_WORDBREAK, col);
}

VOID CRender::DrawRect(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor)
{
	CVertexList VertexList[4] =
	{
		{ X, Y + Height, 0.0f, dColor },
		{ X, Y, 0.0f, dColor },
		{ X + Width, Y + Height, 0.0f, dColor },
		{ X + Width, Y, 0.0f, dColor },
	};

	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(CVertexList));
}

VOID CRender::DrawOutlinedFontText(INT X, INT Y, CColor dColor, int type, PCHAR szString, ...)
{
	CHAR szBuffer[101] = "";

	va_list ArgumentList;
	va_start(ArgumentList, szString);
	vsnprintf_s(szBuffer, sizeof(szBuffer), sizeof(szBuffer) - strlen(szBuffer), szString, ArgumentList);
	va_end(ArgumentList);

	RECT FontRect = { X, Y, X + 500, Y + 50 };
	RECT OutlineRect1 = { X + 1, Y, X + 500, Y + 50 };
	RECT OutlineRect2 = { X, Y + 1, X + 500, Y + 50 };


	if (type == 1)
	{
		pFont->DrawTextA(NULL, szBuffer, -1, &OutlineRect1, DT_NOCLIP, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
		pFont->DrawTextA(NULL, szBuffer, -1, &OutlineRect2, DT_NOCLIP, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
		pFont->DrawTextA(NULL, szBuffer, -1, &FontRect, DT_LEFT | DT_WORDBREAK, dColor);
	}
	else if (type == 2) {
		pFontSmaller->DrawTextA(NULL, szBuffer, -1, &OutlineRect1, DT_NOCLIP, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
		pFontSmaller->DrawTextA(NULL, szBuffer, -1, &OutlineRect2, DT_NOCLIP, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
		pFontSmaller->DrawTextA(NULL, szBuffer, -1, &FontRect, DT_LEFT | DT_WORDBREAK, dColor);
	}
	else if (type == 3) {
		pFontSmallerSmaller->DrawTextA(NULL, szBuffer, -1, &OutlineRect1, DT_NOCLIP, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
		pFontSmallerSmaller->DrawTextA(NULL, szBuffer, -1, &OutlineRect2, DT_NOCLIP, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
		pFontSmallerSmaller->DrawTextA(NULL, szBuffer, -1, &FontRect, DT_LEFT | DT_WORDBREAK, dColor);
	}
}

VOID CRender::DrawSprite(LPDIRECT3DTEXTURE9 tex, FLOAT x, FLOAT y, FLOAT resolution, FLOAT scale, FLOAT rotation)
{
	FLOAT screen_pos_x = x;
	FLOAT screen_pos_y = y;

	D3DXVECTOR2 spriteCentre = D3DXVECTOR2(resolution / 2, resolution / 2);
	D3DXVECTOR2 trans = D3DXVECTOR2(screen_pos_x, screen_pos_y);

	D3DXMATRIX mat;
	D3DXVECTOR2 scaling(scale, scale);

	D3DXMatrixTransformation2D(&mat, NULL, 0.0, &scaling, &spriteCentre, rotation, &trans);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pDevice->SetPixelShader(NULL);
	pSprite->Begin(NULL);
	pSprite->SetTransform(&mat); // Tell the sprite about the matrix
	pSprite->Draw(tex, NULL, NULL, NULL, 0xFFFFFFFF);
	pSprite->End();
}

VOID CRender::DrawTexture(INT x, INT y, LPDIRECT3DTEXTURE9 dTexture, LPD3DXSPRITE sprite)
{
	sprite->Draw(dTexture, NULL, NULL, &D3DXVECTOR3(x, y, 0.0f), 0xFFFFFFFF);
	return;
}

VOID CRender::DrawImage(INT x, INT y, LPDIRECT3DTEXTURE9 dTexture, LPD3DXSPRITE sprite, std::vector<byte> MyImage)
{
	if (dTexture == NULL)
		D3DXCreateTextureFromFileInMemory(pDevice, MyImage.data(), MyImage.size(), &dTexture); //Create image from array

	if (sprite == NULL)
		D3DXCreateSprite(pDevice, &sprite);

	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	DrawTexture(x, y, dTexture, sprite);
	sprite->End();
}

VOID CRender::DrawLine(FLOAT startx, FLOAT starty, FLOAT endx, FLOAT endy, FLOAT width, CColor dCcolor)
{
	D3DXVECTOR2 lines[] = { D3DXVECTOR2(startx, starty), D3DXVECTOR2(endx, endy) };

	line->SetWidth(width);
	line->Begin();
	line->Draw(lines, 2, dCcolor);
	line->End();
}

VOID CRender::DrawCircle(FLOAT x, FLOAT y, FLOAT r, FLOAT segments, CColor dColor)
{
	std::vector<CVertexList> VertexList;
	CVertexList vertex;

	for (float angle = 0.f; angle < 2 * PI; angle += segments)
	{
		vertex.X = x + cosf(angle) * r;
		vertex.Y = y + sinf(angle) * r;
		vertex.Z = 0.f;
		vertex.dColor = dColor;

		VertexList.push_back(vertex);
	}

	vertex.X = x + cosf(2 * PI) * r;
	vertex.Y = y + sinf(2 * PI) * r;
	vertex.Z = 0.f;
	vertex.dColor = dColor;

	VertexList.push_back(vertex);

	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, VertexList.size() - 1, VertexList.data(), sizeof(CVertexList));
}
VOID CRender::DrawFilledCircle(FLOAT x, FLOAT y, FLOAT r, FLOAT smoothness, CColor dColor)
{
	std::vector<CVertexList> VertexList;
	CVertexList vertex;

	vertex.X = x;
	vertex.Y = y;
	vertex.Z = 0.f;
	vertex.dColor = dColor;

	VertexList.push_back(vertex);

	for (float angle = 0.f; angle < 2 * PI; angle += smoothness)
	{
		vertex.X = x + cosf(angle) * r;
		vertex.Y = y + sinf(angle) * r;
		vertex.Z = 0.f;
		vertex.dColor = dColor;

		VertexList.push_back(vertex);
	}

	vertex.X = x + cosf(2 * PI) * r;
	vertex.Y = y + sinf(2 * PI) * r;
	vertex.Z = 0.f;
	vertex.dColor = dColor;

	VertexList.push_back(vertex);

	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, VertexList.size() - 2, VertexList.data(), sizeof(CVertexList));
}


VOID CRender::DrawOutlinedRect(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, CColor dColor)
{
	CVertexList VertexList[5] =
	{
		{ X, Y + Height, 0.0f, dColor },
		{ X, Y, 0.0f, dColor },
		{ X + Width, Y, 0.0f, dColor },
		{ X + Width, Y + Height, 0.0f, dColor },
		{ X, Y + Height, 0.0f, dColor }
	};


	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, VertexList, sizeof(CVertexList));
}