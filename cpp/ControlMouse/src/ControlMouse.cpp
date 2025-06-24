// ControlMouse.cpp
#include <windows.h>
#include <iostream>
#include <cwchar>    // Para manejo de cadenas Unicode
#include <fcntl.h>   // Para _setmode()
#include <io.h>      // Para _setmode()

void HacerClickIzquierdo() {
    INPUT Inputs[2] = {};

    // Simular la pulsación del botón izquierdo
    Inputs[0].type = INPUT_MOUSE;
    Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    // Simular la liberación del botón izquierdo
    Inputs[1].type = INPUT_MOUSE;
    Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    // Enviar los eventos al sistema
    SendInput(2, Inputs, sizeof(INPUT));
}

void ControlMouse() {
    // Configurar stdout para Unicode
    _setmode(_fileno(stdout), _O_U16TEXT);

    // Mover el cursor a una posición específica
    int x = 100;
    int y = 20;
    SetCursorPos(x, y);

    // Esperar un segundo
    Sleep(1000);

    // Simular un clic izquierdo
    HacerClickIzquierdo();

    std::wcout << L"Se ha simulado un clic izquierdo en (" << x << L", " << y << L")." << std::endl;

    system("pause");
}
