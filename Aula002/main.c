#include <windows.h>

// A variável a seguir armazena o nome de nossa classe de janela, nós a usaremos em breve para registrar nossa classe de janela com o sistema.
const char *g_szClassName = "myWindowClass";

// Etapa 4: o procedimento da janela
// Se o loop de mensagem for o coração do programa, o procedimento da janela é o cérebro. É aqui que todas as mensagens enviadas para a nossa janela são processadas.

// O procedimento de janela é chamado para cada mensagem, o parâmetro HWND é o identificador de sua janela, aquele ao qual a mensagem se aplica. Isso é importante porque você pode ter duas ou mais janelas da mesma classe e elas usarão o mesmo procedimento de janela (WndProc()). A diferença é que o parâmetro hwnd será diferente dependendo de qual janela estiver. Por exemplo, quando recebemos a mensagem WM_CLOSE, destruímos a janela. Como usamos o identificador de janela que recebemos como primeiro parâmetro, nenhuma outra janela será afetada, apenas aquela para a qual a mensagem se destina.

// WM_CLOSE é enviado quando o usuário pressiona o botão Fechar [x] ou digita Alt-F4. Isso fará com que a janela seja destruída por padrão, mas eu gosto de lidar com isso explicitamente, já que este é o local perfeito para fazer verificações de limpeza ou pedir ao usuário para salvar arquivos etc. antes de sair do programa.

// Quando chamamos, DestroyWindow() o sistema envia a mensagem WM_DESTROY para a janela que está sendo destruída, neste caso é a nossa janela, e então destrói todas as janelas filhas restantes antes de finalmente remover nossa janela do sistema. Como esta é a única janela em nosso programa, terminamos e queremos que o programa saia, então a chamamos PostQuitMessage(). Isso envia a mensagem WM_QUIT para o loop de mensagem. Nós nunca recebemos esta mensagem, porque faz com que GetMessage() retorne FALSE, e como você vai ver no nosso código mensagem de loop, quando isso acontece nós paramos o processamento de mensagens e retornamos o código de resultado final, a wParam de WM_QUIT que passa a ser o valor que passou para PostQuitMessage(). O valor de retorno só é realmente útil se o seu programa foi projetado para ser chamado por outro programa e você deseja retornar um valor específico.
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
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
    FreeConsole(); // Esconder a janela do console

	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	// Etapa 1: Registrando a Classe Window
	// Uma classe de janela armazena informações sobre um tipo de janela, incluindo seu procedimento de janela que controla a janela, os ícones pequenos e grandes da janela e a cor de fundo. Dessa forma, você pode registrar uma classe uma vez e criar quantas janelas quiser a partir dela, sem ter que especificar todos esses atributos repetidamente. A maioria dos atributos definidos na classe de janela podem ser alterados por janela, se desejado.

	// Este é o código que usamos no método WinMain() para registrar nossa classe de janela. Preenchemos os membros de uma estrutura WNDCLASSEX e chamamos RegisterClassEx().

	// Os membros da estrutura afetam a classe da janela da seguinte maneira:
	// O tamanho da estrutura.
	wc.cbSize = sizeof(WNDCLASSEX);
	//Estilos de classe ( CS_*), não deve ser confundido com Estilos de janela ( WS_*) Isso geralmente pode ser definido como 0.
	wc.style = 0;
	// Ponteiro para o procedimento de janela para esta classe de janela.
	wc.lpfnWndProc = WndProc;
	// Quantidade de dados extras alocados para esta classe na memória. Normalmente 0.
	wc.cbClsExtra = 0;
	// Quantidade de dados extras alocados na memória por janela desse tipo. Normalmente 0.
	wc.cbWndExtra = 0;
	// Identificador para a instância do aplicativo (que obtivemos no primeiro parâmetro de WinMain()).
	wc.hInstance = hInstance;
