#include <windows.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";

HBITMAP g_hbmBall = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
            // Início Centralizar Janela
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
			// Apenas uma observação: nunca use uma MessageBox de dentro de WM_PAINT A caixa causará mais mensagens WM_PAINT e você provavelmente ficará preso em um loop

            // BITMAP é uma estrutura que contém informações sobre um HBITMAP que é o objeto GDI real.
			BITMAP bm;

            // Estrutura que contém informações sobre a janela que está sendo pintada e o que exatamente está acontecendo com a mensagem de pintura. Para a maioria das tarefas simples, você pode simplesmente ignora as informações que ele contém, mas é necessário para a chamada para BeginPaint()
			PAINTSTRUCT ps;

            // BeginPaint() como seu nome sugere, é projetado especificamente para lidar com a mensagem WM_PAINT. Quando WM_PAINT não estiver lidando com uma mensagem você usaria, o GetDC(), mas em WM_PAINT, é importante usar BeginPaint() e EndPaint().
            //BeginPaint() nos retorna um HDCque representa o HWND que passamos para ele, aquele que WM_PAINT está sendo tratado. Qualquer operação de desenho que realizarmos HDC será exibida imediatamente na tela.
			HDC hdc = BeginPaint(hwnd, &ps);

            // Para desenhar em/ou com bitmaps, precisamos criar um DC na memória ... a maneira mais fácil de fazer isso aqui é CreateCompatibleDC() com o que já temos. Isso nos dá uma memória DC compatível com a profundidade de cor e as propriedades de exibição HDC da janela.
			HDC hdcMem = CreateCompatibleDC(hdc);

            // Agora chamamos SelectObject()para selecionar o bitmap no DC, tendo o cuidado de armazenar o bitmap padrão para que possamos substituí-lo posteriormente e não vazar objetos GDI.
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmBall);

            // Não obtém realmente um objeto, mas sim informações sobre um existente.
			GetObject(g_hbmBall, sizeof(bm), &bm);

            // Assim que tivermos preenchido as dimensões do bitmap na estrutura BITMAP, podemos chamar BitBlt() para copiar a imagem de nosso Memory DC para o Window DC, exibindo-a na tela.
            // destino, x, y, width, height, fonte, xFonte, yFonte, Operação Raster (código ROP), que especifica como fazer a cópia. Neste caso (SRCCOPY), queremos uma cópia exata simples da fonte feita, sem coisas complicadas.
			BitBlt(hdc, 100, 50, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

            // Neste ponto, o bitmap deve estar na tela e precisamos limpar depois de nós mesmos. A primeira coisa a fazer é restaurar a memória DC ao estado em que estava quando o obtivemos, o que significa substituir nosso bitmap pelo padrão que salvamos. Em seguida, podemos excluí-lo completamente com DeleteDC().
			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
            
            // Finalmente, liberamos o Window DC que obtivemos com o BeginPaint() usando EndPaint().
            // Destruir um HDCé um pouco confuso às vezes, porque existem pelo menos 3 maneiras de fazer isso, dependendo de como você o conseguiu. Aqui está uma lista dos métodos comuns de obtenção de um HDC e como liberá-lo quando terminar.
                // GetDC() - ReleaseDC()
                // BeginPaint() - EndPaint()
                // CreateCompatibleDC() - DeleteDC()
			EndPaint(hwnd, &ps);
		}
		break;
		case WM_DESTROY:
            // E, finalmente, ao finalizar nosso programa, queremos liberar todos os recursos que alocamos. Tecnicamente falando, isso não é absolutamente necessário, já que as plataformas modernas do Windows são muito boas em liberar tudo quando o seu programa existe, mas é sempre uma boa ideia manter o controle de seus próprios objetos porque se ficar preguiçoso e não excluí-los, eles têm um hábito de se soltar. E, sem dúvida, ainda existem bugs no Windows, especialmente nas versões mais antigas, que não limparão todos os seus objetos GDI se você não fizer um trabalho completo.

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
		"Aplicação Bitmap",
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