#include <windows.h>
#include <commctrl.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";
const char g_szChildClassName[] = "myMDIChildWindowClass";

#define IDC_MAIN_MDI	101
#define IDC_MAIN_TOOL	102
#define IDC_MAIN_STATUS	103

#define IDC_CHILD_EDIT	101

#define ID_MDI_FIRSTCHILD 50000

HWND g_hMDIClient = NULL;
HWND g_hMainWindow = NULL;

BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;

		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF)
		{
			LPSTR pszFileText;

			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL)
			{
				DWORD dwRead;

				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					pszFileText[dwFileSize] = 0; // Add null terminator
					if(SetWindowText(hEdit, pszFileText))
						bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwTextLength;

		dwTextLength = GetWindowTextLength(hEdit);
		// No need to bother if there's no text.
		if(dwTextLength > 0)
		{
			LPSTR pszText;
			DWORD dwBufferSize = dwTextLength + 1;

			pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
			if(pszText != NULL)
			{
				if(GetWindowText(hEdit, pszText, dwBufferSize))
				{
					DWORD dwWritten;

					if(WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
						bSuccess = TRUE;
				}
				GlobalFree(pszText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

void DoFileOpen(HWND hwnd)
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";

	if(GetOpenFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_CHILD_EDIT);
		if(LoadTextFileToEdit(hEdit, szFileName))
		{
			SendDlgItemMessage(g_hMainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)"Opened...");
			SendDlgItemMessage(g_hMainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)szFileName);

			SetWindowText(hwnd, szFileName);
		}
	}
}

void DoFileSave(HWND hwnd)
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "txt";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if(GetSaveFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_CHILD_EDIT);
		if(SaveTextFileFromEdit(hEdit, szFileName))
		{
			SendDlgItemMessage(g_hMainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)"Saved...");
			SendDlgItemMessage(g_hMainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)szFileName);

			SetWindowText(hwnd, szFileName);
		}
	}
}

