// IPCalculator - самостоятельное решение ДЗ:
// "Вычислить количество IP-адресов и количество узлов для заданной пары IP-адрес/Маска"
//
// В отличие от версии учителя (Win32 GUI c IP Address Control / Up-Down Control),
// это консольное кроссплатформенное решение на чистом C++ (без зависимостей от Windows.h),
// чтобы его можно было собрать и проверить на любой машине (g++, clang++, MSVC).
//
// Сборка:
//   g++ -std=c++17 -O2 -o ipcalc main.cpp
// Запуск:
//   ipcalc 192.168.1.10 255.255.255.0
//   ipcalc 192.168.1.10 24
//
// Если IP и маска/префикс не переданы аргументами, программа запросит их интерактивно.
// Требование "поле IP-адреса не должно сбрасывать введённые Маску/Префикс" в консольном
// приложении неприменимо (это особенность конкретных Win32-контролов), поэтому здесь
// маска и IP считываются независимо и никогда не перезаписывают друг друга.

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;

static bool ParseIPAddress(const string& text, uint32_t& outAddress)
{
    std::stringstream stream(text);
    string octetText;
    std::vector<int> octets;

    while (std::getline(stream, octetText, '.'))
    {
        if (octetText.empty()) return false;
        for (char c : octetText) if (!isdigit(static_cast<unsigned char>(c))) return false;

        int value = std::stoi(octetText);
        if (value < 0 || value > 255) return false;
        octets.push_back(value);
    }

    if (octets.size() != 4) return false;

    outAddress = (static_cast<uint32_t>(octets[0]) << 24) |
                 (static_cast<uint32_t>(octets[1]) << 16) |
                 (static_cast<uint32_t>(octets[2]) << 8)  |
                  static_cast<uint32_t>(octets[3]);
    return true;
}

// Маска может быть передана либо как точечная нотация (255.255.255.0),
// либо как префикс (0..32).
static bool ParseMask(const string& text, uint32_t& outMask, int& outPrefix)
{
    bool isPrefix = true;
    for (char c : text) if (!isdigit(static_cast<unsigned char>(c))) { isPrefix = false; break; }

    if (isPrefix && text.find('.') == string::npos)
    {
        int prefix = std::stoi(text);
        if (prefix < 0 || prefix > 32) return false;
        outPrefix = prefix;
        outMask = (prefix == 0) ? 0u : (0xFFFFFFFFu << (32 - prefix));
        return true;
    }

    uint32_t mask = 0;
    if (!ParseIPAddress(text, mask)) return false;

    // Проверяем, что маска состоит из непрерывной последовательности единиц слева направо.
    int prefix = 0;
    uint32_t probe = mask;
    while (probe & 0x80000000u) { prefix++; probe <<= 1; }
    uint32_t rebuilt = (prefix == 0) ? 0u : (0xFFFFFFFFu << (32 - prefix));
    if (rebuilt != mask) return false;

    outMask = mask;
    outPrefix = prefix;
    return true;
}

static string FormatIPAddress(uint32_t address)
{
    std::ostringstream out;
    out << ((address >> 24) & 0xFF) << '.'
        << ((address >> 16) & 0xFF) << '.'
        << ((address >> 8)  & 0xFF) << '.'
        << (address & 0xFF);
    return out.str();
}

int main(int argc, char* argv[])
{
    string ipText, maskText;

    if (argc >= 3)
    {
        ipText = argv[1];
        maskText = argv[2];
    }
    else
    {
        cout << "IP-адрес (например 192.168.1.10): ";
        std::getline(cin, ipText);
        cout << "Маска или префикс (например 255.255.255.0 или 24): ";
        std::getline(cin, maskText);
    }

    uint32_t ipAddress = 0;
    if (!ParseIPAddress(ipText, ipAddress))
    {
        cout << "Ошибка: некорректный IP-адрес: " << ipText << endl;
        return 1;
    }

    uint32_t mask = 0;
    int prefix = 0;
    if (!ParseMask(maskText, mask, prefix))
    {
        cout << "Ошибка: некорректная маска/префикс: " << maskText << endl;
        return 1;
    }

    uint32_t networkAddress = ipAddress & mask;
    uint32_t broadcastAddress = ipAddress | ~mask;

    // Количество IP-адресов в сети = 2^(32 - prefix).
    // Для /31 и /32 используем формулу без вычитания служебных адресов (RFC 3021 / отдельный узел).
    uint64_t ipAmount = (prefix == 0) ? (1ULL << 32) : (1ULL << (32 - prefix));
    uint64_t hostAmount = (prefix >= 31) ? ipAmount : (ipAmount - 2);

    cout << endl;
    cout << "Адрес сети:              " << FormatIPAddress(networkAddress) << endl;
    cout << "Широковещательный адрес: " << FormatIPAddress(broadcastAddress) << endl;
    cout << "Маска:                   " << FormatIPAddress(mask) << " (/" << prefix << ")" << endl;
    cout << "Количество IP-адресов:   " << ipAmount << endl;
    cout << "Количество узлов:        " << hostAmount << endl;

    return 0;
}
