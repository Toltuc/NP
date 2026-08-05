# P_421_NP — Компьютерные сети

Материалы курса «Компьютерные сети» (по программе преподавателя
[okovtun/P_421_NP](https://github.com/okovtun/P_421_NP)) и выполненные домашние задания.

## Структура репозитория

- **`00-Intro/`** — введение, активное сетевое оборудование, расчёт оборудования (`Equipment.md`).
- **`01-Cables/`** — кабели, разъёмы, спецификации Ethernet.
- **`02-NetworkModels/`** — модель OSI, декомпозиция, PDU, инкапсуляция.
- **`03-TCP IP/`** — стек протоколов TCP/IP, уровни, протоколы.
- **`04-Addressing/`** — MAC/IP-адресация, классы адресов, маски, деление сетей.
- **`05-MyHomework/`** — самостоятельно выполненные ДЗ (см. ниже).
- **`IPcalc/`, `NetworkProgramming/`** — референсные проекты (материалы преподавателя).
- **`Terms.md`** — конспект терминов курса.
- **`ReadMe.txt`** — журнал заданий (TODO/DONE), как в репозитории преподавателя.

## Выполненные ДЗ (`05-MyHomework/`)

| Задание | Файл | Комментарий |
|---|---|---|
| Кол-во IP-адресов/узлов по IP+Маске, независимые поля IP/Маски | `IPCalculator/main.cpp` | Консольная кроссплатформенная версия (без Win32 GUI), своя реализация |
| Конфиг-файл клиента с адресом сервера | `ChatApp/Client/Client.ini`, `ChatApp/Client/main.cpp` | |
| Освобождение слота при отключении клиента, счётчик подключений, широковещательная рассылка | `ChatApp/Server/main.cpp` | |
| Сообщение об ошибке вместо кода ошибки | `ChatApp/include/FormatLastError.h/.cpp` | Своя реализация на `FormatMessageA` |
| Системы счисления | `Theory/NumericSystems.md` | |
| Побитовые операции и сдвиги | `Theory/BitwiseOperations.md` | |
| RAID-массивы | `Theory/RAID.md` | |

Подробный список заданий с пометками DONE/TODO — в `ReadMe.txt`.

## Сборка ДЗ

**IPCalculator** (кроссплатформенный, C++17):
```
g++ -std=c++17 -O2 -o ipcalc 05-MyHomework/IPCalculator/main.cpp
./ipcalc 192.168.1.10 255.255.255.0
```

**ChatApp** (Windows, Winsock, Visual Studio):
- Собрать `Server/main.cpp` и `include/FormatLastError.cpp` в один проект, подключить `Ws2_32.lib`.
- Собрать `Client/main.cpp` и `include/FormatLastError.cpp` в отдельный проект, `Ws2_32.lib`.
- Запустить `Server.exe`, затем `Client.exe` (рядом с ним должен лежать `Client.ini`).
