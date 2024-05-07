#include <string>
#include <chrono>
#include <iostream>
#include <vector>
#include <format>

#include "config.hpp"
#include "fetch_data.hpp"

RBCClient::RBCClient()
{

    this->tls_session_state = (TLS_CLIENT_SESSION_STATE_T *)calloc(1, sizeof(TLS_CLIENT_SESSION_STATE_T));

    this->tls_session_state->session = (mbedtls_ssl_session *)calloc(1, sizeof(mbedtls_ssl_session));
    this->tls_session_state->has_session = false;
}

int32_t RBCClient::fetch_collection_data(const std::string &url_encoded_address, std::vector<char> &buffer)
{
    const auto uri = std::format("/rbc/mycollections/{}", url_encoded_address);
    constexpr uint8_t cert[] = READING_GOV_UK_ROOT_CERT;

    TLS_CLIENT_REQUEST request = {
        request.hostname = READING_GOV_UK_HOST,
        request.headers = READING_GOV_UK_HEADERS,
        request.uri = uri.c_str(),
        request.cert = cert,
        request.cert_len = sizeof(cert),
    };

    std::cout << "Starting HTTPS GET: https://" << request.hostname << request.uri << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    int32_t result = https_get(
        request,
        buffer.data(),
        buffer.capacity(),
        this->tls_session_state);

    auto end = std::chrono::high_resolution_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (result < 0)
    {
        std::cout << "Request failed with error code " << result << " after " << milliseconds.count() << " ms" << std::endl;
        return result;
    }

    std::cout << "Request completed in " << milliseconds.count() << " ms" << std::endl;

    return 0;
}
