#include <windows.h>

#define ID_FILE_EXIT 9001
#define ID_STUFF_GO 9002

const char *g_szClassName = "myWindowClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
        case WM_CREATE:
        {
			// Objetos para guardar os menus
            HMENU hMenu, hSubMenu;
			// Objetos para guardar os icones
            HICON hIcon, hIconSm;

			// Criando o Menu
            hMenu = CreateMenu();
			
			// Criando os itens de menu e adicionando ao menu
            hSubMenu = CreatePopupMenu();
            AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "E&xit");
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");
            
			// Criando os itens de menu e adicionando ao menu
			hSubMenu = CreatePopupMenu();
            AppendMenu(hSubMenu, MF_STRING, ID_STUFF_GO, "&Go");
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Stuff");

			// Configurando o menu na janela
            SetMenu(hwnd, hMenu);

			// Criando o objeto de icone grande 32x32
            hIcon = (HICON)LoadImage(NULL, "menu_two.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
            if(hIcon)
				// Adicionando o icone na janela
                SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            else
                MessageBox(hwnd, "Não é possível carregar o ícone grande!", "Error", MB_OK | MB_ICONERROR);

			// Criando o objeto de icone pequeno
            hIconSm = (HICON)LoadImage(NULL, "menu_two.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
            if(hIconSm)
				// Adicionando o icone na janela
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
            else
                MessageBox(hwnd, "Não é possível carregar o ícone pequeno!", "Error", MB_OK | MB_ICONERROR);
        }
        break;
		case WM_COMMAND: // Comandos do menu
			// Verifica qual comando de menu foi acinado
			switch (LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
				break;
				case ID_STUFF_GO:
					MessageBox(hwnd, "Clicou no stuff go", "Clique menu", MB_OK | MB_ICONINFORMATION);
				break;
			}
		break;
		case WM_LBUTTONDOWN:
        {
			MessageBox(hwnd, "Clicou com o botão esquerdo", "Clique do Mouse", MB_OK | MB_ICONINFORMATION);
        }
        break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    FreeConsole();

	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = NULL;

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, g_szClassName, "O título da minha janela", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Falha na criação da janela!", "Error!",
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