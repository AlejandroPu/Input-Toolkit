#include <iostream>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

// Declaraci�n de las funciones
void ControlMouse();
void IdentificarMouse();

int main() {
    // Establecer el c�digo de p�gina UTF-8 en la consola
    SetConsoleOutputCP(CP_UTF8);

    // Configurar stdout y stdin para trabajar en modo Unicode
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    int opcion = 0;
    do {
        std::wcout << L"Seleccione una opci�n:" << std::endl;
        std::wcout << L"1. Controlar el mouse" << std::endl;
        std::wcout << L"2. Identificar dispositivos HID" << std::endl;
        std::wcout << L"0. Salir" << std::endl;
        std::wcout << L"Opci�n: ";
        std::wcin >> opcion;

        switch (opcion) {
        case 1:
            ControlMouse();
            break;
        case 2:
            IdentificarMouse();
            break;
        case 0:
            std::wcout << L"Saliendo del programa." << std::endl;
            break;
        default:
            std::wcout << L"Opci�n no v�lida. Intente de nuevo." << std::endl;
        }
    } while (opcion != 0);

    return 0;
}
