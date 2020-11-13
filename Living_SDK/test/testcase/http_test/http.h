#ifndef __HTTP_H__
#define __HTTP_H__

#define MAX_BUF_LEN 256
#define DEFAULT_TIMEOUT_MS 5000

#define HTTP_LOG_TAG "[HTTP]:"

extern int ct_http(const char *host, uint16_t port, int count);

#endif
