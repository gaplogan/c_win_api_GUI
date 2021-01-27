#include <windows.h>

const char *g_szClassName = "myWindowClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
        case WM_LBUTTONDOWN: // Clique com o botão esquerdo
        {
            // Ponteiro para um buffer que recebe o caminho e o nome do arquivo do módulo especificado
            // MAX_PATH é uma macro útil incluída através de <windows.h> que é definida para o comprimento máximo de um buffer necessário para armazenar um nome de arquivo no Win32. Também passamos MAX_PATH para GetModuleFileName() para que saiba o tamanho do buffer.
            char szFileName [MAX_PATH];

            // Obter a instancia do modulo executavel, nosso programa
            // NULL nos retornará um identificador para o arquivo usado para criar o processo de chamada
            HINSTANCE hIntance = GetModuleHandle(NULL);

            // Depois de GetModuleFileName() ser chamado, o buffer szFileName será preenchido com uma string terminada em null contendo o nome do nosso arquivo .exe. Passamos esse valor para MessageBox() como uma maneira fácil de exibi-lo ao usuário.
            GetModuleFileName(hIntance, szFileName, MAX_PATH);
            MessageBox(hwnd, szFileName, "Este programa é:", MB_OK | MB_ICONINFORMATION);
        }        
        break;
        case WM_RBUTTONDOWN:
        {
            MessageBox(hwnd, "Clicou com o botão direito", "Clique do Mouse", MB_OK | MB_ICONWARNING);
            PostMessage(hwnd, WM_CLOSE, 0, 0); // Colocar na fila a mensagem para fechar a janela.
        }
        break;
        case WM_MBUTTONDOWN:
        {
            MessageBox(hwnd, "Clicou com o botão do meio", "Clique do Mouse", MB_OK | MB_ICONHAND);
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

#pragma region Descrição
/*
O que é uma mensagem?
Uma mensagem é um valor inteiro. Se você procurar em seus arquivos de cabeçalho (o que é uma prática boa e comum ao investigar o funcionamento de APIs), poderá encontrar coisas como:
#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111

#define WM_LBUTTONDOWN 0x0201
...e assim por diante. As mensagens são usadas para comunicar praticamente tudo no Windows, pelo menos nos níveis básicos. Se você quiser que uma janela ou controle (que é apenas uma janela especializada) faça algo, envie-lhe uma mensagem. Se outra janela quiser que você faça algo, ela lhe enviará uma mensagem. Se ocorrer um evento como o usuário digitar no teclado, mover o mouse, clicar em um botão, as mensagens são enviadas pelo sistema para as janelas afetadas. Se você for uma dessas janelas, você lida com a mensagem e age de acordo.
Cada mensagem do Windows pode ter até dois parâmetros wParame lParam. Originalmente wParamera de 16 bits e lParamera de 32 bits, mas no Win32 ambos são de 32 bits. Nem toda mensagem usa esses parâmetros, e cada mensagem os usa de maneira diferente. Por exemplo, a WM_CLOSEmensagem não usa nenhum dos dois e você deve ignorar os dois. A WM_COMMANDmensagem usa ambos, wParamcontém dois valores, HIWORD(wParam)é a mensagem de notificação (se aplicável) e LOWORD(wParam)é o controle ou id do menu que enviou a mensagem. lParam é o HWND(identificador da janela) para o controle que enviou a mensagem ou NULLse as mensagens não são de um controle.

HIWORD()e LOWORD()são macros definidas por janelas que separam os dois bytes altos (palavra alta) de um valor de 32 bits ( ) e a palavra baixa ( ), respectivamente. No Win32, a é um valor de 16 bits, tornando (ou Double Word) um valor de 32 bits. 0xFFFF00000x0000FFFFWORDDWORD

Para enviar uma mensagem, você pode usar PostMessage()ou SendMessage(). PostMessage()coloca a mensagem na fila de mensagens e retorna imediatamente. Isso significa que, uma vez que a chamada para PostMessage()é feita, a mensagem pode ou não ter sido processada ainda. SendMessage()envia a mensagem diretamente para a janela e não retorna até que a janela tenha concluído o processamento. Se quiséssemos fechar uma janela, poderíamos enviar-lhe uma WM_CLOSEmensagem como esta, PostMessage(hwnd, WM_CLOSE, 0, 0); que teria o mesmo efeito que clicar no [x]botão no topo da janela. Observe que wParame lParamsão ambos 0. Isso ocorre porque, como mencionado, eles não são usados ??para WM_CLOSE.

Diálogos
Depois de começar a usar as caixas de diálogo, você precisará enviar mensagens aos controles para se comunicar com eles. Você pode fazer isso usando GetDlgItem()primeiro para obter o identificador para o controle usando o ID e, em seguida SendMessage(), usar , OU você pode usar o SendDlgItemMessage()que combina as etapas. Você atribui a ele um identificador de janela e um ID filho e ele obterá o identificador filho e enviará a mensagem. SendDlgItemMessage()e APIs semelhantes GetDlgItemText()funcionarão em todas as janelas, não apenas nas caixas de diálogo.

O que é a fila de mensagens
Digamos que você estava ocupado manipulando a WM_PAINTmensagem e de repente o usuário digita um monte de coisas no teclado. O que deve acontecer? Você deve ser interrompido em seu desenho para manusear as chaves ou as chaves devem ser descartadas? Errado! Obviamente, nenhuma dessas opções é razoável, por isso temos a fila de mensagens, quando as mensagens são postadas, elas são adicionadas à fila de mensagens e quando você as manipula, elas são removidas. Isso garante que você não vai perder mensagens, se estiver lidando com uma, as outras ficarão na fila até que você as receba.

O que é um loop de mensagem
while (GetMessage (& Msg, NULL, 0, 0)> 0)
{
    TranslateMessage (& Msg);
    DispatchMessage (& Msg);
}
As chamadas de loop de mensagem GetMessage(), que procuram em sua fila de mensagens. Se a fila de mensagens estiver vazia, seu programa basicamente para e espera por uma (ele bloqueia ).
Quando ocorre um evento que faz com que uma mensagem seja adicionada à fila (por exemplo o sistema registra um clique do mouse) GetMessages()retorna um valor positivo indicando que há uma mensagem a ser processada e que preencheu os membros da MSGestrutura que passamos . Ele retornará 0se acertar WM_QUITe um valor negativo se ocorrer um erro.
Pegamos a mensagem (na Msgvariável) e a passamos para TranslateMessage(), isso faz um pouco de processamento adicional, traduzindo mensagens-chave virtuais em mensagens de caracteres. Esta etapa é opcional, mas certas coisas não funcionarão se não estiver lá.
Feito isso, passamos a mensagem para DispatchMessage(). O que DispatchMessage()faz é pegar a mensagem, verificar a qual janela ela se destina e, em seguida, consultar o Procedimento de janela da janela. Em seguida, ele chama esse procedimento, enviando como parâmetros o identificador da janela, a mensagem wParame lParam.
Em seu procedimento de janela você verifica a mensagem e seus parâmetros, e faz o que quiser com eles! Se você não está lidando com a mensagem específica, quase sempre chama o DefWindowProc() que executará as ações padrão para você (o que geralmente significa que não faz nada).
Depois de terminar de processar a mensagem, o procedimento do Windows retorna, DispatchMessage() retorna e voltamos ao início do loop.
Este é um conceito muito importante para programas do Windows. Seu procedimento de janela não é magicamente chamado pelo sistema; na verdade, você o chama indiretamente, chamando DispatchMessage(). Se você quiser, pode usar GetWindowLong()o identificador da janela a que a mensagem se destina, para consultar o procedimento da janela e chamá-lo diretamente!

while (GetMessage (& Msg, NULL, 0, 0)> 0)
{
    WNDPROC fWndProc = (WNDPROC) GetWindowLong (Msg.hwnd, GWL_WNDPROC);
    fWndProc (Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
}
Eu tentei isso com o código de exemplo anterior e funciona, no entanto, existem vários problemas, como tradução Unicode / ANSI, chamada de retornos de chamada de temporizador e assim por diante, que esse método não levará em conta e muito provavelmente quebrará todos os aplicativos, exceto os triviais. Então faça para experimentar, mas não faça em código real :)

Observe que usamos GetWindowLong()para recuperar o procedimento de janela associado à janela. Por que não ligamos WndProc()diretamente para o nosso ? Bem, nosso loop de mensagem é responsável por TODAS as janelas em nosso programa, isso inclui coisas como botões e caixas de listagem que têm seus próprios procedimentos de janela, então precisamos ter certeza de que chamamos o procedimento correto para a janela. Como mais de uma janela pode usar o mesmo procedimento de janela, o primeiro parâmetro (o identificador para a janela) é usado para informar ao procedimento de janela a qual janela a mensagem se destina.

Como você pode ver, seu aplicativo passa a maior parte do tempo girando e girando neste loop de mensagens, onde você envia mensagens alegremente para as janelas felizes que as processarão. Mas o que você faz quando deseja encerrar o programa? Como estamos usando um while()loop, se GetMessage()retornássemos FALSE(aka 0), o loop terminaria e chegaríamos ao fim de nosso WinMain(), saindo do programa. Isso é exatamente o que PostQuitMessage()realiza. Ele coloca uma WM_QUIT mensagem na fila e, em vez de retornar um valor positivo, GetMessage()preenche a estrutura Msg e retorna 0. Neste ponto, o wParammembro de Msgcontém o valor que você passou para PostQuitMessage() e você pode ignorá-lo ou retorná-lo deWinMain() que será usado como código de saída quando o processo terminar.

IMPORTANTE: GetMessage() retornará -1se encontrar um erro. Certifique-se de lembrar disso, ou você será descoberto em algum momento ... mesmo que GetMessage() seja definido como retornando a BOOL, ele pode retornar valores diferentes de TRUEou FALSE, uma vez que BOOLé definido como UINT( unsigned int). A seguir estão exemplos de código que podem parecer funcionar, mas não processam as condições certian corretamente:

    while (GetMessage (& Msg, NULL, 0, 0))
    while (GetMessage (& Msg, NULL, 0, 0)! = 0)
    while (GetMessage (& Msg, NULL, 0, 0) == TRUE)
Os itens acima estão todos errados! Pode ser interessante notar que eu costumava usar a primeira delas ao longo do tutorial, já que, como acabei de mencionar, ela funciona bem, desde que GetMessage()nunca falhe, o que quando seu código está correto não funciona. No entanto, deixei de levar em consideração que se você está lendo isso, seu código provavelmente não estará correto na maioria das vezes e GetMessage()falhará em algum ponto :) Eu já passei e corrigi isso, mas me perdoe se Eu perdi alguns pontos.
    while (GetMessage (& Msg, NULL, 0, 0)> 0)
Este, ou código que tem o mesmo efeito, deve sempre ser usado.
Espero que agora você tenha um melhor entendimento do loop de mensagens do Windows, se não, não tenha medo, as coisas farão mais sentido depois de usá-los por um tempo.
*/
#pragma endregion