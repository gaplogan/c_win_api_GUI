#include <windows.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";

#define IDC_MAIN_EDIT 101

// Existe uma função completa para ler um arquivo de texto em um controle de edição. Leva como parâmetros o identificador para o controle de edição e o nome do arquivo a ser lido. Esta função perticular tem uma boa verificação de erros, o IO do arquivo é um lugar onde muitas coisas podem dar errado, então você precisa esteja atento a erros.
BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	// GENERIC_READ: apenas acesso de leitura
	// FILE_SHARE_READ: tudo bem se outros programas abrirem o arquivo ao mesmo tempo que nós, mas SOMENTE se eles quiserem ler também, não queremos que eles gravem no arquivo enquanto o estamos lendo.
	// OPEN_EXISTING: apenas abrir o arquivo se ele já existir, não criá-lo e não substituí-lo.
	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	
	// Verificar se o identificador do arquivo é válido.
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;

		// Verificar o tamanho do arquivo para saber quanta memória precisa alocar para ler tudo.
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF)
		{
			// Alocamos a memória, verificamos se a alocação foi bem-sucedida
			LPSTR pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL)
			{
				// Não o usamos, exceto como um parâmetro em ReadFile(). Este parâmetro DEVE ser fornecido, ou a chamada falhará sem ele.
				DWORD dwRead;

				// Carregar o conteúdo do arquivo em nosso buffer de memória.
				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					// As funções do arquivo API não têm nenhum conceito de arquivos de texto, portanto, não farão coisas como ler uma única linha de texto ou adicionar terminadores NULL ao final de nossas strings. É por isso que alocamos um byte extra e, depois de ler o arquivo, adicionamos NULL nós mesmos para que possamos então passar o buffer de memória como uma string para SetWindowText().
					pszFileText[dwFileSize] = 0; // Add null terminator
					if(SetWindowText(hEdit, pszFileText))
						// Se foi bem-sucedido, definimos a variável de sucesso para TRUE
						bSuccess = TRUE; // Funciona!
				}
				// Liberando o buffer de memória.
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

	// Especificamos que queremos acesso de Leitura, que o arquivo deve ser sempre criado novo (e se existir será apagado ao ser aberto) e que se não existir, será criado com o atributos de arquivo normais.
	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		// Obtemos o comprimento do buffer de memória necessário do controle de edição, uma vez que esta é a fonte dos dados.
		DWORD dwTextLength = GetWindowTextLength(hEdit);

		// Não precisa se preocupar se não houver texto.
		if(dwTextLength > 0)
		{
			LPSTR pszText;

			// Definir o tamanho do buffer de memória.
			DWORD dwBufferSize = dwTextLength + 1;

			// Alocar a memória
			pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
			if(pszText != NULL)
			{
				// Solicitamos a string do controle de edição usando GetWindowText().
				if(GetWindowText(hEdit, pszText, dwBufferSize))
				{
					DWORD dwWritten;

					// Gravamos no arquivo com WriteFile().
					// Novamente, como com ReadFile() o parâmetro que retorna o quanto foi realmente escrito é necessário, embora não o utilizemos.
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

	// Para inicializar a estrutura como 0. Dessa forma, você não precisa definir explicitamente cada membro que não usa.
	ZeroMemory(&ofn, sizeof(ofn));

	// Especifica o comprimento, em bytes, da estrutura. Alternativa para versões anteriores do windows: OPENFILENAME_SIZE_VERSION_400
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	// Filtros para os tipos de arquivos que queremos utilizar
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	// Pontos para o buffer que alocamos para armazenar o nome do arquivo, uma vez que os nomes dos arquivos não podem ser maiores do que MAX_PATH este é o valor que escolhi para o tamanho do buffer.
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	// Os sinalizadores indicam que a caixa de diálogo só deve permitir ao usuário inserir nomes de arquivos já existentes (já que queremos abri-los, não criá-los) e ocultar a opção de abrir o arquivo no modo somente leitura, que não iremos oferecer suporte.
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	// Extensão padrão, então se o usuário digitar "nome_arquivo" e o arquivo não for encontrado, ele tentará abrir "nome_arquivo.txt" antes de finalmente desistir.
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

	// Para inicializar a estrutura como 0. Dessa forma, você não precisa definir explicitamente cada membro que não usa.
	ZeroMemory(&ofn, sizeof(ofn));

	// Especifica o comprimento, em bytes, da estrutura. Alternativa para versões anteriores do windows: OPENFILENAME_SIZE_VERSION_400
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "txt";
	// Nesse caso, não queremos mais exigir que o arquivo exista, mas queremos que o diretório exista, pois não vamos tentar criá-lo primeiro. Também perguntaremos ao usuário se ele seleciona um arquivo existente para certificar-se de que deseja sobrescrevê-lo.
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
