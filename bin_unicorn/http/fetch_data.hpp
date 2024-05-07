#ifndef HTTP_FETCH_DATA_H_
#define HTTP_FETCH_DATA_H_

#include <string>
#include <vector>

#include "mbedtls/ssl.h"
extern "C"
{
#include "tls_client.h"
}

class RBCClient
{
public:
    /// @brief Initializes a new instance of the \c RBCClient class.
    RBCClient();

    /// @brief Fetch data on waste collection from Reading Borough Council.
    /// @param buffer The buffer that the UTF-8 response should be written to. Will be demoted to a pointer in tls_client.c; ensure it has enough space reserved.
    /// @param url_encoded_address The address to fetch collection data for. Must be URL-encoded.
    /// @return A result code from the https_get request. 0 if successful. See tls_client.h https_get() documentation.
    int32_t fetch_collection_data(const std::string &url_encoded_address, std::vector<char> &buffer);

private:
    TLS_CLIENT_SESSION_STATE_T *tls_session_state;
};

#endif // HTTP_FETCH_DATA_H