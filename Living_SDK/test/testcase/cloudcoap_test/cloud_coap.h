#ifndef __CLOUD_COAP_H__
#define __CLOUD_COAP_H__

#define CLOUD_COAP_LOG_TAG "[CloudCoap]:"

#define CLOUD_COAP_DEFAULT_ENV "online"
#define CLOUD_COAP_DEFAULT_SECURE_TYPE "psk"

#define CLOUD_COAP_URL_MAX_LEN (128)
#define CLOUD_COAP_YIELD_TIMEOUT (200)
#define CLOUD_COAP_CONNECT_TIMEOUT (300)
/* daily url */
#define IOTX_DAILY_DTLS_SERVER_URI "coaps://11.239.164.238:5684"
#define IOTX_DAILY_PSK_SERVER_URI "coap-psk://10.101.83.159:5682"

/* pre url */
#define IOTX_PRE_DTLS_SERVER_URI "coaps://pre.coap.cn-shanghai.link.aliyuncs.com:5684"
#define IOTX_PRE_NOSEC_SERVER_URI "coap://pre.coap.cn-shanghai.link.aliyuncs.com:5683"
#define IOTX_PRE_PSK_SERVER_URI "coap-psk://pre.coap.cn-shanghai.link.aliyuncs.com:5683"

/* online url */
#define IOTX_ONLINE_DTLS_SERVER_URL "coaps://%s.coap.cn-shanghai.link.aliyuncs.com:5684"
#define IOTX_ONLINE_NOSEC_SERVER_URI "coap://%s.coap.cn-shanghai.link.aliyuncs.com:5683"
#define IOTX_ONLINE_PSK_SERVER_URL "coap-psk://%s.coap.cn-shanghai.link.aliyuncs.com:5682"

#endif
