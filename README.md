# c_win_api_GUI
Exemplos mostrando como criar aplicações de ambiente gráfico em C utilizando a api do windows.

-lgdi32: subsistema gr�fico
-luser32: subsistema do usu�rio
-lkernel32: subsistema do kernel
-lcomctl32: vers�es dll de controle comuns
-lm: bibliotecas matem�ticas
-mwindows: aplicativo GUI independente (console gratuito)

windres *.rc *.o // Para criar o arquivo .o a partir do .rc