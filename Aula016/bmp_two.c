#include <windows.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";

// Dar aos bitmaps a aparência de seções transparentes é bastante simples e envolve o uso de uma imagem de máscara em preto e branco, além da imagem colorida que queremos parecer transparente.
// As seguintes condições devem ser atendidas para que o efeito funcione corretamente: Primeiro, a imagem colorida deve ser preta em todas as áreas que desejamos exibir como transparente . E, em segundo lugar, a imagem da máscara deve ser branca nas áreas que queremos transparentes e preta em outras áreas. As imagens coloridas e de máscara são exibidas como as duas imagens mais à esquerda na imagem de exemplo desta página.

HBITMAP g_hbmBall = NULL;
HBITMAP g_hbmMask = NULL;

// Criação de Máscara
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

    // Cria bitmap de máscara monocromática (1 bit).
	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

    // Obtenha alguns HDCs compatíveis com o driver de exibição
	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);

    // Defina a cor de fundo da imagem colorida para a cor que você deseja que seja transparente.
	SetBkColor(hdcMem, crTransparent);

    // Operações BitBlt
    // Como isso nos traz transparência? Primeiro, usamos BitBlt() a imagem da máscara usando a operação SRCAND como o último parâmetro e, depois disso, BitBlt() a imagem colorida usando a operação SRCPAINT. O resultado é que as áreas que queríamos transparentes não mudam no HDC de destino enquanto o resto da imagem é desenhado normalmente.

    // Copie os bits da imagem colorida para a máscara B + W ... tudo com a cor de fundo acaba sendo branco enquanto todo o resto acaba preto ... Exatamente o que queríamos.
	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

    // Pegue nossa nova máscara e use-a para transformar a cor transparente em nossa imagem de cor original em preto para que o efeito de transparência funcione direito.
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

    // Limpar.
	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
			g_hbmBall = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BALL));
			if(g_hbmBall == NULL)
				MessageBox(hwnd, "Não foi possível carregar IDB_BALL!", "Error", MB_OK | MB_ICONEXCLAMATION);

			g_hbmMask = CreateBitmapMask(g_hbmBall, RGB(0, 0, 0));
			if(g_hbmMask == NULL)
				MessageBox(hwnd, "Não foi possível criar a máscara!", "Error", MB_OK | MB_ICONEXCLAMATION);
		break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_PAINT:
		{
			BITMAP bm;
			RECT rcClient;
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd, &ps);

			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmMask);

			GetObject(g_hbmBall, sizeof(bm), &bm);

			GetClientRect(hwnd, &rcClient);
			FillRect(hdc, &rcClient, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
			BitBlt(hdc, bm.bmWidth, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND);
			BitBlt(hdc, bm.bmWidth * 2, bm.bmHeight * 2, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND);

			SelectObject(hdcMem, g_hbmBall);
			BitBlt(hdc, 0, bm.bmHeight, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
			BitBlt(hdc, bm.bmWidth, bm.bmHeight, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT);
			BitBlt(hdc, bm.bmWidth * 2, bm.bmHeight * 2, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);

			EndPaint(hwnd, &ps);
		}
		break;
		case WM_DESTROY:
			DeleteObject(g_hbmBall);
			DeleteObject(g_hbmMask);

			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Outro programa de bitmap",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 250,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

// compilar: g++ -o bmp_two bmp_two.c bmp_two.o -lgdi32