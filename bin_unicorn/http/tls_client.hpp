

#ifndef HTTP_TLS_CLIENT_H_
#define HTTP_TLS_CLIENT_H_

#include <cstdint>
#include <string>

#include "hardware/structs/rosc.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
extern "C"
{
#include "lwip/pbuf.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"
#include "lwip/tcpbase.h"
}

enum HttpsGetResult
{
    EmptyResponse = -3,
    GenericLwipError = -2,
    OpenConnectionFailure = -1,
    Success = 0,
};

struct TlsClientState
{
    struct altcp_pcb *pcb;
    bool complete;
    int32_t error;
    char *http_request;
    char *response;
    int32_t response_buffer_len;
    int32_t response_cursor;
    int32_t timeout;
};

/// @brief Request object for TlsClient.
struct TlsClientRequest
{
    std::string uri;
    std::string headers;
    const u8_t *cert;
    size_t cert_len;
};

class TlsClient
{
public:
    TlsClient(std::string hostname);
    HttpsGetResult HttpsGet(TlsClientRequest request, char *buffer, uint16_t buffer_len);

private:
    TlsClientState state;
    std::string hostname;

    bool OpenConnection(TlsClientState *state, altcp_tls_config *tls_config);
    static void DnsFound(const char *hostname, const ip_addr_t *ipaddr, void *arg);
    static void ConnectToServerIp(const ip_addr_t *ipaddr, TlsClientState *state);
    static err_t Connected(void *arg, struct altcp_pcb *pcb, err_t err);
    static err_t ReceiveData(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err);
    static err_t Poll(void *arg, altcp_pcb *pcb);
    static err_t Close(void *arg);
    static void Error(void *arg, err_t err);
};

#endif // HTTP_TLS_CLIENT_H_