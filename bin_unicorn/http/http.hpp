#ifndef HTTP_HTTP_H_
#define HTTP_HTTP_H_

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

extern "C" {
#include "tls_client.h"
}
#include "config.hpp"

namespace http {

/// @brief Fetch data on waste collection from Reading Borough Council.
/// @param buffer The buffer that the UTF-8 response should be written to. Will be demoted to a
/// pointer in tls_client.c; ensure it has enough space reserved.
/// @param url_encoded_address The address to fetch collection data for. Must be URL-encoded.
/// @return A result code from the https_get request. 0 if successful. See tls_client.h https_get()
/// documentation.
template <size_t BufferSize>
int8_t fetch_collection_data(std::string url_encoded_address,
                             std::array<char, BufferSize> &buffer) {
    const auto uri = "/rbc/mycollections/" + url_encoded_address;
    constexpr uint8_t cert[] = READING_GOV_UK_ROOT_CERT;

    TLS_CLIENT_REQUEST request = {
        request.hostname = READING_GOV_UK_HOST,
        request.headers = READING_GOV_UK_HEADERS,
        request.uri = uri.c_str(),
        request.cert = cert,
        request.cert_len = sizeof(cert),
    };

    std::cout << "Starting HTTPS GET: https://" << request.hostname << request.uri << "\n";

    auto start = std::chrono::high_resolution_clock::now();

    int8_t result = https_get(request, buffer.data(), BufferSize);

    if (result < 0) {
        std::cout << "Request failed; err=" << std::to_string(result) << "\n";
        return result;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Request completed in " << milliseconds.count() << " ms\n";

    return 0;
}

} // namespace http

#endif // HTTP_HTTP_H_