#include <windows.h>

const char *g_szClassName = "myWindowClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
        case WM_LBUTTONDOWN: // Clique com o bot�o esquerdo
        {
            // Ponteiro para um buffer que recebe o caminho e o nome do arquivo do m�dulo especificado
            // MAX_PATH � uma macro �til inclu�da atrav�s de <windows.h> que � definida para o comprimento m�ximo de um buffer necess�rio para armazenar um nome de arquivo no Win32. Tamb�m passamos MAX_PATH para GetModuleFileName() para que saiba o tamanho do buffer.
            char szFileName [MAX_PATH];

            // Obter a instancia do modulo executavel, nosso programa
            // NULL nos retornar� um identificador para o arquivo usado para criar o processo de chamada
            HINSTANCE hIntance = GetModuleHandle(NULL);

            // Depois de GetModuleFileName() ser chamado, o buffer szFileName ser� preenchido com uma string terminada em null contendo o nome do nosso arquivo .exe. Passamos esse valor para MessageBox() como uma maneira f�cil de exibi-lo ao usu�rio.
            GetModuleFileName(hIntance, szFileName, MAX_PATH);
            MessageBox(hwnd, szFileName, "Este programa �:", MB_OK | MB_ICONINFORMATION);
        }        
        break;
        case WM_RBUTTONDOWN:
        {
            MessageBox(hwnd, "Clicou com o bot�o direito", "Clique do Mouse", MB_OK | MB_ICONWARNING);
            PostMessage(hwnd, WM_CLOSE, 0, 0); // Colocar na fila a mensagem para fechar a janela.
        }
        break;
        case WM_MBUTTONDOWN:
        {
            MessageBox(hwnd, "Clicou com o bot�o do meio", "Clique do Mouse", MB_OK | MB_ICONHAND);
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
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, g_szClassName, "O t�tulo da minha janela", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Falha na cria��o da janela!", "Error!",
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

#pragma region Descri��o
/*
O que � uma mensagem?
Uma mensagem � um valor inteiro. Se voc� procurar em seus arquivos de cabe�alho (o que � uma pr�tica boa e comum ao investigar o funcionamento de APIs), poder� encontrar coisas como:
#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111

#define WM_LBUTTONDOWN 0x0201
...e assim por diante. As mensagens s�o usadas para comunicar praticamente tudo no Windows, pelo menos nos n�veis b�sicos. Se voc� quiser que uma janela ou controle (que � apenas uma janela especializada) fa�a algo, envie-lhe uma mensagem. Se outra janela quiser que voc� fa�a algo, ela lhe enviar� uma mensagem. Se ocorrer um evento como o usu�rio digitar no teclado, mover o mouse, clicar em um bot�o, as mensagens s�o enviadas pelo sistema para as janelas afetadas. Se voc� for uma dessas janelas, voc� lida com a mensagem e age de acordo.
Cada mensagem do Windows pode ter at� dois par�metros wParame lParam. Originalmente wParamera de 16 bits e lParamera de 32 bits, mas no Win32 ambos s�o de 32 bits. Nem toda mensagem usa esses par�metros, e cada mensagem os usa de maneira diferente. Por exemplo, a WM_CLOSEmensagem n�o usa nenhum dos dois e voc� deve ignorar os dois. A WM_COMMANDmensagem usa ambos, wParamcont�m dois valores, HIWORD(wParam)� a mensagem de notifica��o (se aplic�vel) e LOWORD(wParam)� o controle ou id do menu que enviou a mensagem. lParam � o HWND(identificador da janela) para o controle que enviou a mensagem ou NULLse as mensagens n�o s�o de um controle.

HIWORD()e LOWORD()s�o macros definidas por janelas que separam os dois bytes altos (palavra alta) de um valor de 32 bits ( ) e a palavra baixa ( ), respectivamente. No Win32, a � um valor de 16 bits, tornando (ou Double Word) um valor de 32 bits. 0xFFFF00000x0000FFFFWORDDWORD

Para enviar uma mensagem, voc� pode usar PostMessage()ou SendMessage(). PostMessage()coloca a mensagem na fila de mensagens e retorna imediatamente. Isso significa que, uma vez que a chamada para PostMessage()� feita, a mensagem pode ou n�o ter sido processada ainda. SendMessage()envia a mensagem diretamente para a janela e n�o retorna at� que a janela tenha conclu�do o processamento. Se quis�ssemos fechar uma janela, poder�amos enviar-lhe uma WM_CLOSEmensagem como esta, PostMessage(hwnd, WM_CLOSE, 0, 0); que teria o mesmo efeito que clicar no [x]bot�o no topo da janela. Observe que wParame lParams�o ambos 0. Isso ocorre porque, como mencionado, eles n�o s�o usados ??para WM_CLOSE.

Di�logos
Depois de come�ar a usar as caixas de di�logo, voc� precisar� enviar mensagens aos controles para se comunicar com eles. Voc� pode fazer isso usando GetDlgItem()primeiro para obter o identificador para o controle usando o ID e, em seguida SendMessage(), usar , OU voc� pode usar o SendDlgItemMessage()que combina as etapas. Voc� atribui a ele um identificador de janela e um ID filho e ele obter� o identificador filho e enviar� a mensagem. SendDlgItemMessage()e APIs semelhantes GetDlgItemText()funcionar�o em todas as janelas, n�o apenas nas caixas de di�logo.

O que � a fila de mensagens
Digamos que voc� estava ocupado manipulando a WM_PAINTmensagem e de repente o usu�rio digita um monte de coisas no teclado. O que deve acontecer? Voc� deve ser interrompido em seu desenho para manusear as chaves ou as chaves devem ser descartadas? Errado! Obviamente, nenhuma dessas op��es � razo�vel, por isso temos a fila de mensagens, quando as mensagens s�o postadas, elas s�o adicionadas � fila de mensagens e quando voc� as manipula, elas s�o removidas. Isso garante que voc� n�o vai perder mensagens, se estiver lidando com uma, as outras ficar�o na fila at� que voc� as receba.

O que � um loop de mensagem
while (GetMessage (& Msg, NULL, 0, 0)> 0)
{
    TranslateMessage (& Msg);
    DispatchMessage (& Msg);
}
As chamadas de loop de mensagem GetMessage(), que procuram em sua fila de mensagens. Se a fila de mensagens estiver vazia, seu programa basicamente para e espera por uma (ele bloqueia ).
Quando ocorre um evento que faz com que uma mensagem seja adicionada � fila (por exemplo o sistema registra um clique do mouse) GetMessages()retorna um valor positivo indicando que h� uma mensagem a ser processada e que preencheu os membros da MSGestrutura que passamos . Ele retornar� 0se acertar WM_QUITe um valor negativo se ocorrer um erro.
Pegamos a mensagem (na Msgvari�vel) e a passamos para TranslateMessage(), isso faz um pouco de processamento adicional, traduzindo mensagens-chave virtuais em mensagens de caracteres. Esta etapa � opcional, mas certas coisas n�o funcionar�o se n�o estiver l�.
Feito isso, passamos a mensagem para DispatchMessage(). O que DispatchMessage()faz � pegar a mensagem, verificar a qual janela ela se destina e, em seguida, consultar o Procedimento de janela da janela. Em seguida, ele chama esse procedimento, enviando como par�metros o identificador da janela, a mensagem wParame lParam.
Em seu procedimento de janela voc� verifica a mensagem e seus par�metros, e faz o que quiser com eles! Se voc� n�o est� lidando com a mensagem espec�fica, quase sempre chama o DefWindowProc() que executar� as a��es padr�o para voc� (o que geralmente significa que n�o faz nada).
Depois de terminar de processar a mensagem, o procedimento do Windows retorna, DispatchMessage() retorna e voltamos ao in�cio do loop.
Este � um conceito muito importante para programas do Windows. Seu procedimento de janela n�o � magicamente chamado pelo sistema; na verdade, voc� o chama indiretamente, chamando DispatchMessage(). Se voc� quiser, pode usar GetWindowLong()o identificador da janela a que a mensagem se destina, para consultar o procedimento da janela e cham�-lo diretamente!

while (GetMessage (& Msg, NULL, 0, 0)> 0)
{
    WNDPROC fWndProc = (WNDPROC) GetWindowLong (Msg.hwnd, GWL_WNDPROC);
    fWndProc (Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
}
Eu tentei isso com o c�digo de exemplo anterior e funciona, no entanto, existem v�rios problemas, como tradu��o Unicode / ANSI, chamada de retornos de chamada de temporizador e assim por diante, que esse m�todo n�o levar� em conta e muito provavelmente quebrar� todos os aplicativos, exceto os triviais. Ent�o fa�a para experimentar, mas n�o fa�a em c�digo real :)

Observe que usamos GetWindowLong()para recuperar o procedimento de janela associado � janela. Por que n�o ligamos WndProc()diretamente para o nosso ? Bem, nosso loop de mensagem � respons�vel por TODAS as janelas em nosso programa, isso inclui coisas como bot�es e caixas de listagem que t�m seus pr�prios procedimentos de janela, ent�o precisamos ter certeza de que chamamos o procedimento correto para a janela. Como mais de uma janela pode usar o mesmo procedimento de janela, o primeiro par�metro (o identificador para a janela) � usado para informar ao procedimento de janela a qual janela a mensagem se destina.

Como voc� pode ver, seu aplicativo passa a maior parte do tempo girando e girando neste loop de mensagens, onde voc� envia mensagens alegremente para as janelas felizes que as processar�o. Mas o que voc� faz quando deseja encerrar o programa? Como estamos usando um while()loop, se GetMessage()retorn�ssemos FALSE(aka 0), o loop terminaria e chegar�amos ao fim de nosso WinMain(), saindo do programa. Isso � exatamente o que PostQuitMessage()realiza. Ele coloca uma WM_QUIT mensagem na fila e, em vez de retornar um valor positivo, GetMessage()preenche a estrutura Msg e retorna 0. Neste ponto, o wParammembro de Msgcont�m o valor que voc� passou para PostQuitMessage() e voc� pode ignor�-lo ou retorn�-lo deWinMain() que ser� usado como c�digo de sa�da quando o processo terminar.

IMPORTANTE: GetMessage() retornar� -1se encontrar um erro. Certifique-se de lembrar disso, ou voc� ser� descoberto em algum momento ... mesmo que GetMessage() seja definido como retornando a BOOL, ele pode retornar valores diferentes de TRUEou FALSE, uma vez que BOOL� definido como UINT( unsigned int). A seguir est�o exemplos de c�digo que podem parecer funcionar, mas n�o processam as condi��es certian corretamente:

    while (GetMessage (& Msg, NULL, 0, 0))
    while (GetMessage (& Msg, NULL, 0, 0)! = 0)
    while (GetMessage (& Msg, NULL, 0, 0) == TRUE)
Os itens acima est�o todos errados! Pode ser interessante notar que eu costumava usar a primeira delas ao longo do tutorial, j� que, como acabei de mencionar, ela funciona bem, desde que GetMessage()nunca falhe, o que quando seu c�digo est� correto n�o funciona. No entanto, deixei de levar em considera��o que se voc� est� lendo isso, seu c�digo provavelmente n�o estar� correto na maioria das vezes e GetMessage()falhar� em algum ponto :) Eu j� passei e corrigi isso, mas me perdoe se Eu perdi alguns pontos.
    while (GetMessage (& Msg, NULL, 0, 0)> 0)
Este, ou c�digo que tem o mesmo efeito, deve sempre ser usado.
Espero que agora voc� tenha um melhor entendimento do loop de mensagens do Windows, se n�o, n�o tenha medo, as coisas far�o mais sentido depois de us�-los por um tempo.
*/
#pragma endregion