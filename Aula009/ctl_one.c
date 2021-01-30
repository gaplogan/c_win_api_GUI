#include <windows.h>
#include "resource.h" 

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_INITDIALOG:
			// � aqui que configuramos a caixa de di�logo e inicializamos todos os valores padr�o

			SetDlgItemText(hwnd, IDC_TEXT, "Esta � uma string");
			SetDlgItemInt(hwnd, IDC_NUMBER, 3, FALSE);
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_ADD:
				{
					// Quando algu�m clica no bot�o Adicionar, primeiro obtemos o n�mero de pessoas inseridas
					BOOL bSuccess;
					int nTimes = GetDlgItemInt(hwnd, IDC_NUMBER, &bSuccess, FALSE);
					if(bSuccess) 
					{
						// Ent�o n�s pegamos a string que eles inseriram. Primeiro precisamos descobrir qu�o grande �, para que possamos alocar alguma mem�ria

						int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT));
						if(len > 0)
						{
							// Agora n�s alocamos e colocamos a string em nosso buffer
							int i;
							char* buf;

							buf = (char*)GlobalAlloc(GPTR, len + 1);
							GetDlgItemText(hwnd, IDC_TEXT, buf, len + 1);

							// Agora, adicionamos a string � caixa de listagem quantas vezes o usu�rio nos solicitar.
							for(i = 0; i < nTimes; i++)
							{
								int index = SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)buf);

								// Aqui, estamos associando o valor nTimes com o item apenas por divers�o, vamos us�-lo para exibir mais tarde.
								// Normalmente, voc� colocaria alguns dados mais �teis aqui, como um ponteiro.
								SendDlgItemMessage(hwnd, IDC_LIST, LB_SETITEMDATA, (WPARAM)index, (LPARAM)nTimes);
							}

							// N�o se esque�a de liberar a mem�ria!
							GlobalFree((HANDLE)buf);
						}
						else 
						{
							MessageBox(hwnd, "Voc� n�o inseriu nada!", "Aten��o", MB_OK);
						}
					}
					else 
					{
						MessageBox(hwnd, "N�o foi poss�vel traduzir esse n�mero :(", "Aten��o", MB_OK);
					}

				}
				break;
				case IDC_REMOVE:
				{
					// Quando o usu�rio clica no bot�o Remover, primeiro obtemos o n�mero de itens selecionados

					HWND hList = GetDlgItem(hwnd, IDC_LIST);
					int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
					if(count != LB_ERR)
					{
						if(count != 0)
						{
							// E ent�o aloque espa�o para armazenar a lista de itens selecionados.

							int i;
							int *buf = (int*)GlobalAlloc(GPTR, sizeof(int) * count);
							SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);
							
							// Agora percorremos a lista e removemos cada item que foi selecionado.

							// WARNING!!!  
							// N�s fazemos um loop para tr�s, porque se remov�ssemos os itens de cima para baixo, isso mudaria os �ndices dos outros itens !!!

							for(i = count - 1; i >= 0; i--)
							{
								SendMessage(hList, LB_DELETESTRING, (WPARAM)buf[i], 0);
							}

							GlobalFree(buf);
						}
						else 
						{
							MessageBox(hwnd, "Nenhum item selecionado.", "Aten��o", MB_OK);
						}
					}
					else
					{
						MessageBox(hwnd, "Erro ao contar itens :(", "Aten��o", MB_OK);
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
							// Obtenha o n�mero de itens selecionados.
							HWND hList = GetDlgItem(hwnd, IDC_LIST);
							int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
							if(count != LB_ERR)
							{
								// Queremos continuar apenas se um e apenas um item for selecionado.

								if(count == 1)
								{
									// Como sabemos de antem�o que estamos obtendo apenas um �ndice, n�o h� necessidade de alocar um array.

									int index;
									int err = SendMessage(hList, LB_GETSELITEMS, (WPARAM)1, (LPARAM)&index);
									if(err != LB_ERR)
									{
										// Obtenha os dados que associamos ao item acima (o n�mero de vezes que ele foi adicionado)

										int data = SendMessage(hList, LB_GETITEMDATA, (WPARAM)index, 0);

										SetDlgItemInt(hwnd, IDC_SHOWCOUNT, data, FALSE);
									}
									else 
									{
										MessageBox(hwnd, "Erro ao obter o item selecionado :(", "Aten��o", MB_OK);
									}
								}
								else 
								{
									// Nenhum item selecionado, ou mais de um De qualquer maneira, n�o iremos processar isso.
									SetDlgItemText(hwnd, IDC_SHOWCOUNT, "-");
								}
							}
							else
							{
								MessageBox(hwnd, "Erro ao contar itens :(", "Aten��o", MB_OK);
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
