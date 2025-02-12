#ifndef HTTP_HTTP_H_
#define HTTP_HTTP_H_

#include <expected>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace http {

enum class HttpsGetResult : int8_t {
    Success = 0,
    ClientTimeout = -1,
    FailedToConnect = -2,
    FailedToAllocState = -3,
    EmptyResponse = -4,
};

enum class HttpsParseResult : int8_t {
    Failure = -1,
};

struct HttpResponse {
    uint16_t status_code;
    uint16_t content_length;
    std::optional<std::string_view> content_type;
    std::string_view body;
};

/// @brief Fetch data on waste collection from Reading Borough Council.
/// @param buffer The buffer that the UTF-8 response should be written to. Will be demoted to a
/// pointer in tls_client.c; ensure it has enough space reserved.
/// @param url_encoded_address The address to fetch collection data for. Must be URL-encoded.
/// @return A result code from the https_get request.
HttpsGetResult fetch_collection_data(const std::string &url_encoded_address,
                                     std::span<char> &buffer);

/// @brief Parse a returned HTTP response buffer to retrieve the status code and some useful
/// headers.
/// @param buffer The buffer that the UTF-8 response was written to.
/// @return A @ref HttpResponse, or a @ref HttpParseResult error if parsing failed.
std::expected<HttpResponse, HttpsParseResult> parse_response(const std::span<char> &buffer);

} // namespace http

#endif // HTTP_HTTP_H_