#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MessageBox(NULL, "Ola Mundo !!!", "GUI com WinAPI", MB_OK);
    return 0;
}

// HINSTANCE hInstance : Lidar com o módulo executável dos programas (o arquivo .exe na memória)
// HINSTANCE hPrevInstance : Sempre NULL para programas Win32.
// LPSTR lpCmdLine : Os argumentos da linha de comando como uma única string. NÃO incluindo o nome do programa.
// int nCmdShow : Um valor inteiro que pode ser passado para ShowWindow(). Chegaremos a isso mais tarde.