#pragma once

#include <windows.h>
#include <string>
#include <algorithm>

static std::string wide_to_utf8(std::wstring_view w)
{
    if (w.empty()) return {};

    int size = WideCharToMultiByte(
        CP_UTF8, 0,
        w.data(), (int)w.size(),
        nullptr, 0, nullptr, nullptr);

    std::string result(size, '\0');

    WideCharToMultiByte(
        CP_UTF8, 0,
        w.data(), (int)w.size(),
        result.data(), size,
        nullptr, nullptr);

    return result;
}

static std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

static DWORD copy_string(
    LPCSTR lpDefault,
    LPSTR  lpReturnedString,
    DWORD  nSize
) {
    if (nSize == 0 || lpReturnedString == nullptr)
    {
        return 0;
    }

    if (lpDefault == nullptr)
    {
        lpDefault = "";
    }

    // Number of chars we can copy (leave space for null terminator)
    const size_t maxCopy = nSize - 1;

    size_t len = std::strlen(lpDefault);
    size_t toCopy = (len < maxCopy) ? len : maxCopy;

    std::memcpy(lpReturnedString, lpDefault, toCopy);
    lpReturnedString[toCopy] = '\0';

    // Return number of chars copied (excluding null terminator)
    return static_cast<DWORD>(toCopy);
}