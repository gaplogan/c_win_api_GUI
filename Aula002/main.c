#include <windows.h>

// A vari�vel a seguir armazena o nome de nossa classe de janela, n�s a usaremos em breve para registrar nossa classe de janela com o sistema.
const char *g_szClassName = "myWindowClass";

// Etapa 4: o procedimento da janela
// Se o loop de mensagem for o cora��o do programa, o procedimento da janela � o c�rebro. � aqui que todas as mensagens enviadas para a nossa janela s�o processadas.

// O procedimento de janela � chamado para cada mensagem, o par�metro HWND � o identificador de sua janela, aquele ao qual a mensagem se aplica. Isso � importante porque voc� pode ter duas ou mais janelas da mesma classe e elas usar�o o mesmo procedimento de janela (WndProc()). A diferen�a � que o par�metro hwnd ser� diferente dependendo de qual janela estiver. Por exemplo, quando recebemos a mensagem WM_CLOSE, destru�mos a janela. Como usamos o identificador de janela que recebemos como primeiro par�metro, nenhuma outra janela ser� afetada, apenas aquela para a qual a mensagem se destina.

// WM_CLOSE � enviado quando o usu�rio pressiona o bot�o Fechar [x] ou digita Alt-F4. Isso far� com que a janela seja destru�da por padr�o, mas eu gosto de lidar com isso explicitamente, j� que este � o local perfeito para fazer verifica��es de limpeza ou pedir ao usu�rio para salvar arquivos etc. antes de sair do programa.

// Quando chamamos, DestroyWindow() o sistema envia a mensagem WM_DESTROY para a janela que est� sendo destru�da, neste caso � a nossa janela, e ent�o destr�i todas as janelas filhas restantes antes de finalmente remover nossa janela do sistema. Como esta � a �nica janela em nosso programa, terminamos e queremos que o programa saia, ent�o a chamamos PostQuitMessage(). Isso envia a mensagem WM_QUIT para o loop de mensagem. N�s nunca recebemos esta mensagem, porque faz com que GetMessage() retorne FALSE, e como voc� vai ver no nosso c�digo mensagem de loop, quando isso acontece n�s paramos o processamento de mensagens e retornamos o c�digo de resultado final, a wParam de WM_QUIT que passa a ser o valor que passou para PostQuitMessage(). O valor de retorno s� � realmente �til se o seu programa foi projetado para ser chamado por outro programa e voc� deseja retornar um valor espec�fico.
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
	// Uma classe de janela armazena informa��es sobre um tipo de janela, incluindo seu procedimento de janela que controla a janela, os �cones pequenos e grandes da janela e a cor de fundo. Dessa forma, voc� pode registrar uma classe uma vez e criar quantas janelas quiser a partir dela, sem ter que especificar todos esses atributos repetidamente. A maioria dos atributos definidos na classe de janela podem ser alterados por janela, se desejado.

	// Este � o c�digo que usamos no m�todo WinMain() para registrar nossa classe de janela. Preenchemos os membros de uma estrutura WNDCLASSEX e chamamos RegisterClassEx().

	// Os membros da estrutura afetam a classe da janela da seguinte maneira:
	// O tamanho da estrutura.
	wc.cbSize = sizeof(WNDCLASSEX);
	//Estilos de classe ( CS_*), n�o deve ser confundido com Estilos de janela ( WS_*) Isso geralmente pode ser definido como 0.
	wc.style = 0;
	// Ponteiro para o procedimento de janela para esta classe de janela.
	wc.lpfnWndProc = WndProc;
	// Quantidade de dados extras alocados para esta classe na mem�ria. Normalmente 0.
	wc.cbClsExtra = 0;
	// Quantidade de dados extras alocados na mem�ria por janela desse tipo. Normalmente 0.
	wc.cbWndExtra = 0;
	// Identificador para a inst�ncia do aplicativo (que obtivemos no primeiro par�metro de WinMain()).
	wc.hInstance = hInstance;
