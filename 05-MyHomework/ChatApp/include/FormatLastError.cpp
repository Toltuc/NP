// Собственная реализация FormatLastError() для ДЗ "вместо кода ошибки программа
// должна выводить сообщение об ошибке" (см. FormatMessage / Windows Sockets Error Codes).
#include "FormatLastError.h"

CHAR* FormatLastError(DWORD dwError, CHAR szBuffer[])
{
    ZeroMemory(szBuffer, 256);

    DWORD written = FormatMessageA
    (
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        szBuffer,
        255,
        NULL
    );

    if (written == 0)
    {
        wsprintfA(szBuffer, "Unknown error code: %lu", dwError);
    }

    return szBuffer;
}