// 	Ícone grande (geralmente 32x32) mostrado quando o usuário pressiona Alt + Tab.
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// Cursor que será exibido em nossa janela.
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// Pincel de fundo para definir a cor da nossa janela.
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	// Nome de um recurso de menu a ser usado para as janelas com esta classe.
	wc.lpszMenuName = NULL;
	// Nome com o qual identificar a classe.
	wc.lpszClassName = g_szClassName;
	// Ícone pequeno (geralmente 16x16) para mostrar na barra de tarefas e no canto superior esquerdo da janela.
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Em seguida, chamamos RegisterClassEx() e verificamos se há falha, se falhar, exibimos uma mensagem que diz isso e abortamos o programa retornando da função WinMain().
	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Etapa 2: Criando a janela
	// Assim que a classe estiver registrada, podemos criar uma janela com ela. Você deve procurar os parâmetros para CreateWindowEx()(como você sempre deve fazer ao usar uma nova chamada de API), mas vou explicá-los resumidamente aqui.
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE, // Estilo de janelas estendidas
		g_szClassName, // Nome da classe (g_szClassName), que informa ao sistema que tipo de janela criar.
		"O título da minha janela", // Texto exibido na barra de título
		WS_OVERLAPPEDWINDOW, // Estilo da janela
		CW_USEDEFAULT, // Coordenada X (CW_USEDEFAULT para deixar o OS decidir)
		CW_USEDEFAULT, // Coordenada Y (CW_USEDEFAULT para deixar o OS decidir)
		600, // Largura em pixels (CW_USEDEFAULT para deixar o OS decidir)
		400, // Altura em pixels (CW_USEDEFAULT para deixar o OS decidir)
		NULL, // Identificador da janela pai, NULL janela principal
		NULL, // Identificador de menu, NULL sem menu
		hInstance, // Identificador de instância do aplicativo
		NULL // Ponteiro para os dados de criação da janela
		);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Falha na criação da janela!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
    
	// Depois de criar a janela e verificar se temos um identificador válido, mostramos a janela, usando o último parâmetro em WinMain() e, em seguida, atualizamos para garantir que ela foi redesenhada corretamente na tela.
	// O parâmetro nCmdShow é opcional, você pode simplesmente passar SW_SHOWNORMAL o tempo todo e pronto. No entanto, usar o parâmetro passado para WinMain() permite a quem está executando o programa especificar se deseja ou não que sua janela comece visível, maximizada, minimizada, etc ... Você encontrará opções para estes nas propriedades de atalhos do Windows e neste parâmetro é como a escolha é realizada.
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Etapa 3: O loop de mensagens
	// Este é o coração de todo o programa, praticamente tudo o que o seu programa faz passa por esse ponto de controle.
	// GetMessage()obtém uma mensagem da fila de mensagens do seu aplicativo. Sempre que o usuário move o mouse, digita no teclado, clica no menu da janela ou faz uma série de outras coisas, as mensagens são geradas pelo sistema e inseridas na fila de mensagens do programa. Ao ligar, GetMessage()você está solicitando que a próxima mensagem disponível seja removida da fila e devolvida a você para processamento. Se não houver mensagem, GetMessage() Blocks . Se você não estiver familiarizado com o termo, significa que ele espera até que haja uma mensagem e, em seguida, a retorna para você.
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		// TranslateMessage() faz algum processamento adicional em eventos de teclado, como gerar mensagens WM_CHAR para acompanhar as mensagens WM_KEYDOWN.
		TranslateMessage(&Msg);

		// Finalmente DispatchMessage() envia a mensagem para a janela para a qual a mensagem foi enviada. Esta pode ser nossa janela principal ou pode ser outra ou um controle e, em alguns casos, uma janela que foi criada nos bastidores pelo sistema ou outro programa. Isso não é algo com que você precise se preocupar, porque tudo o que nos preocupa é que recebamos a mensagem e a enviemos, o sistema cuida do resto para garantir que ela chegue na janela correta.
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
