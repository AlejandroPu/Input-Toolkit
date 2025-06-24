// IdentificarMouse.cpp
#include <windows.h>
#include <setupapi.h>
#include <iostream>
#include <hidsdi.h>
#include <cwchar>    // Para manejo de cadenas Unicode
#include <fcntl.h>   // Para _setmode()
#include <io.h>      // Para _setmode()
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")

void ObtenerNombreDispositivo(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData) {
    WCHAR deviceName[256];
    DWORD size = 0;

    if (SetupDiGetDeviceRegistryPropertyW(
        hDevInfo,
        &devInfoData,
        SPDRP_DEVICEDESC,
        NULL,
        (PBYTE)deviceName,
        sizeof(deviceName),
        &size
    )) {
        std::wcout << L"Nombre del dispositivo: " << deviceName << std::endl;
    }
    else {
        std::cerr << "Error al obtener el nombre del dispositivo. Código de error: " << GetLastError() << std::endl;
    }
}

void ObtenerDetallesDispositivo(HDEVINFO hDevInfo, SP_DEVICE_INTERFACE_DATA& deviceInterfaceData) {
    // Obtener el tamaño requerido para los detalles
    DWORD requiredSize = 0;
    SetupDiGetDeviceInterfaceDetail(hDevInfo, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        std::cerr << "Error al obtener el tamaño requerido para los detalles del dispositivo. Código de error: " << GetLastError() << std::endl;
        return;
    }

    // Asignar el búfer para los detalles del dispositivo
    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
    if (deviceDetailData == NULL) {
        std::cerr << "Error al asignar memoria para los detalles del dispositivo." << std::endl;
        return;
    }

    // Establecer cbSize correctamente
    deviceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    // Declarar SP_DEVINFO_DATA
    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    // Obtener los detalles de la interfaz del dispositivo
    if (SetupDiGetDeviceInterfaceDetail(
        hDevInfo,
        &deviceInterfaceData,
        deviceDetailData,
        requiredSize,
        NULL,
        &devInfoData
    )) {
        std::wcout << L"Ruta del dispositivo: " << deviceDetailData->DevicePath << std::endl;

        // Obtener el nombre del dispositivo
        ObtenerNombreDispositivo(hDevInfo, devInfoData);

        // Abrir el dispositivo y obtener los atributos HID
        HANDLE deviceHandle = CreateFile(
            deviceDetailData->DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        if (deviceHandle != INVALID_HANDLE_VALUE) {
            HIDD_ATTRIBUTES attributes;
            attributes.Size = sizeof(HIDD_ATTRIBUTES);

            if (HidD_GetAttributes(deviceHandle, &attributes)) {
                std::wcout << L"Vendor ID: 0x" << std::hex << attributes.VendorID << std::endl;
                std::wcout << L"Product ID: 0x" << std::hex << attributes.ProductID << std::endl;
                std::wcout << L"Número de versión: 0x" << std::hex << attributes.VersionNumber << std::endl;
            }
            else {
                std::cerr << "Error al obtener los atributos HID. Código de error: " << GetLastError() << std::endl;
            }

            CloseHandle(deviceHandle);
        }
        else {
            std::cerr << "Error al abrir el dispositivo. Código de error: " << GetLastError() << std::endl;
        }
    }
    else {
        std::cerr << "Error al obtener los detalles de la interfaz del dispositivo. Código de error: " << GetLastError() << std::endl;
    }

    free(deviceDetailData);
}

// Función para enumerar dispositivos HID
void EnumerarDispositivosUSB() {
    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);

    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        &hidGuid,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
    );

    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "Error al obtener la lista de dispositivos HID." << std::endl;
        return;
    }

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD deviceIndex = 0;
    while (SetupDiEnumDeviceInterfaces(
        hDevInfo,
        NULL,
        &hidGuid,
        deviceIndex,
        &deviceInterfaceData
    )) {
        std::wcout << L"Dispositivo HID encontrado. Índice: " << deviceIndex << std::endl;
        ObtenerDetallesDispositivo(hDevInfo, deviceInterfaceData);
        deviceIndex++;
    }

    if (GetLastError() != ERROR_NO_MORE_ITEMS) {
        std::cerr << "Error al enumerar dispositivos. Código de error: " << GetLastError() << std::endl;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
}

void IdentificarMouse() {
    // Configurar stdout para Unicode
    _setmode(_fileno(stdout), _O_U16TEXT);

    std::wcout << L"Enumerando dispositivos HID..." << std::endl;
    EnumerarDispositivosUSB();
    system("pause");
}