HWND CreateNewMDIChild(HWND hMDIClient)
{
	MDICREATESTRUCT mcs;
	HWND hChild;

	mcs.szTitle = "[Untitled]";
	mcs.szClass = g_szChildClassName;
	mcs.hOwner  = GetModuleHandle(NULL);
	mcs.x = mcs.cx = CW_USEDEFAULT;
	mcs.y = mcs.cy = CW_USEDEFAULT;
	mcs.style = MDIS_ALLCHILDSTYLES;

	hChild = (HWND)SendMessage(hMDIClient, WM_MDICREATE, 0, (LONG)&mcs);
	if(!hChild)
	{
		MessageBox(hMDIClient, "MDI Child creation failed.", "Oh Oh...",
			MB_ICONEXCLAMATION | MB_OK);
	}
	return hChild;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			HWND hTool;
			TBBUTTON tbb[3];
			TBADDBITMAP tbab;

			HWND hStatus;
			int statwidths[] = {100, -1};

			CLIENTCREATESTRUCT ccs;

			// Criar MDI Client

			// Encontre o menu da janela onde as filhas ser�o listadas
            // Identificador do menu ao qual o cliente MDI adicionar� itens para identificar cada janela que � criada, neste caso menu indice 2 (men� window). 
			ccs.hWindowMenu  = GetSubMenu(GetMenu(hwnd), 2);
            // N�mero para usar como o primeiro ID para os itens que o Cliente adiciona ao menu Janela ... voc� quer que seja facilmente distingu�vel de seus pr�prios identificadores de menu para que possa manipular seus comandos de menu e passar os comandos do menu Janela DefFrameProc()para processamento. No exemplo, especifico um identificador definido como 50000, alto o suficiente para que eu saiba que nenhum dos meus IDs de comando de menu estar� acima dele.
			ccs.idFirstChild = ID_MDI_FIRSTCHILD;

			g_hMDIClient = CreateWindowEx(WS_EX_CLIENTEDGE, "mdiclient", NULL,
				WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				hwnd, (HMENU)IDC_MAIN_MDI, GetModuleHandle(NULL), (LPVOID)&ccs);

			if(g_hMDIClient == NULL)
				MessageBox(hwnd, "Could not create MDI client.", "Error", MB_OK | MB_ICONERROR);

			// Create Toolbar

			hTool = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
				hwnd, (HMENU)IDC_MAIN_TOOL, GetModuleHandle(NULL), NULL);
			if(hTool == NULL)
				MessageBox(hwnd, "Could not create tool bar.", "Error", MB_OK | MB_ICONERROR);

			// Send the TB_BUTTONSTRUCTSIZE message, which is required for
			// backward compatibility.
			SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
			
			tbab.hInst = HINST_COMMCTRL;
			tbab.nID = IDB_STD_SMALL_COLOR;
			SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

			ZeroMemory(tbb, sizeof(tbb));
			tbb[0].iBitmap = STD_FILENEW;
			tbb[0].fsState = TBSTATE_ENABLED;
			tbb[0].fsStyle = TBSTYLE_BUTTON;
			tbb[0].idCommand = ID_FILE_NEW;

			tbb[1].iBitmap = STD_FILEOPEN;
			tbb[1].fsState = TBSTATE_ENABLED;
			tbb[1].fsStyle = TBSTYLE_BUTTON;
			tbb[1].idCommand = ID_FILE_OPEN;

			tbb[2].iBitmap = STD_FILESAVE;
			tbb[2].fsState = TBSTATE_ENABLED;
			tbb[2].fsStyle = TBSTYLE_BUTTON;
			tbb[2].idCommand = ID_FILE_SAVEAS;

			SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb);

			// Create Status bar

			hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
				WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
				hwnd, (HMENU)IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);

			SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
			SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Hi there :)");
		}
		break;
		case WM_SIZE:
		{
			HWND hTool;
			RECT rcTool;
			int iToolHeight;

			HWND hStatus;
			RECT rcStatus;
			int iStatusHeight;

			HWND hMDI;
			int iMDIHeight;
			RECT rcClient;

			// Size toolbar and get height

			hTool = GetDlgItem(hwnd, IDC_MAIN_TOOL);
			SendMessage(hTool, TB_AUTOSIZE, 0, 0);

			GetWindowRect(hTool, &rcTool);
			iToolHeight = rcTool.bottom - rcTool.top;

			// Size status bar and get height

			hStatus = GetDlgItem(hwnd, IDC_MAIN_STATUS);
			SendMessage(hStatus, WM_SIZE, 0, 0);

			GetWindowRect(hStatus, &rcStatus);
			iStatusHeight = rcStatus.bottom - rcStatus.top;

			// Calculate remaining height and size edit
            // O c�digo para dimensionar o cliente MDI � o mesmo que o c�digo para redimensionar o controle de edi��o no �ltimo exemplo, que leva em considera��o o tamanho e a posi��o da ferramenta e as barras de status para que n�o sobreponha a janela do cliente MDI.
			GetClientRect(hwnd, &rcClient);

			iMDIHeight = rcClient.bottom - iToolHeight - iStatusHeight;

			hMDI = GetDlgItem(hwnd, IDC_MAIN_MDI);
			SetWindowPos(hMDI, NULL, 0, iToolHeight, rcClient.right, iMDIHeight, SWP_NOZORDER);
		}
		break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
				break;
				case ID_FILE_NEW:
					CreateNewMDIChild(g_hMDIClient);
				break;
				case ID_FILE_OPEN:
				{
					HWND hChild = CreateNewMDIChild(g_hMDIClient);
					if(hChild)
					{
						DoFileOpen(hChild);	
					}
				}
				break;
				case ID_FILE_CLOSE:
				{
					HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE,0,0);
					if(hChild)
					{
						SendMessage(hChild, WM_CLOSE, 0, 0);
					}
				}
				break;
				case ID_WINDOW_TILE:
					SendMessage(g_hMDIClient, WM_MDITILE, 0, 0);
				break;
				case ID_WINDOW_CASCADE:
					SendMessage(g_hMDIClient, WM_MDICASCADE, 0, 0);
				break;
                // Captura todos os comandos que n�o processei diretamente e faz uma verifica��o para ver se o valor � maior ou igual a ID_MDI_FIRSTCHILD.
				default:
				{
					if(LOWORD(wParam) >= ID_MDI_FIRSTCHILD)
					{
                        // Se for um dos IDs de janela, o usu�rio clicou em um dos itens do menu 'Window' e enviamos a mensagem para DefFrameProc() para processamento.
						DefFrameProc(hwnd, g_hMDIClient, WM_COMMAND, wParam, lParam);
					}
					else 
					{
                        // Se n�o for um dos IDs de janela, pego o identificador para a janela filha ativa e encaminho a mensagem para ela processar. Isso permite que voc� delegue responsabilidade �s janelas filhas pela execu��o de determinadas a��es e permite que diferentes janelas filhas tratem os comandos de maneiras diferentes, se desejado. No exemplo, eu apenas trato de comandos que s�o globais para o programa no procedimento da janela Frame e envio os comandos que afetam um determinado documento ou janela filho para a pr�pria janela filho para processamento.
						HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE,0,0);
						if(hChild)
						{
							SendMessage(hChild, WM_COMMAND, wParam, lParam);
						}
					}
				}
			}
		break;
		default:
        // J� que estamos criando uma janela Frame que hospedar� um cliente MDI, precisamos alterar a chamada DefWindowProc() para a DefFrameProc() a qual adiciona tratamento especializado de mensagens para Frame Windows
			return DefFrameProc(hwnd, g_hMDIClient, msg, wParam, lParam);
	}
	return 0;
}

