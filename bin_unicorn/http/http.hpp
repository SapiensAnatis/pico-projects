#ifndef HTTP_HTTP_H_
#define HTTP_HTTP_H_

#include <chrono>
#include <expected>
#include <iostream>
#include <string>

extern "C" {
#include "tls_client.h"
}

namespace http {

using namespace std::string_view_literals;

namespace {
// ISRG Root X1
// Expiry: Mon, 04 Jun 2035 11:04:38 GMT
constexpr uint8_t READING_GOV_UK_ROOT_CERT[] = "-----BEGIN CERTIFICATE-----\n\
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n\
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n\
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n\
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n\
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n\
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n\
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n\
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n\
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n\
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n\
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n\
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n\
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n\
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n\
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n\
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n\
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n\
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n\
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n\
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n\
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n\
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n\
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n\
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n\
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n\
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n\
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n\
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n\
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n\
-----END CERTIFICATE-----\n";

constexpr const char *READING_GOV_UK_HOST = "api.reading.gov.uk";

constexpr const char *READING_GOV_UK_HEADERS = "Accept: application/json\r\n\
User-Agent: bin_unicorn/0.1.0 RP2040\r\n\
GitHub-Username: sapiensanatis\r\n";
} // namespace

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
    std::string content_type;
    std::string_view body;
};

/// @brief Fetch data on waste collection from Reading Borough Council.
/// @param buffer The buffer that the UTF-8 response should be written to. Will be demoted to a
/// pointer in tls_client.c; ensure it has enough space reserved.
/// @param url_encoded_address The address to fetch collection data for. Must be URL-encoded.
/// @return A result code from the https_get request.
template <size_t BufferSize>
HttpsGetResult fetch_collection_data(std::string url_encoded_address,
                                     std::array<char, BufferSize> &buffer) {
    const auto uri = "/rbc/mycollections/" + url_encoded_address;

    TLS_CLIENT_REQUEST request = {
        request.hostname = READING_GOV_UK_HOST,
        request.headers = READING_GOV_UK_HEADERS,
        request.uri = uri.c_str(),
        request.cert = READING_GOV_UK_ROOT_CERT,
        request.cert_len = sizeof(READING_GOV_UK_ROOT_CERT),
    };

    std::cout << "Starting HTTPS GET: https://" << request.hostname << request.uri << "\n";

    auto start = std::chrono::high_resolution_clock::now();

    int8_t result = https_get(request, buffer.data(), BufferSize);

    if (result < 0) {
        std::cout << "Request failed; err=" << std::to_string(result) << "\n";
        return static_cast<HttpsGetResult>(result);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Request completed in " << milliseconds.count() << " ms\n";

    return HttpsGetResult::Success;
}

template <size_t BufferSize>
std::expected<HttpResponse, HttpsParseResult> parse_response(const std::array<char, BufferSize> &buffer) {
    /* A raw HTTP response looks like:
     *
     *   HTTP/1.1 200 OK
     *   Server: nginx
     *   Date: Tue, 11 Feb 2025 22:17:30 GMT
     *   Content-Type: application/json
     *   Content-Length: 1031
     *   Connection: close
     *   Access-Control-Allow-Origin: https://api.reading.gov.uk
     *   Vary: Origin
     *   
     *   {
     *     "Collections": [
     * ... <rest of the response body>
     * 
     * Each line is delimeted by \r\n.
     */

    std::string_view buffer_string(buffer.data(), BufferSize);
    std::string_view::iterator line_end;

    auto status_code_start = sizeof("HTTP/1.1");
    auto status_code_size = 3;

    if (!buffer_string.starts_with("HTTP/1.1") || buffer_string.length() < sizeof("HTTP/1.1") + status_code_size) {
        return std::unexpected(HttpsParseResult::Failure);
    }

    std::string_view status_code_view(buffer_string.begin() + status_code_start, status_code_size);
    uint16_t status_code;
    if (!try_parse_number(status_code_view, status_code)) {
        std::cerr << "Failed to parse status code\n";
        return std::unexpected(HttpsParseResult::Failure);
    }

    auto content_length_start = buffer_string.find("Content-Length:");
    auto content_length_end = buffer_string.find("\r\n", content_length_start);

    if (content_length_start == std::string::npos || content_length_end == std::string::npos) {
        std::cerr << "Failed to parse Content-Length\n";
        return std::unexpected(HttpsParseResult::Failure);
    }

    std::string_view content_length_view(buffer_string.begin() + content_length_start + sizeof("Content-Length:"), buffer_string.begin() + content_length_end);
    uint16_t content_length;
    if (!try_parse_number(content_length_view, content_length)) {
        return std::unexpected(HttpsParseResult::Failure);
    }

    auto content_type_start = buffer_string.find("Content-Type:");
    auto content_type_end = buffer_string.find("\r\n", content_type_start);

    if (content_length_start == std::string::npos || content_type_end == std::string::npos) {
        std::cerr << "Failed to parse Content-Type\n";
        return std::unexpected(HttpsParseResult::Failure);
    }

    std::string content_type(buffer_string.begin() + content_type_start + sizeof("Content-Type:"), buffer_string.begin() + content_type_end);

    auto body_start = buffer_string.find("\r\n\r\n");
    if (body_start == std::string::npos) {
        std::cerr << "Failed to find start of response body\n";
        return std::unexpected(HttpsParseResult::Failure);
    }

    std::string_view body(buffer_string.begin() + body_start, buffer_string.end());
    
    return HttpResponse{.status_code = status_code, .content_length = content_length, .content_type = content_type, .body = body};
}


} // namespace http

#endif // HTTP_HTTP_H_