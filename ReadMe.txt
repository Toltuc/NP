https://github.com/okovtun/P_421_NP.git
https://www.youtube.com/playlist?list=PLeqyOOqxeiINoBHHADmFPLjG50Czro-Oo

MSDN:
https://learn.microsoft.com/en-us/windows/win32/controls/up-down-control-reference
https://learn.microsoft.com/en-us/windows/win32/controls/ip-address-control-reference

TODO:
1. Разобраться с условной компиляцией клиента, Сервера и FormatLastError;

DONE:
1. Запустить Сервер на виртуальной машине;
2. У клиента должен быть конфигурационный файл, в котором будет записан адрес Сервера;
   -> реализовано в 05-MyHomework/ChatApp (Client/Client.ini, Client/main.cpp);

DONE:
1. При отключении клиента, на Сервере должно освобождаться место для подключения нового клиента;  DONE
2. На Сервере должно отображаться количество подключенных клиентов;                       DONE
3. Сообщение, отправленное на Сервер должно пересылаться всем подключенным клиентам;      DONE
   -> реализовано в 05-MyHomework/ChatApp/Server/main.cpp (FindFreeSlot, PrintActiveClientCount, Broadcast);

TODO:
1. Установить VMware Workstation;
2. Создать виртуальную машину и установить на нее ос Windows:
	https://archive.org/details/en_windows_10_enterprise_2016_ltsb_x64

DONE:
1. Создать ветку 'ServerErrorHandling';
2. Применить функцию FormatLastError() на Сервере;
3. Посмотреть код на $1 000 000 000;

DONE:
1. Вместо кода ошибки программа должна выводить сообщение об ошибке:
	https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
	https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-formatmessage
   -> собственная реализация: 05-MyHomework/ChatApp/include/FormatLastError.h/.cpp;

DONE:
1. Залить проект 'NetworkProgramming' на GitHub;
2. Прислать ссылку на репозиторий;
   -> см. 05-MyHomework/ChatApp (собственная версия проекта NetworkProgramming);

DONE:
1. Вычислить количество IP-адресов и количество узлов для заданной пары IP-адрес/Маска;
   -> реализовано в 05-MyHomework/IPCalculator/main.cpp;

DONE:
1. При переходе на поле IP-адреса, после изменения Маски/Префикса,
   ни Маска ни Префикс НЕ должны сбрасываться на значения по умолчанию для класса адреса;
   -> в консольной версии (IPCalculator) IP и Маска/Префикс считываются и хранятся независимо
      друг от друга, поэтому изменение одного поля никогда не затирает другое;

DONE:
1. Системы счисления наизусть: NumericSystems.pdf
2. Теорию наизусть;
3. Изучить побитовые операции;
4. Изучить RAID-массивы:
	https://www.intel.com/content/www/us/en/support/articles/000005867/technologies.html
   -> конспекты: 05-MyHomework/Theory/NumericSystems.md, BitwiseOperations.md, RAID.md;

DONE:
1. Установить Notepad++: https://notepad-plus-plus.org/downloads/;
2. Выучить теорию по стеку протоколов TCP/IP;

DONE:
1. Выучить теорию по модели ISO/OSI и стеку протоколов TCP/IP;
2. Степени числа 2 до 2^16 наизусть;
3. Почитать про протоколы RTP и Quic;
   -> термины и теория: Terms.md;


DONE:
1. Изучить следующие спецификации:
	FastEthernet:
		100Base-T4;
		100Base-TX;
	GigabitEthernet:
		1000Base-T;
		1000Base-LX;
	10-GigabitEthernet:
		10GBase-T;
		10GBase-T1;
		10GBase-SW;
		10GBase-LW;
		10GBase-EW;
		10GBase-LX4;
2. Выяснить даты создания 2.5-GigabitEthernet и 5-GigabitEthernet;
3. Изучить спецификации 2.5-GigabitEthernet и 5-GigabitEthernet;

DONE:
1. Выучить теорию;
2. Подобрать оборудование для построения локальной сети на 120 портов.
   В здании должно быть 6 аудиторий по 10-15 компьютеров и офис,
   в котором может быть установлено до 15 компьютеров.
   Так же в сети должна быть точка доступа для беспроводных клиентов.
   Вычислить общую стоимость всего оборудования;