// O procedimento de janela para um filho MDI � muito parecido com qualquer outro, com algumas pequenas exce��es. Em primeiro lugar, as mensagens padr�o s�o passadas para em DefMDIChildProc()vez de DefWindowProc().
// Neste caso particular, n�s tamb�m queremos desabilitar os menus Editar e Window quando eles n�o forem necess�rios (apenas porque � uma coisa legal de se fazer), ent�o n�s os manipulamos WM_MDIACTIVEATE e habilitamos ou desabilitamos dependendo se nossa janela est� sendo ativada ou n�o . Se voc� tiver v�rios tipos de janela filho, � aqui que voc� pode colocar o c�digo para mudar completamente o menu ou barra de ferramentas ou fazer altera��es em outros aspectos do programa para refletir as a��es e comandos que s�o espec�ficos ao tipo de janela que est� sendo ativada.
// Para ser ainda mais completo, podemos desabilitar os itens de menu Fechar e Salvar Arquivo tamb�m, j� que eles n�o ser�o nada bons sem janelas para atuar. Desativei todos esses itens por padr�o no recurso para que n�o precise adicionar c�digo extra para fazer isso quando o aplicativo � inicializado pela primeira vez.
LRESULT CALLBACK MDIChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			HFONT hfDefault;
			HWND hEdit;

			// Create Edit Control

			hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 0, 0, 100, 100, hwnd, (HMENU)IDC_CHILD_EDIT, GetModuleHandle(NULL), NULL);
			if(hEdit == NULL)
				MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

            // Um membro MDICREATESTRUCTque n�o usei e que pode ser bastante �til � o lParammembro. Isso pode ser usado para enviar qualquer valor de 32 bits (como um ponteiro) para o filho que voc� est� criando, a fim de fornecer qualquer informa��o personalizada que voc� escolher. No WM_CREATEmanipulador da janela filha, o lParamvalor da WM_CREATE mensagem apontar� para a CREATESTRUCT. o lpCreateParamsmembro dessa estrutura apontar� para o que MDICREATESTRUCTvoc� enviou junto com WM_MDICREATE. Portanto, para acessar o lParamvalor da janela filho, voc� precisa fazer algo assim no procedimento da janela filho ...

            // CREATESTRUCT *pCreateStruct;
            // MDICREATESTRUCT *pMDICreateStruct;

            // pCreateStruct = (CREATESTRUCT *) lParam;
            // pMDICreateStruct = (MDICREATESTRUCT *) pCreateStruct-> lpCreateParams;
            
            // pMDICreateStruct agora aponta para o mesmo MDICREATESTRUCT que voc� enviada junto com a mensagem WM_MDICREATE e voc� pode us�-la para acessar o lParam.

            // Se voc� n�o quiser se preocupar com essas duas dicas extras, voc� pode acessar o lParam em uma etapa com ((MDICREATESTRUCT*)((CREATESTRUCT*)lParam)->lpCreateParams)->lParam         
		}
		break;
		case WM_MDIACTIVATE:
		{
			HMENU hMenu, hFileMenu;
			UINT EnableFlag;

			hMenu = GetMenu(g_hMainWindow);
			if(hwnd == (HWND)lParam)
			{   // Sendo ativado, ativa os menus
				EnableFlag = MF_ENABLED;
			}
			else
			{   // Sendo desativado, desativa os menus
				EnableFlag = MF_GRAYED;
			}

            // Ativando / desativando menu Editar
			EnableMenuItem(hMenu, 1, MF_BYPOSITION | EnableFlag);
            // Ativando / desativando menu Window
			EnableMenuItem(hMenu, 2, MF_BYPOSITION | EnableFlag);

            // Obtendo menu Arquivo
			hFileMenu = GetSubMenu(hMenu, 0);
            // Ativando / desativando menu item Salvar Como
			EnableMenuItem(hFileMenu, ID_FILE_SAVEAS, MF_BYCOMMAND | EnableFlag);
            // Ativando / desativando menu item Fechar
			EnableMenuItem(hFileMenu, ID_FILE_CLOSE, MF_BYCOMMAND | EnableFlag);
            // Ativando / desativando menu item Fechar Todos
			EnableMenuItem(hFileMenu, ID_FILE_CLOSEALL, MF_BYCOMMAND | EnableFlag);

            // Redesenhar a barra de menus
			DrawMenuBar(g_hMainWindow);
		}
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_FILE_OPEN:
					DoFileOpen(hwnd);
				break;
				case ID_FILE_SAVEAS:
					DoFileSave(hwnd);
				break;
				case ID_EDIT_CUT:
					SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_CUT, 0, 0);
				break;
				case ID_EDIT_COPY:
					SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_COPY, 0, 0);
				break;
				case ID_EDIT_PASTE:
					SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_PASTE, 0, 0);
				break;
			}
		break;
		case WM_SIZE:
		{
			HWND hEdit;
			RECT rcClient;

			// Calculate remaining height and size edit

			GetClientRect(hwnd, &rcClient);

			hEdit = GetDlgItem(hwnd, IDC_CHILD_EDIT);
			SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		}
		return DefMDIChildProc(hwnd, msg, wParam, lParam);
		default:
			return DefMDIChildProc(hwnd, msg, wParam, lParam);
	
	}
	return 0;
}

