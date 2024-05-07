/*
 * Adapted from pico_w/wifi/tls_client/tls_verify.c of https://github.com/raspberrypi/pico-examples/blob/eca13acf57916a0bd5961028314006983894fc84/pico_w/wifi/tls_client/tls_verify.c
 * Additional credit to Micheal Bell for https_get: https://github.com/MichaelBell/Picodon/blob/08d30cfb9d10d6afd966fdd4f9210867cf3bb461/tls_client.c#L268
 */

#define TLS_CLIENT_REQUEST_FORMAT "GET %s HTTP/1.1\r\n"   \
                                  "Host: %s\r\n"          \
                                  "Connection: close\r\n" \
                                  "%s\r\n"                \
                                  "\r\n"
#define TLS_CLIENT_TIMEOUT_SECS 30

#include <string.h>
#include <time.h>

#include "hardware/structs/rosc.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"
#include "lwip/tcpbase.h"
#include "tls_client.h"

typedef struct TLS_CLIENT_T_
{
    struct altcp_pcb *pcb;
    bool complete;
    int32_t error;
    uint8_t *http_request;
    uint8_t *response;
    int32_t response_buffer_len;
    int32_t response_cursor;
    int32_t timeout;
} TLS_CLIENT_T;

static struct altcp_tls_config *tls_config = NULL;

static err_t tls_client_close(void *arg)
{
    TLS_CLIENT_T *state = (TLS_CLIENT_T *)arg;
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
            printf("close failed %d, calling abort\n", err);
            altcp_abort(state->pcb);
            err = ERR_ABRT;
        }
        state->pcb = NULL;
    }
    return err;
}

static err_t tls_client_connected(void *arg, struct altcp_pcb *pcb, err_t err)
{
    TLS_CLIENT_T *state = (TLS_CLIENT_T *)arg;
    if (err != ERR_OK)
    {
        printf("connect failed %d\n", err);
        return tls_client_close(state);
    }

    printf("connected to server, sending request\n");
    err = altcp_write(state->pcb, state->http_request, strlen(state->http_request), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK)
    {
        printf("error writing data, err=%d", err);
        return tls_client_close(state);
    }

    return ERR_OK;
}

static err_t tls_client_poll(void *arg, struct altcp_pcb *pcb)
{
    TLS_CLIENT_T *state = (TLS_CLIENT_T *)arg;
    printf("timed out\n");
    state->error = PICO_ERROR_TIMEOUT;
    return tls_client_close(arg);
}

static void tls_client_err(void *arg, err_t err)
{
    TLS_CLIENT_T *state = (TLS_CLIENT_T *)arg;
    printf("tls_client_err %d\n", err);
    tls_client_close(state);
    state->error = PICO_ERROR_GENERIC;
}

static err_t tls_client_recv(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err)
{
    TLS_CLIENT_T *state = (TLS_CLIENT_T *)arg;
    if (!p)
    {
        printf("connection closed\n");
        return tls_client_close(state);
    }

    int copy_len = p->tot_len;
    if (p->tot_len + state->response_cursor > state->response_buffer_len)
    {
        printf("HTTPS response exceeded buffer length\n");
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

            printf("***\nnew data received from server:\n***\n\n%s\n", buffer);
        }

        altcp_recved(pcb, p->tot_len);
    }
    pbuf_free(p);

    return ERR_OK;
}

static void tls_client_connect_to_server_ip(const ip_addr_t *ipaddr, TLS_CLIENT_T *state)
{
    err_t err;
    u16_t port = 443;

    printf("connecting to server IP %s port %d\n", ipaddr_ntoa(ipaddr), port);
    err = altcp_connect(state->pcb, ipaddr, port, tls_client_connected);
    if (err != ERR_OK)
    {
        fprintf(stderr, "error initiating connect, err=%d\n", err);
        tls_client_close(state);
    }
}

static void tls_client_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg)
{
    if (ipaddr)
    {
        printf("DNS resolving complete\n");
        tls_client_connect_to_server_ip(ipaddr, (TLS_CLIENT_T *)arg);
    }
    else
    {
        printf("error resolving hostname %s\n", hostname);
        tls_client_close(arg);
    }
}

static bool tls_client_open(const char *hostname, void *arg)
{
    err_t err;
    ip_addr_t server_ip;
    TLS_CLIENT_T *state = (TLS_CLIENT_T *)arg;

    state->pcb = altcp_tls_new(tls_config, IPADDR_TYPE_ANY);
    if (!state->pcb)
    {
        printf("failed to create pcb\n");
        return false;
    }

    altcp_arg(state->pcb, state);
    altcp_poll(state->pcb, tls_client_poll, state->timeout * 2);
    altcp_recv(state->pcb, tls_client_recv);
    altcp_err(state->pcb, tls_client_err);

    /* Set SNI */
    mbedtls_ssl_set_hostname(altcp_tls_context(state->pcb), hostname);

    printf("resolving %s\n", hostname);

    // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
    // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
    // these calls are a no-op and can be omitted, but it is a good practice to use them in
    // case you switch the cyw43_arch type later.
    cyw43_arch_lwip_begin();

    err = dns_gethostbyname(hostname, &server_ip, tls_client_dns_found, state);
    if (err == ERR_OK)
    {
        /* host is in DNS cache */
        tls_client_connect_to_server_ip(&server_ip, state);
    }
    else if (err != ERR_INPROGRESS)
    {
        printf("error initiating DNS resolving, err=%d\n", err);
        tls_client_close(state->pcb);
    }

    cyw43_arch_lwip_end();

    return err == ERR_OK || err == ERR_INPROGRESS;
}

// Perform initialisation
static TLS_CLIENT_T *tls_client_init(void)
{
    TLS_CLIENT_T *state = calloc(1, sizeof(TLS_CLIENT_T));
    if (!state)
    {
        printf("failed to allocate state\n");
        return NULL;
    }

    return state;
}

int32_t https_get(TLS_CLIENT_REQUEST request, char *restrict buffer, uint16_t buffer_len)
{
    tls_config = altcp_tls_create_config_client(request.cert, request.cert_len);

    TLS_CLIENT_T *state = tls_client_init();
    if (!state)
    {
        return -3;
    }

    state->timeout = TLS_CLIENT_TIMEOUT_SECS;

    state->http_request = buffer;
    snprintf(state->http_request, buffer_len, TLS_CLIENT_REQUEST_FORMAT, request.uri, request.hostname, request.headers);

    state->response = buffer;
    state->response_buffer_len = buffer_len;

    if (!tls_client_open(request.hostname, state))
    {
        return -2;
    }

    while (!state->complete)
    {
#if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll();
#endif
        sleep_ms(1);
    }

    if (state->error != 0)
    {
        return state->error;
    }

    int response_length = state->response_cursor;

    free(state);
    altcp_tls_free_config(tls_config);

    if (response_length == 0)
    {
        return -4;
    }

    return response_length;
}