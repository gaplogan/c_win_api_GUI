#include <windows.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";

HBITMAP g_hbmBall = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
            // In�cio Centralizar Janela
            // RECT rect;
            // GetWindowRect(hwnd, &rect);

            // MoveWindow(hwnd, (GetSystemMetrics(SM_CXSCREEN)-rect.top)/2, (GetSystemMetrics(SM_CYSCREEN)-rect.left)/2, rect.top, rect.left, TRUE);
            // Fim centralizar janela

			g_hbmBall = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BALL));
			if(g_hbmBall == NULL)
				MessageBox(hwnd, "Could not load IDB_BALL!", "Error", MB_OK | MB_ICONEXCLAMATION);
		break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_PAINT:
		{
			// Apenas uma observa��o: nunca use uma MessageBox de dentro de WM_PAINT A caixa causar� mais mensagens WM_PAINT e voc� provavelmente ficar� preso em um loop

            // BITMAP � uma estrutura que cont�m informa��es sobre um HBITMAP que � o objeto GDI real.
			BITMAP bm;

            // Estrutura que cont�m informa��es sobre a janela que est� sendo pintada e o que exatamente est� acontecendo com a mensagem de pintura. Para a maioria das tarefas simples, voc� pode simplesmente ignora as informa��es que ele cont�m, mas � necess�rio para a chamada para BeginPaint()
			PAINTSTRUCT ps;

            // BeginPaint() como seu nome sugere, � projetado especificamente para lidar com a mensagem WM_PAINT. Quando WM_PAINT n�o estiver lidando com uma mensagem voc� usaria, o GetDC(), mas em WM_PAINT, � importante usar BeginPaint() e EndPaint().
            //BeginPaint() nos retorna um HDCque representa o HWND que passamos para ele, aquele que WM_PAINT est� sendo tratado. Qualquer opera��o de desenho que realizarmos HDC ser� exibida imediatamente na tela.
			HDC hdc = BeginPaint(hwnd, &ps);

            // Para desenhar em/ou com bitmaps, precisamos criar um DC na mem�ria ... a maneira mais f�cil de fazer isso aqui � CreateCompatibleDC() com o que j� temos. Isso nos d� uma mem�ria DC compat�vel com a profundidade de cor e as propriedades de exibi��o HDC da janela.
			HDC hdcMem = CreateCompatibleDC(hdc);

            // Agora chamamos SelectObject()para selecionar o bitmap no DC, tendo o cuidado de armazenar o bitmap padr�o para que possamos substitu�-lo posteriormente e n�o vazar objetos GDI.
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmBall);

            // N�o obt�m realmente um objeto, mas sim informa��es sobre um existente.
			GetObject(g_hbmBall, sizeof(bm), &bm);

            // Assim que tivermos preenchido as dimens�es do bitmap na estrutura BITMAP, podemos chamar BitBlt() para copiar a imagem de nosso Memory DC para o Window DC, exibindo-a na tela.
            // destino, x, y, width, height, fonte, xFonte, yFonte, Opera��o Raster (c�digo ROP), que especifica como fazer a c�pia. Neste caso (SRCCOPY), queremos uma c�pia exata simples da fonte feita, sem coisas complicadas.
			BitBlt(hdc, 100, 50, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

            // Neste ponto, o bitmap deve estar na tela e precisamos limpar depois de n�s mesmos. A primeira coisa a fazer � restaurar a mem�ria DC ao estado em que estava quando o obtivemos, o que significa substituir nosso bitmap pelo padr�o que salvamos. Em seguida, podemos exclu�-lo completamente com DeleteDC().
			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
            
            // Finalmente, liberamos o Window DC que obtivemos com o BeginPaint() usando EndPaint().
            // Destruir um HDC� um pouco confuso �s vezes, porque existem pelo menos 3 maneiras de fazer isso, dependendo de como voc� o conseguiu. Aqui est� uma lista dos m�todos comuns de obten��o de um HDC e como liber�-lo quando terminar.
                // GetDC() - ReleaseDC()
                // BeginPaint() - EndPaint()
                // CreateCompatibleDC() - DeleteDC()
			EndPaint(hwnd, &ps);
		}
		break;
		case WM_DESTROY:
            // E, finalmente, ao finalizar nosso programa, queremos liberar todos os recursos que alocamos. Tecnicamente falando, isso n�o � absolutamente necess�rio, j� que as plataformas modernas do Windows s�o muito boas em liberar tudo quando o seu programa existe, mas � sempre uma boa ideia manter o controle de seus pr�prios objetos porque se ficar pregui�oso e n�o exclu�-los, eles t�m um h�bito de se soltar. E, sem d�vida, ainda existem bugs no Windows, especialmente nas vers�es mais antigas, que n�o limpar�o todos os seus objetos GDI se voc� n�o fizer um trabalho completo.

			DeleteObject(g_hbmBall);
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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
    // Para centralizar a janela
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int winWidth = 800;
    int winHeight = 600;
    int winX = (screenWidth - winWidth) / 2;
    int winY = (screenHeight - winHeight) / 2;

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Aplica��o Bitmap",
		WS_OVERLAPPEDWINDOW,
		winX, winY, winWidth, winHeight,
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


// compilar: g++ -o bmp_one bmp_one.c bmp_one.o -lgdi32