// 	�cone grande (geralmente 32x32) mostrado quando o usu�rio pressiona Alt + Tab.
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// Cursor que ser� exibido em nossa janela.
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// Pincel de fundo para definir a cor da nossa janela.
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	// Nome de um recurso de menu a ser usado para as janelas com esta classe.
	wc.lpszMenuName = NULL;
	// Nome com o qual identificar a classe.
	wc.lpszClassName = g_szClassName;
	// �cone pequeno (geralmente 16x16) para mostrar na barra de tarefas e no canto superior esquerdo da janela.
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Em seguida, chamamos RegisterClassEx() e verificamos se h� falha, se falhar, exibimos uma mensagem que diz isso e abortamos o programa retornando da fun��o WinMain().
	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Etapa 2: Criando a janela
	// Assim que a classe estiver registrada, podemos criar uma janela com ela. Voc� deve procurar os par�metros para CreateWindowEx()(como voc� sempre deve fazer ao usar uma nova chamada de API), mas vou explic�-los resumidamente aqui.
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE, // Estilo de janelas estendidas
		g_szClassName, // Nome da classe (g_szClassName), que informa ao sistema que tipo de janela criar.
		"O t�tulo da minha janela", // Texto exibido na barra de t�tulo
		WS_OVERLAPPEDWINDOW, // Estilo da janela
		CW_USEDEFAULT, // Coordenada X (CW_USEDEFAULT para deixar o OS decidir)
		CW_USEDEFAULT, // Coordenada Y (CW_USEDEFAULT para deixar o OS decidir)
		600, // Largura em pixels (CW_USEDEFAULT para deixar o OS decidir)
		400, // Altura em pixels (CW_USEDEFAULT para deixar o OS decidir)
		NULL, // Identificador da janela pai, NULL janela principal
		NULL, // Identificador de menu, NULL sem menu
		hInstance, // Identificador de inst�ncia do aplicativo
		NULL // Ponteiro para os dados de cria��o da janela
		);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Falha na cria��o da janela!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
    
	// Depois de criar a janela e verificar se temos um identificador v�lido, mostramos a janela, usando o �ltimo par�metro em WinMain() e, em seguida, atualizamos para garantir que ela foi redesenhada corretamente na tela.
	// O par�metro nCmdShow � opcional, voc� pode simplesmente passar SW_SHOWNORMAL o tempo todo e pronto. No entanto, usar o par�metro passado para WinMain() permite a quem est� executando o programa especificar se deseja ou n�o que sua janela comece vis�vel, maximizada, minimizada, etc ... Voc� encontrar� op��es para estes nas propriedades de atalhos do Windows e neste par�metro � como a escolha � realizada.
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Etapa 3: O loop de mensagens
	// Este � o cora��o de todo o programa, praticamente tudo o que o seu programa faz passa por esse ponto de controle.
	// GetMessage()obt�m uma mensagem da fila de mensagens do seu aplicativo. Sempre que o usu�rio move o mouse, digita no teclado, clica no menu da janela ou faz uma s�rie de outras coisas, as mensagens s�o geradas pelo sistema e inseridas na fila de mensagens do programa. Ao ligar, GetMessage()voc� est� solicitando que a pr�xima mensagem dispon�vel seja removida da fila e devolvida a voc� para processamento. Se n�o houver mensagem, GetMessage() Blocks . Se voc� n�o estiver familiarizado com o termo, significa que ele espera at� que haja uma mensagem e, em seguida, a retorna para voc�.
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		// TranslateMessage() faz algum processamento adicional em eventos de teclado, como gerar mensagens WM_CHAR para acompanhar as mensagens WM_KEYDOWN.
		TranslateMessage(&Msg);

		// Finalmente DispatchMessage() envia a mensagem para a janela para a qual a mensagem foi enviada. Esta pode ser nossa janela principal ou pode ser outra ou um controle e, em alguns casos, uma janela que foi criada nos bastidores pelo sistema ou outro programa. Isso n�o � algo com que voc� precise se preocupar, porque tudo o que nos preocupa � que recebamos a mensagem e a enviemos, o sistema cuida do resto para garantir que ela chegue na janela correta.
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
