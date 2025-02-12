#ifndef UTIL_H_
#define UTIL_H_

#include <charconv>
#include <cstdint>
#include <iostream>
#include <string>

static constexpr char HEX_DIGITS[] = "0123456789ABCDEF";

/// @brief Convert a character to its hex equivalent, e.g. " " -> "20"
/// @param num The character to convert.
/// @return The hex string.
static std::string to_hex_string(char num) {
    std::string result;

    while (num > 0) {
        auto remainder = num % 16;
        result = HEX_DIGITS[remainder] + result;
        num = num / 16;
    }

    return result;
}

/// @brief URL-encode a string, escaping any non-URL-safe characters.
/// @param input The input string.
/// @return The input string, URL-encoded.
inline std::string url_encode(std::string input) {
    std::string result;

    for (char &c : input) {
        if (std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~') {
            result += c;
        } else {
            result += '%';
            result += to_hex_string(c);
        }
    }

    return result;
}

/// @brief Attempt to parse a string into a number.
/// @tparam TNumber The numeric type to parse into.
/// @param input The input string.
/// @param out A reference to a number to assign the result to, if parsing is successful.
/// @return A boolean indicating whether parsing succeeded.
template <typename TNumber>
static bool try_parse_number(const std::string_view &input, TNumber &out) {
    const std::from_chars_result result =
        std::from_chars(input.data(), input.data() + input.size(), out);

    if (result.ec == std::errc::invalid_argument || result.ec == std::errc::result_out_of_range) {
        return false;
    }

    return true;
}

#endif // UTIL_H_