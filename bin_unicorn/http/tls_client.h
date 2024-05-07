#ifndef HTTP_TLS_CLIENT_H_
#define HTTP_TLS_CLIENT_H_

/// @brief Request object for tls_client.
typedef struct TLS_CLIENT_REQUEST_T_
{
    const char *hostname;
    const char *uri;
    const char *headers;
    const uint8_t *cert;
    size_t cert_len;
} TLS_CLIENT_REQUEST;

/// @brief Send a HTTPS GET request.
/// @param request Request details.
/// @param buffer Buffer to write the response to.
/// @param buffer_len The length of \p buffer.
/// @return If greater than 0, the number of bytes written to \p buffer.
/// Otherwise, one of the following error codes:
///   * -1: Client timeout.
///   * -2: Failed to resolve DNS & open connection, or the tls_client_err callback was hit.
///   * -3: Failed to allocate client state.
///   * -4: Request was apparently successful, but no bytes were written to the buffer.
int32_t https_get(TLS_CLIENT_REQUEST request, char *buffer, uint16_t buffer_len);

#endif // HTTP_TLS_CLIENT_H_