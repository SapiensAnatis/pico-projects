#ifndef TLS_CLIENT_H_
#define TLS_CLIENT_H_

int https_get(const char *hostname, const char *uri, const char *headers, char *buffer, int buffer_len);

#endif // TLS_CLIENT_H_