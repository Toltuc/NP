#pragma once
#include <Windows.h>

// Возвращает читаемое сообщение об ошибке по коду dwError.
// szBuffer должен вмещать минимум 256 символов.
CHAR* FormatLastError(DWORD dwError, CHAR szBuffer[]);
