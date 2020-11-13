#include <string.h>
#include "bl_flash.h"
#include "hal/soc/soc.h"
#include "aos/kernel.h"
#include "board.h"
#include <bl_mtd.h>
#include <bl_boot2.h>
#include "hal/soc/flash.h"
#include "utils_log.h"
#include "utils_sha256.h"

#define ALI_FACTORY_5GROUP_NAME    "media"
#define CHIP_DEFAULT_NAME          "chipdef"
#define NOBIN_DEBUG_ENABLE         (0)

enum __TLV_TYPE {
   TLV_TYPE_VERSION = 0x0001,
   TLV_TYPE_KEY     = 0x0101,
   TLV_TYPE_VALUE   = 0x0102,
};

#if NOBIN_DEBUG_ENABLE
const static uint8_t test_buf[] = {
    0xA5, 0xA5, 0xA5, 0xA5,                               //magic
    0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,// default
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,       // hash
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x01, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,       // 0x0001, 9
    0x76, 0x65, 0x72, 0x30, 0x2E, 0x30, 0x2E, 0x32, 0x00, // ver0.0.2
    0x01, 0x01, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,// key01
    0x50, 0x72, 0x6F, 0x64, 0x75, 0x63, 0x74, 0x4B, 0x65, 0x79, 0x31, 0x00,
    0x02, 0x01, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x31, 0x31, 0x31, 0x31, 0x31, 0x00,
    0x01, 0x01, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,// key02
    0x50, 0x72, 0x6F, 0x64, 0x75, 0x63, 0x74, 0x4B, 0x65, 0x79, 0x32, 0x00,
    0x02, 0x01, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x32, 0x32, 0x32, 0x32, 0x32, 0x00,
    0x01, 0x01, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,// key03
    0x50, 0x72, 0x6F, 0x64, 0x75, 0x63, 0x74, 0x4B, 0x65, 0x79, 0x33, 0x00,
    0x02, 0x01, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x00,
};
#endif

/*
 * input
 *     key
 * output
 *     value
 * return
 *     <=0 success
 */
int prase_tlv_kv(uint8_t *addr, uint8_t **key, uint8_t **value)
{
    uint32_t type1, type2;
    uint32_t length1, length2;
    uint8_t *key_tmp, *value_tmp;
    int off = 0;

    /* check arg */
    if ((!addr) || (!key) || (!value)) {
        //printf("prase_tlv_kv arg error.\r\n");
        return -1;
    }

    type1 = *(uint32_t *)(addr + off);
    off += sizeof(uint8_t *);
    length1 = *(uint32_t *)(addr + off);
    off += sizeof(uint8_t *);
    key_tmp = (uint8_t *)(addr + off);
    off += length1;
    if (type1 != TLV_TYPE_KEY) {
        printf("not key type type1 = 0x%04lx\r\n", type1);
        return -2;
    }

    type2 = *(uint32_t *)(addr + off);
    off += sizeof(uint8_t *);
    length2 = *(uint32_t *)(addr + off);
    off += sizeof(uint8_t *);
    value_tmp = (uint8_t *)(addr + off);
    off += length2;
    if (type2 != TLV_TYPE_VALUE) {
        printf("not value type2 = 0x%04lx\r\n", type2);
        return -3;
    }

    *key = key_tmp;
    *value = value_tmp;
    return off;
}

/*
 * output
 *     version
 * return
 *     0-success
 */
int prase_tlv_version(uint8_t *addr, uint8_t **version)
{
    uint32_t type1, length1;
    uint8_t *value_tmp;
    int off = 0;

    /* check arg */
    if ((!addr) || (!version)) {
        printf("prase_tlv_version arg error.\r\n");
        return -1;
    }

    type1 = *(uint32_t *)(addr + off);
    off += sizeof(uint8_t *);
    length1 = *(uint32_t *)(addr + off);
    off += sizeof(uint8_t *);
    value_tmp = (uint8_t *)(addr + off);
    off += length1;
    if (type1 != TLV_TYPE_VERSION) {
        printf("not key type.\r\n");
        return -2;
    }

    *version = value_tmp;
    return off;
}

/*
 * output
 *     version
 * return
 *     (>0) success
 */
uint32_t prase_tlv_getnextoff(uint8_t *addr)
{
    uint32_t type1, length1;
    uint32_t off = 0;

    if (!addr) {
        return 0;
    }

    type1 = *(uint32_t *)(addr + off);
    off += sizeof(uint8_t *);
    length1 = *(uint32_t *)(addr + off);
    off += sizeof(uint8_t *);

    return (off + length1);
}

static void prase_media_partition(uint8_t *inputbuf, int inputlen)
{
    uint8_t *addr = NULL;
    uint8_t *version = NULL;
    uint8_t *key = NULL;
    uint8_t *value = NULL;
    uint8_t sha256_tmp[32];
    uint8_t *sha256_in;

    uint32_t tmp;
    uint32_t tmplen;

    uint32_t off = 0;
    int res = 0;

    addr = (uint8_t *)inputbuf;
    /* check arg */

    /* check magic check length */
    tmp = *(uint32_t *)(addr + off);
    if (tmp != 0xA5A5A5A5) {
        printf("magic error.\r\n");
        return;
    }
    off += 4;

    tmplen = *(uint32_t *)(addr + off);
    if (((tmplen + 64) > inputlen) || ((tmplen + 64) <= 0)) {//fixme
        printf("tmplen error.\r\n");
        return;
    }
    tmplen = *(uint32_t *)(addr + off);
    off += 4;

    off += 40;// default

    sha256_in = addr + off;
    off += 16;

    /* check hash */
    utils_sha256(addr + off, tmplen, sha256_tmp);
    if (0 != memcmp(sha256_tmp + 16, sha256_in, 16)) {
        printf("sha256 error.\r\n");
        return;
    }

    /* get version */
    res = prase_tlv_version(addr + off, &version);
    if (0 <= res) {
        printf("version = %s\r\n", version);
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, &value);
    if (0 <= res) {
        printf("KV = %s:%s\r\n", key, value);
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, &value);
    if (0 <= res) {
        printf("KV = %s:%s\r\n", key, value);
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, &value);
    if (0 <= res) {
        printf("KV = %s:%s\r\n", key, value);
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, &value);
    if (0 <= res) {
        printf("KV = %s:%s\r\n", key, value);
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, &value);
    if (0 <= res) {
        printf("KV = %s:%s\r\n", key, value);
    }
    off += res;
}

