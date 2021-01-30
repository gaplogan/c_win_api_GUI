#include <windows.h>
#include "resource.h" 

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_INITDIALOG:
			// É aqui que configuramos a caixa de diálogo e inicializamos todos os valores padrão

			SetDlgItemText(hwnd, IDC_TEXT, "Esta é uma string");
			SetDlgItemInt(hwnd, IDC_NUMBER, 3, FALSE);
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_ADD:
				{
					// Quando alguém clica no botão Adicionar, primeiro obtemos o número de pessoas inseridas
					BOOL bSuccess;
					int nTimes = GetDlgItemInt(hwnd, IDC_NUMBER, &bSuccess, FALSE);
					if(bSuccess) 
					{
						// Então nós pegamos a string que eles inseriram. Primeiro precisamos descobrir quão grande é, para que possamos alocar alguma memória

						int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT));
						if(len > 0)
						{
							// Agora nós alocamos e colocamos a string em nosso buffer
							int i;
							char* buf;

							buf = (char*)GlobalAlloc(GPTR, len + 1);
							GetDlgItemText(hwnd, IDC_TEXT, buf, len + 1);

							// Agora, adicionamos a string à caixa de listagem quantas vezes o usuário nos solicitar.
							for(i = 0; i < nTimes; i++)
							{
								int index = SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)buf);

								// Aqui, estamos associando o valor nTimes com o item apenas por diversão, vamos usá-lo para exibir mais tarde.
								// Normalmente, você colocaria alguns dados mais úteis aqui, como um ponteiro.
								SendDlgItemMessage(hwnd, IDC_LIST, LB_SETITEMDATA, (WPARAM)index, (LPARAM)nTimes);
							}

							// Não se esqueça de liberar a memória!
							GlobalFree((HANDLE)buf);
						}
						else 
						{
							MessageBox(hwnd, "Você não inseriu nada!", "Atenção", MB_OK);
						}
					}
					else 
					{
						MessageBox(hwnd, "Não foi possível traduzir esse número :(", "Atenção", MB_OK);
					}

				}
				break;
				case IDC_REMOVE:
				{
					// Quando o usuário clica no botão Remover, primeiro obtemos o número de itens selecionados

					HWND hList = GetDlgItem(hwnd, IDC_LIST);
					int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
					if(count != LB_ERR)
					{
						if(count != 0)
						{
							// E então aloque espaço para armazenar a lista de itens selecionados.

							int i;
							int *buf = (int*)GlobalAlloc(GPTR, sizeof(int) * count);
							SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);
							
							// Agora percorremos a lista e removemos cada item que foi selecionado.

							// WARNING!!!  
							// Nós fazemos um loop para trás, porque se removêssemos os itens de cima para baixo, isso mudaria os índices dos outros itens !!!

							for(i = count - 1; i >= 0; i--)
							{
								SendMessage(hList, LB_DELETESTRING, (WPARAM)buf[i], 0);
							}

							GlobalFree(buf);
						}
						else 
						{
							MessageBox(hwnd, "Nenhum item selecionado.", "Atenção", MB_OK);
						}
					}
					else
					{
						MessageBox(hwnd, "Erro ao contar itens :(", "Atenção", MB_OK);
					}
				}
				break;
				case IDC_CLEAR:
					SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
					SetDlgItemText(hwnd, IDC_TEXT, "");
					SetFocus(GetDlgItem(hwnd, IDC_TEXT));
				break;
				case IDC_LIST:
					switch(HIWORD(wParam))
					{
						case LBN_SELCHANGE:
						{
							// Obtenha o número de itens selecionados.
							HWND hList = GetDlgItem(hwnd, IDC_LIST);
							int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
							if(count != LB_ERR)
							{
								// Queremos continuar apenas se um e apenas um item for selecionado.

								if(count == 1)
								{
									// Como sabemos de antemão que estamos obtendo apenas um índice, não há necessidade de alocar um array.

									int index;
									int err = SendMessage(hList, LB_GETSELITEMS, (WPARAM)1, (LPARAM)&index);
									if(err != LB_ERR)
									{
										// Obtenha os dados que associamos ao item acima (o número de vezes que ele foi adicionado)

										int data = SendMessage(hList, LB_GETITEMDATA, (WPARAM)index, 0);

										SetDlgItemInt(hwnd, IDC_SHOWCOUNT, data, FALSE);
									}
									else 
									{
										MessageBox(hwnd, "Erro ao obter o item selecionado :(", "Atenção", MB_OK);
									}
								}
								else 
								{
									// Nenhum item selecionado, ou mais de um De qualquer maneira, não iremos processar isso.
									SetDlgItemText(hwnd, IDC_SHOWCOUNT, "-");
								}
							}
							else
							{
								MessageBox(hwnd, "Erro ao contar itens :(", "Atenção", MB_OK);
							}
						}
						break;
					}
				break;
			}
		break;
		case WM_CLOSE:
			EndDialog(hwnd, 0);
		break;
		default:
			return FALSE;
	}
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, DlgProc);
}
