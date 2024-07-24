#include <string>
#include <iostream>
#include <vector>
#include <chrono>

extern "C"
{
#include "tls_client.h"
}
#include "config.hpp"
#include "fetch_data.hpp"

int8_t fetch_collection_data(const std::string &url_encoded_address, std::vector<char> &buffer)
{
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

    int8_t result = https_get(
        request,
        buffer.data(),
        buffer.capacity());

    if (result < 0)
    {
        std::cout << "Request failed with error code " << result << "\n";
        return result;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Request completed in " << milliseconds.count() << " ms\n";

    return 0;
}