int ali_factory_media_info(uint8_t **addr, uint32_t *size)
{
    bl_mtd_handle_t hdr;
    bl_mtd_info_t hdrinfo;
    int res = 0;
    uint8_t *read_buf = NULL;

    res = bl_mtd_open(ALI_FACTORY_5GROUP_NAME, &hdr, BL_MTD_OPEN_FLAG_BUSADDR);
    if (0 != res) {
        log_info("no %s partition.\r\n", ALI_FACTORY_5GROUP_NAME);
        return -1;
    }
    bl_mtd_info(hdr, &hdrinfo);

    *addr = (uint8_t *)hdrinfo.xip_addr;
    *size = hdrinfo.size;

    bl_mtd_close(hdr);
    return 0;
}

int ali_factory_media_get(
        char **pp_product_key,
        char **pp_product_secret,
        char **pp_device_name,
        char **pp_device_secret,
        char **pp_pidStr)
{
    uint8_t *inputbuf;
    uint32_t inputlen;

    uint8_t *addr = NULL;
    uint8_t *version = NULL;
    uint8_t *key = NULL;
    uint8_t *value = NULL;
    uint8_t sha256_tmp[32];
    uint8_t *sha256_in;

    uint32_t tmp;
    uint32_t tmplen;

    uint32_t off = 0;
    int res = 0;

    if (0 != ali_factory_media_info(&inputbuf, &inputlen)) {
        printf("media partition invalid.\r\n");
        return -1;
    }

    addr = (uint8_t *)inputbuf;
    /* check arg */

    /* check magic check length */
    tmp = *(uint32_t *)(addr + off);
    if (tmp != 0xA5A5A5A5) {
        printf("media magic invalid.\r\n");
        return -2;
    }
    off += 4;

    tmplen = *(uint32_t *)(addr + off);
    if (((tmplen + 64) > inputlen) || ((tmplen + 64) <= 0)) {//fixme
        printf("media length invalid.\r\n");
        return -3;
    }
    tmplen = *(uint32_t *)(addr + off);
    off += 4;

    off += 40;// default

    sha256_in = addr + off;
    off += 16;

    /* check hash */
    utils_sha256(addr + off, tmplen, sha256_tmp);
    if (0 != memcmp(sha256_tmp + 16, sha256_in, 16)) {
        printf("media sha256 error.\r\n");
        return -4;
    }

    /* get version */
    res = prase_tlv_version(addr + off, &version);
    if (0 <= res) {
        printf("version = %s\r\n", version);
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, (uint8_t **)pp_product_key);
    if (0 <= res) {
        printf("KV = %s:%s\r\n", key, *pp_product_key);
    } else {
        return -5;
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, (uint8_t **)pp_device_name);
    if (0 <= res) {
        printf("KV = %s:%s\r\n", key, *pp_device_name);
    } else {
        return -6;
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, (uint8_t **)pp_device_secret);
    if (0 <= res) {
        //printf("KV = %s:%s\r\n", key, *pp_device_secret);
    } else {
        return -7;
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, (uint8_t **)pp_product_secret);
    if (0 <= res) {
        //printf("KV = %s:%s\r\n", key, *pp_product_secret);
    } else {
        return -8;
    }
    off += res;

    /* get kv */
    res = prase_tlv_kv(addr + off, &key, (uint8_t **)pp_pidStr);
    if (0 <= res) {
        //printf("KV = %s:%s\r\n", key, *pp_pidStr);
    } else {
        return -9;
    }
    off += res;

    return 0;
}

int ali_factory_media_print(void)
{
    bl_mtd_handle_t hdr;
    bl_mtd_info_t hdrinfo;
    int res = 0;
    uint8_t *read_buf = NULL;

    res = bl_mtd_open(ALI_FACTORY_5GROUP_NAME, &hdr, BL_MTD_OPEN_FLAG_BUSADDR);
    if (0 != res) {
        log_info("no %s partition.\r\n", ALI_FACTORY_5GROUP_NAME);
        return -1;
    }
    bl_mtd_info(hdr, &hdrinfo);

    printf("=================================================================\r\n");
    printf("=================================================================\r\n");
    printf("=================================================================\r\n");
    printf("name     = %s\r\n", (hdrinfo.name));
    printf("xip_addr = 0x%08lx\r\n", (uint32_t)(hdrinfo.xip_addr));
    printf("offset   = 0x%08lx\r\n", (uint32_t)(hdrinfo.offset));
    printf("size     = 0x%08lx\r\n", (uint32_t)(hdrinfo.size));
#if NOBIN_DEBUG_ENABLE
    prase_media_partition((uint8_t *)test_buf, sizeof(test_buf));
#else
    prase_media_partition((uint8_t *)hdrinfo.xip_addr, hdrinfo.size);
#endif
    printf("=================================================================\r\n");
    printf("=================================================================\r\n");
    printf("=================================================================\r\n");

    bl_mtd_close(hdr);
    return 0;
}

