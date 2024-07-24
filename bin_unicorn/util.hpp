#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <cstdint>

static constexpr char hex_digits[] = "0123456789ABCDEF";

/// @brief Convert a character to its hex equivalent, e.g. " " -> "20"
/// @param num The character to convert.
/// @return The hex string.
static constexpr std::string to_hex_string(char num)
{
    std::string result;

    while (num > 0)
    {
        auto remainder = num % 16;
        result = hex_digits[remainder] + result;
        num = num / 16;
    }

    return result;
}

/// @brief URL-encode a string, escaping any non-URL-safe characters.
/// @param input The input string.
/// @return The input string, URL-encoded.
constexpr std::string url_encode(std::string input)
{
    std::string result;

    for (char &c : input)
    {
        if (std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~')
        {
            result += c;
        }
        else
        {
            result += '%';
            result += to_hex_string(c);
        }
    }

    return result;
}

#endif // UTIL_H_