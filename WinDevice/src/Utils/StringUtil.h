#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <locale>
#include <codecvt>

template<typename To, typename From>
To Convert(const From& input) {
    std::wstring_convert<std::codecvt_utf8<typename From::value_type>, typename From::value_type> converter;
    return converter.to_bytes(input);
}

std::string Wstring2String(const std::wstring& input) {
    return Convert<std::string>(input);
}

std::string Wchar2String(const WCHAR* input) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(input);
}


#endif // STRING_CONVERSION_H