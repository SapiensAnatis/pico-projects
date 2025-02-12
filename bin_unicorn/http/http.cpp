#include <chrono>
#include <cstdint>
#include <expected>
#include <iostream>
#include <optional>
#include <span>
#include <string>

extern "C" {
#include "tls_client.h"
}

#include "http.hpp"
#include "util.hpp"

namespace http {

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

consteval size_t string_length(const std::string &arg) {
    // Get string length without pesky null terminator which is included in sizeof()
    return arg.size();
}

HttpsGetResult fetch_collection_data(const std::string &url_encoded_address,
                                     std::span<char> &buffer) {
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

    int8_t result = https_get(request, buffer.data(), buffer.size());

    if (result < 0) {
        std::cout << "Request failed; err=" << std::to_string(result) << "\n";
        return static_cast<HttpsGetResult>(result);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Request completed in " << milliseconds.count() << " ms\n";

    return HttpsGetResult::Success;
}

std::optional<std::string_view> find_header_value(const std::string_view &buffer_string,
                                                  const std::string &header_name) {
    auto header_start = buffer_string.find(header_name);
    auto header_end = buffer_string.find("\r\n", header_start);

    if (header_start == std::string::npos || header_end == std::string::npos) {
        std::cerr << "Failed to find header: " << header_name << "\n";
        return std::nullopt;
    }

    // +2 for colon and space before value.
    auto header_value_start = header_start + header_name.length() + 2;

    return std::string_view(buffer_string.begin() + header_value_start,
                            buffer_string.begin() + header_end);
}

std::expected<HttpResponse, HttpsParseResult> parse_response(const std::span<char> &buffer) {
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

    std::string_view buffer_string(buffer.data(), buffer.size());

    auto status_code_start = string_length("HTTP/1.1 ");
    auto status_code_size = 3;

    if (!buffer_string.starts_with("HTTP/1.1") ||
        buffer_string.length() < string_length("HTTP/1.1 ") + status_code_size) {
        return std::unexpected(HttpsParseResult::Failure);
    }

    std::string_view status_code_view(buffer_string.begin() + status_code_start, status_code_size);
    uint16_t status_code;
    if (!try_parse_number(status_code_view, status_code)) {
        std::cerr << "Failed to parse status code\n";
        return std::unexpected(HttpsParseResult::Failure);
    }

    std::optional<std::string_view> content_length_view =
        find_header_value(buffer_string, "Content-Length");
    uint16_t content_length;
    if (!content_length_view || !try_parse_number(*content_length_view, content_length)) {
        std::cerr << "Failed to parse Content-Length\n";
        return std::unexpected(HttpsParseResult::Failure);
    }

    std::optional<std::string_view> content_type_view =
        find_header_value(buffer_string, "Content-Type");

    auto headers_end = buffer_string.find("\r\n\r\n");
    if (headers_end == std::string::npos) {
        std::cerr << "Failed to find end of response headers\n";
        return std::unexpected(HttpsParseResult::Failure);
    }

    std::string_view body(buffer_string.begin() + headers_end + string_length("\r\n\r\n"),
                          std::min(static_cast<size_t>(content_length), buffer.size()));

    return HttpResponse{.status_code = status_code,
                        .content_length = content_length,
                        .content_type = content_type_view,
                        .body = body};
}

} // namespace http