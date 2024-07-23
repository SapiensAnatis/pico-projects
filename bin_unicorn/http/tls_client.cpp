/*
 * Adapted from pico_w/wifi/tls_client/tls_verify.c of https://github.com/raspberrypi/pico-examples/blob/eca13acf57916a0bd5961028314006983894fc84/pico_w/wifi/tls_client/tls_verify.c
 * Additional credit to Micheal Bell for https_get: https://github.com/MichaelBell/Picodon/blob/08d30cfb9d10d6afd966fdd4f9210867cf3bb461/tls_client.c#L268
 */

#define TLS_CLIENT_REQUEST_FORMAT "GET %s HTTP/1.1\r\n"   \
                                  "Host: %s\r\n"          \
                                  "Connection: close\r\n" \
                                  "%s\r\n"                \
                                  "\r\n"
#define TLS_CLIENT_TIMEOUT_SECS 60

#include <string.h>
#include <time.h>
#include <iostream>

#include "pico/cyw43_arch.h"

#include "tls_client.hpp"

TlsClient::TlsClient(std::string hostname)
{
    this->hostname = hostname;
    this->state = {};
    this->state.timeout = TLS_CLIENT_TIMEOUT_SECS;
}

HttpsGetResult TlsClient::HttpsGet(TlsClientRequest request, char *buffer, uint16_t buffer_len)
{
    auto tls_config = altcp_tls_create_config_client(request.cert, request.cert_len);

    this->state.http_request = buffer;
    snprintf(this->state.http_request, buffer_len, TLS_CLIENT_REQUEST_FORMAT, request.uri.c_str(), this->hostname.c_str(), request.headers.c_str());

    this->state.response = buffer;
    this->state.response_buffer_len = buffer_len;

    if (!OpenConnection(&this->state, tls_config))
    {
        return HttpsGetResult::OpenConnectionFailure;
    }

    while (!this->state.complete)
    {
#if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll();
#endif
        sleep_ms(1);
    }

    if (this->state.error != 0)
    {
        std::cerr << "HTTPS request failed: err=" << this->state.error << "\n";
        return HttpsGetResult::GenericLwipError;
    }

    int response_length = this->state.response_cursor;

    altcp_tls_free_config(tls_config);

    if (response_length == 0)
    {
        return HttpsGetResult::EmptyResponse;
    }

    return HttpsGetResult::Success;
}

bool TlsClient::OpenConnection(TlsClientState *state, altcp_tls_config *tls_config)
{
    err_t err;
    ip_addr_t server_ip;

    this->state.pcb = altcp_tls_new(tls_config, IPADDR_TYPE_ANY);
    if (!this->state.pcb)
    {
        std::cerr << "Failed to create TLS pcb\n";
        return false;
    }

    altcp_arg(state->pcb, state);
    altcp_poll(state->pcb, this->Poll, state->timeout * 2);
    altcp_recv(state->pcb, this->ReceiveData);
    altcp_err(state->pcb, this->Error);

    /* Set SNI */
    mbedtls_ssl_set_hostname((mbedtls_ssl_context *)altcp_tls_context(state->pcb), this->hostname.c_str());

    std::cout << "Resolving host " << hostname << "\n";

    // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
    // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
    // these calls are a no-op and can be omitted, but it is a good practice to use them in
    // case you switch the cyw43_arch type later.
    cyw43_arch_lwip_begin();

    err = dns_gethostbyname(hostname.c_str(), &server_ip, DnsFound, state);
    if (err == ERR_OK)
    {
        /* host is in DNS cache */
        ConnectToServerIp(&server_ip, state);
    }
    else if (err != ERR_INPROGRESS)
    {
        std::cout << "Error initiating DNS resolving, err=" << err << "\n";
        Close(state->pcb);
    }

    cyw43_arch_lwip_end();

    return err == ERR_OK || err == ERR_INPROGRESS;
}

void TlsClient::DnsFound(const char *hostname, const ip_addr_t *ipaddr, void *arg)
{
    if (ipaddr)
    {
        std::cout << "DNS resolving complete\n";
        ConnectToServerIp(ipaddr, (TlsClientState *)arg);
    }
    else
    {
        std::cout << "Error resolving hostname" << hostname << "\n";
        Close(arg);
    }
}

err_t TlsClient::Poll(void *arg, struct altcp_pcb *pcb)
{
    TlsClientState *state = (TlsClientState *)arg;
    std::cout << "TLS request timed out\n";
    state->error = PICO_ERROR_TIMEOUT;
    return Close(arg);
}

err_t TlsClient::Close(void *arg)
{
    TlsClientState *state = (TlsClientState *)arg;
    err_t err = ERR_OK;

    state->complete = true;
    if (state->pcb != NULL)
    {
        altcp_arg(state->pcb, NULL);
        altcp_poll(state->pcb, NULL, 0);
        altcp_recv(state->pcb, NULL);
        altcp_err(state->pcb, NULL);
        err = altcp_close(state->pcb);
        if (err != ERR_OK)
        {
            std::cout << "TLS client close failed: " << err << ". Calling abort\n";
            altcp_abort(state->pcb);
            err = ERR_ABRT;
        }
        state->pcb = NULL;
    }
    return err;
}

void TlsClient::ConnectToServerIp(const ip_addr_t *ipaddr, TlsClientState *state)
{
    err_t err;
    u16_t port = 443;

    std::cout << "Connecting to server IP " << ipaddr_ntoa(ipaddr) << " port " << port << "\n";
    err = altcp_connect(state->pcb, ipaddr, port, Connected);
    if (err != ERR_OK)
    {
        std::cerr << "Error initiating connect, err=" << err << "\n";
        Close(state);
    }
}

err_t TlsClient::ReceiveData(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err)
{
    TlsClientState *state = (TlsClientState *)arg;
    if (!p)
    {
        std::cout << "Connection closed\n";
        return Close(state);
    }

    int copy_len = p->tot_len;
    if (p->tot_len + state->response_cursor > state->response_buffer_len)
    {
        std::cerr << "HTTPS response exceeded buffer length\n";
        copy_len = state->response_buffer_len - state->response_cursor - 1;
    }

    if (p->tot_len > 0)
    {
        if (copy_len > 0)
        {
            char *buffer = state->response + state->response_cursor;
            pbuf_copy_partial(p, buffer, copy_len, 0);
            buffer[copy_len] = 0;
            state->response_cursor += copy_len;

#ifdef DEBUG
            std::cout << "***\nnew data received from server:\n***\n\n"
                      << buffer << "\n";
#endif
        }

        altcp_recved(pcb, p->tot_len);
    }
    pbuf_free(p);

    return ERR_OK;
}

err_t TlsClient::Connected(void *arg, struct altcp_pcb *pcb, err_t err)
{
    TlsClientState *state = (TlsClientState *)arg;
    if (err != ERR_OK)
    {
        std::cerr << "Connection failed, err=" << err << "\n";
        return Close(state);
    }

    std::cout << "Connected to server, sending request\n";
    std::cout << "Request contents: " << state->http_request << "\n";

    err = altcp_write(state->pcb, state->http_request, strlen(state->http_request), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK)
    {
        std::cerr << "Error writing data, err=" << err << "\n";
        return Close(state);
    }

    return ERR_OK;
}

void TlsClient::Error(void *arg, err_t err)
{
    TlsClientState *state = (TlsClientState *)arg;
    std::cout << "tls_client_err=" << err << "\n";
    Close(state);
    state->error = PICO_ERROR_GENERIC;
}