// Al�m da janela principal do programa (a janela Frame), precisamos criar novas classes de janela para cada tipo de janela filha que desejamos. Por exemplo, voc� pode ter um para exibir texto e outro para exibir uma imagem ou gr�fico. Neste exemplo, criaremos apenas um tipo filho, que ser� como o programa editor dos exemplos anteriores.
BOOL SetUpMDIChildWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	 = MDIChildWndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szChildClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(0, "Could Not Register Child Window", "Oh Oh...", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}
	else
		return TRUE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	InitCommonControls();

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	if(!SetUpMDIChildWindowClass(hInstance))
		return 0;

	hwnd = CreateWindowEx(
		0,
		g_szClassName,
		"Aplica��o C++ GUI",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	g_hMainWindow = hwnd;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
        // Adicionamos uma etapa extra ( TranslateMDISysAccel()), que verifica as teclas de acelera��o predefinidas, Ctrl + F6 que muda para a pr�xima janela, Ctrl + F4 que fecha o filho e assim por diante. Se voc� n�o adicionar esta verifica��o, voc� irritar� seus usu�rios por n�o fornecer o comportamento padr�o a que eles est�o acostumados ou ter� que implement�-lo manualmente.
		if (!TranslateMDISysAccel(g_hMDIClient, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	return Msg.wParam;
}

// compilar: g++ -o app_four app_four.c app_four.o -lcomctl32 -lgdi32 -mwindows