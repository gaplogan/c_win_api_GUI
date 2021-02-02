#include <windows.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";

#define IDC_MAIN_EDIT 101

// Existe uma fun��o completa para ler um arquivo de texto em um controle de edi��o. Leva como par�metros o identificador para o controle de edi��o e o nome do arquivo a ser lido. Esta fun��o perticular tem uma boa verifica��o de erros, o IO do arquivo � um lugar onde muitas coisas podem dar errado, ent�o voc� precisa esteja atento a erros.
BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	// GENERIC_READ: apenas acesso de leitura
	// FILE_SHARE_READ: tudo bem se outros programas abrirem o arquivo ao mesmo tempo que n�s, mas SOMENTE se eles quiserem ler tamb�m, n�o queremos que eles gravem no arquivo enquanto o estamos lendo.
	// OPEN_EXISTING: apenas abrir o arquivo se ele j� existir, n�o cri�-lo e n�o substitu�-lo.
	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	
	// Verificar se o identificador do arquivo � v�lido.
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;

		// Verificar o tamanho do arquivo para saber quanta mem�ria precisa alocar para ler tudo.
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF)
		{
			// Alocamos a mem�ria, verificamos se a aloca��o foi bem-sucedida
			LPSTR pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL)
			{
				// N�o o usamos, exceto como um par�metro em ReadFile(). Este par�metro DEVE ser fornecido, ou a chamada falhar� sem ele.
				DWORD dwRead;

				// Carregar o conte�do do arquivo em nosso buffer de mem�ria.
				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					// As fun��es do arquivo API n�o t�m nenhum conceito de arquivos de texto, portanto, n�o far�o coisas como ler uma �nica linha de texto ou adicionar terminadores NULL ao final de nossas strings. � por isso que alocamos um byte extra e, depois de ler o arquivo, adicionamos NULL n�s mesmos para que possamos ent�o passar o buffer de mem�ria como uma string para SetWindowText().
					pszFileText[dwFileSize] = 0; // Add null terminator
					if(SetWindowText(hEdit, pszFileText))
						// Se foi bem-sucedido, definimos a vari�vel de sucesso para TRUE
						bSuccess = TRUE; // Funciona!
				}
				// Liberando o buffer de mem�ria.
				GlobalFree(pszFileText);
			}
		}
		// Fechando o identificador de arquivo.
		CloseHandle(hFile);
	}
	// Finalmente retornar ao chamador.
	return bSuccess;
}

// Salvando/Escrevendo o arquivo.
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	// Especificamos que queremos acesso de Leitura, que o arquivo deve ser sempre criado novo (e se existir ser� apagado ao ser aberto) e que se n�o existir, ser� criado com o atributos de arquivo normais.
	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		// Obtemos o comprimento do buffer de mem�ria necess�rio do controle de edi��o, uma vez que esta � a fonte dos dados.
		DWORD dwTextLength = GetWindowTextLength(hEdit);

		// N�o precisa se preocupar se n�o houver texto.
		if(dwTextLength > 0)
		{
			LPSTR pszText;

			// Definir o tamanho do buffer de mem�ria.
			DWORD dwBufferSize = dwTextLength + 1;

			// Alocar a mem�ria
			pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
			if(pszText != NULL)
			{
				// Solicitamos a string do controle de edi��o usando GetWindowText().
				if(GetWindowText(hEdit, pszText, dwBufferSize))
				{
					DWORD dwWritten;

					// Gravamos no arquivo com WriteFile().
					// Novamente, como com ReadFile() o par�metro que retorna o quanto foi realmente escrito � necess�rio, embora n�o o utilizemos.
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

	// Para inicializar a estrutura como 0. Dessa forma, voc� n�o precisa definir explicitamente cada membro que n�o usa.
	ZeroMemory(&ofn, sizeof(ofn));

	// Especifica o comprimento, em bytes, da estrutura. Alternativa para vers�es anteriores do windows: OPENFILENAME_SIZE_VERSION_400
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	// Filtros para os tipos de arquivos que queremos utilizar
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	// Pontos para o buffer que alocamos para armazenar o nome do arquivo, uma vez que os nomes dos arquivos n�o podem ser maiores do que MAX_PATH este � o valor que escolhi para o tamanho do buffer.
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	// Os sinalizadores indicam que a caixa de di�logo s� deve permitir ao usu�rio inserir nomes de arquivos j� existentes (j� que queremos abri-los, n�o cri�-los) e ocultar a op��o de abrir o arquivo no modo somente leitura, que n�o iremos oferecer suporte.
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	// Extens�o padr�o, ent�o se o usu�rio digitar "nome_arquivo" e o arquivo n�o for encontrado, ele tentar� abrir "nome_arquivo.txt" antes de finalmente desistir.
	ofn.lpstrDefExt = "txt";

	// Tentar abrir o arquivo
	if(GetOpenFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		LoadTextFileToEdit(hEdit, szFileName);
	}
}

void DoFileSave(HWND hwnd)
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	// Para inicializar a estrutura como 0. Dessa forma, voc� n�o precisa definir explicitamente cada membro que n�o usa.
	ZeroMemory(&ofn, sizeof(ofn));

	// Especifica o comprimento, em bytes, da estrutura. Alternativa para vers�es anteriores do windows: OPENFILENAME_SIZE_VERSION_400
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "txt";
	// Nesse caso, n�o queremos mais exigir que o arquivo exista, mas queremos que o diret�rio exista, pois n�o vamos tentar cri�-lo primeiro. Tamb�m perguntaremos ao usu�rio se ele seleciona um arquivo existente para certificar-se de que deseja sobrescrev�-lo.
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if(GetSaveFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		SaveTextFileFromEdit(hEdit, szFileName);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			HFONT hfDefault;
			HWND hEdit;

			hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
				0, 0, 100, 100, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			if(hEdit == NULL)
				MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		}
		break;
		case WM_SIZE:
		{
			HWND hEdit;
			RECT rcClient;

			GetClientRect(hwnd, &rcClient);

			hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
			SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
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
					SetDlgItemText(hwnd, IDC_MAIN_EDIT, "");
				break;
				case ID_FILE_OPEN:
					DoFileOpen(hwnd);
				break;
				case ID_FILE_SAVEAS:
					DoFileSave(hwnd);
				break;
			}
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
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		0,
		g_szClassName,
		"Tutorial WinApi C",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
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

// compilar: g++ -o app_two app_two.c app_two.o -lcomctl32 -lgdi32 -mwindows
