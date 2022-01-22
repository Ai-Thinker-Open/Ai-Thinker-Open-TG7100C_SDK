/**
 * @file aiot_kv_api.c
 * @brief kv module which is for data storage
 * @date 2020-07-08
 * 
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "iot_import.h"
#include "iotx_log.h"

#include "aiot_al_kv.h"
#include "aiot_kv_api.h"

#ifdef BUILD_AOS
#include <aos/cli.h>
#ifdef DEFAULT_LOG_LEVEL_DEBUG
#include <ctype.h>
#define SUPPORT_KV_LIST_CMD 1
#endif
#endif

static kv_handle_t g_kv_mgr;
void *gc_thread;

static const uint8_t BLK_MAGIC_NUM = 'K';  /* The block header magic number */
static const uint8_t ITEM_MAGIC_NUM = 'I'; /* The key-value item header magic number */

static void *aos_kv_gc(void *arg);

static void kv_al_free(void *ptr)
{
#ifdef BUILD_AOS
    aos_free(ptr);
#else
    HAL_Free(ptr);
#endif
}

static void *kv_al_malloc(uint32_t size)
{
#ifdef BUILD_AOS
    return aos_malloc(size);
#else
    return HAL_Malloc(size);
#endif
}

void *aiot_al_mutex_init(void)
{
#ifdef BUILD_AOS
    aos_mutex_t mutex;
    if (0 != aos_mutex_new(&mutex))
    {
        return NULL;
    }

    return mutex.hdl;
#else
    return HAL_MutexCreate();
#endif
}

void aiot_al_mutex_lock(void *mutex)
{
#ifdef BUILD_AOS
    aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
#else
    HAL_MutexLock(mutex);
#endif
}

void aiot_al_mutex_unlock(void *mutex)
{
#ifdef BUILD_AOS
    aos_mutex_unlock((aos_mutex_t *)&mutex);
#else
    HAL_MutexUnlock(mutex);
#endif
}

void aiot_al_mutex_deinit(void **mutex)
{
#ifdef BUILD_AOS
    if (NULL != mutex)
    {
        aos_mutex_free(*(aos_mutex_t **)mutex);
        *mutex = NULL;
    }
#else
    HAL_MutexDestroy(mutex);
#endif
}

int aiot_al_thread_create(void **thread_handle,
                          void *(*work_routine)(void *),
                          void *arg,
                          hal_os_thread_param_t *hal_os_thread_param,
                          int *stack_used)
{
    hal_os_thread_param_t os_thread_param;

    memset(&os_thread_param, 0, sizeof(hal_os_thread_param_t));
    os_thread_param.stack_size = KV_GC_TASK_STACK_SIZE;
    os_thread_param.name = KV_GC_TASK_NAME;

    return HAL_ThreadCreate(thread_handle, work_routine, arg, &os_thread_param, stack_used);
}

void aiot_al_thread_delete(void *thread_handle)
{
#ifdef BUILD_AOS
    if (thread_handle != NULL)
    {
        aos_free(thread_handle);
        thread_handle = NULL;
    }
    aos_task_exit(0);
#else
    HAL_ThreadDelete(thread_handle);
#endif
}

/* CRC-8: the poly is 0x31 (x^8 + x^5 + x^4 + 1) */
static uint8_t utils_crc8(uint8_t *buf, uint16_t length)
{
    uint8_t crc = 0x00;
    uint8_t i;

    while (length--)
    {
        crc ^= *buf++;
        for (i = 8; i > 0; i--)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x31;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

static int raw_read(uint32_t offset, void *buf, size_t nbytes)
{
#if defined(BUILD_AOS)
    return hal_flash_read((hal_partition_t)KV_PTN, (uint32_t *)&offset, buf, nbytes);
#else
    return aiot_al_kv_flash_read(&offset, buf, nbytes);
#endif
}

static int raw_write(uint32_t offset, const void *buf, size_t nbytes)
{
#if defined(BUILD_AOS)
    return hal_flash_write((hal_partition_t)KV_PTN, (uint32_t *)&offset, buf, nbytes);
#else
    return aiot_al_kv_flash_write(&offset, buf, nbytes);
#endif
}

static int raw_erase(uint32_t offset, uint32_t size)
{
#if defined(BUILD_AOS)
    return hal_flash_erase((hal_partition_t)KV_PTN, offset, size);
#else
    return aiot_al_kv_flash_erase(offset, size);
#endif
}

#ifdef SUPPORT_GC_TASK_STACK_SIZE
static int trigger_gc(void)
{
    int ret = -1;
    pthread_t gc_thread;
    pthread_attr_t attr;

    if (g_kv_mgr.gc_triggered)
    {
        return 0;
    }

    g_kv_mgr.gc_triggered = 1;

    ret = pthread_attr_init(&attr);
    if (ret != 0)
    {
        log_err(KV_MODULE_NAME, "ret:%d", ret);
        return -1;
    }

    ret = pthread_attr_setstacksize(&attr, KV_GC_STACK_SIZE);
    if (ret != 0)
    {
        log_err(KV_MODULE_NAME, "ret:%d", ret);
        return -1;
    }

    ret = pthread_create(&gc_thread, &attr, aos_kv_gc, NULL);
    if (ret != 0)
    {
        log_err(KV_MODULE_NAME, "ret:%d", ret);
        return -1;
    }

    ret = pthread_attr_destroy(&attr);
    if (ret != 0)
    {
        log_err(KV_MODULE_NAME, "ret:%d", ret);
        return -1;
    }

    return 0;
}
#else
static int trigger_gc(void)
{
    int ret = -1;

    if (g_kv_mgr.gc_triggered)
    {
        return 0;
    }

    g_kv_mgr.gc_triggered = 1;

    ret = aiot_al_thread_create(&gc_thread, aos_kv_gc, NULL, NULL, NULL);
    if (ret != 0)
    {
        log_err(KV_MODULE_NAME, "ret:%d", ret);
        return -1;
    }

    return 0;
}
#endif

static void kv_item_free(kv_item_t *item)
{
    if (item)
    {
        if (item->store)
        {
            kv_al_free(item->store);
        }
        kv_al_free(item);
    }
}

static int kv_state_set(uint16_t pos, uint8_t state)
{
    return raw_write(pos + KV_STATE_OFF, &state, 1);
}

static int kv_block_format(uint8_t index)
{
    block_hdr_t hdr;
    uint16_t pos = index << BLK_BITS;

    memset(&hdr, 0, sizeof(hdr));
    hdr.magic = BLK_MAGIC_NUM;
    if (!raw_erase(pos, BLK_SIZE))
    {
        hdr.state = BLK_STATE_CLEAN;
    }
    else
    {
        return RES_FLASH_EARSE_ERR;
    }

    if (raw_write(pos, &hdr, BLK_HEADER_SIZE) != RES_OK)
    {
        return RES_FLASH_WRITE_ERR;
    }
    g_kv_mgr.block_info[index].state = BLK_STATE_CLEAN;
    g_kv_mgr.block_info[index].space = BLK_SIZE - BLK_HEADER_SIZE;
    (g_kv_mgr.clean_blk_nums)++;

    return RES_OK;
}

static uint16_t kv_item_calc_pos(uint16_t len)
{
    block_info_t *blk_info;
    uint8_t blk_index = (g_kv_mgr.write_pos) >> BLK_BITS;
#if BLK_NUMS > KV_GC_RESERVED + 1
    uint8_t i;
#endif

    blk_info = &(g_kv_mgr.block_info[blk_index]);

    if (blk_info->space > len)
    {
        if (((blk_info->space - len) < ITEM_MAX_LEN) && (g_kv_mgr.clean_blk_nums <= KV_GC_RESERVED))
        {
            trigger_gc();
        }
        return g_kv_mgr.write_pos;
    }

#if BLK_NUMS > KV_GC_RESERVED + 1
    for (i = blk_index + 1; i != blk_index; i++)
    {
        if (i == BLK_NUMS)
        {
            i = 0;
        }

        blk_info = &(g_kv_mgr.block_info[i]);
        if ((blk_info->space) > len)
        {
            g_kv_mgr.write_pos = (i << BLK_BITS) + BLK_SIZE - blk_info->space;
            if (blk_info->state == BLK_STATE_CLEAN)
            {
                if (kv_state_set((i << BLK_BITS), BLK_STATE_USED) != RES_OK)
                {
                    return 0;
                }
                blk_info->state = BLK_STATE_USED;
                (g_kv_mgr.clean_blk_nums)--;
            }
            return g_kv_mgr.write_pos;
        }
    }
#endif

    trigger_gc();
    return 0;
}

static int kv_item_del(kv_item_t *item, int mode)
{
    int ret = RES_OK;
    item_hdr_t hdr;
    char *origin_key = NULL;
    char *new_key = NULL;
    uint8_t i;
    uint16_t offset;

    if (mode == KV_SELF_REMOVE)
    {
        offset = item->pos;
    }
    else if (mode == KV_ORIG_REMOVE)
    {
        offset = item->hdr.origin_off;
        memset(&hdr, 0, ITEM_HEADER_SIZE);
        if (raw_read(offset, &hdr, ITEM_HEADER_SIZE) != RES_OK)
        {
            return RES_FLASH_READ_ERR;
        }

        if ((hdr.magic != ITEM_MAGIC_NUM) ||
            (hdr.state != ITEM_STATE_NORMAL) ||
            (hdr.key_len != item->hdr.key_len))
        {
            return RES_OK;
        }

        origin_key = (char *)kv_al_malloc(hdr.key_len);
        if (!origin_key)
        {
            return RES_MALLOC_FAILED;
        }
        new_key = (char *)kv_al_malloc(hdr.key_len);
        if (!new_key)
        {
            kv_al_free(origin_key);
            return RES_MALLOC_FAILED;
        }

        raw_read(offset + ITEM_HEADER_SIZE, origin_key, hdr.key_len);
        raw_read(item->pos + ITEM_HEADER_SIZE, new_key, hdr.key_len);
        if (memcmp(origin_key, new_key, hdr.key_len) != 0)
        {
            kv_al_free(origin_key);
            kv_al_free(new_key);
            return RES_OK;
        }

        kv_al_free(origin_key);
        kv_al_free(new_key);
    }
    else
    {
        return RES_INVALID_PARAM;
    }

    if ((ret = kv_state_set(offset, ITEM_STATE_DELETE)) != RES_OK)
    {
        return ret;
    }

    i = offset >> BLK_BITS;
    if (g_kv_mgr.block_info[i].state == BLK_STATE_USED)
    {
        if ((ret = kv_state_set((offset & BLK_OFF_MASK), BLK_STATE_DIRTY)) != RES_OK)
        {
            return ret;
        }
        g_kv_mgr.block_info[i].state = BLK_STATE_DIRTY;
    }

    return ret;
}

/*the function to be invoked while polling the used block*/
typedef int (*item_func)(kv_item_t *item, const char *key);

static int __item_recovery_cb(kv_item_t *item, const char *key)
{
    char *p = (char *)kv_al_malloc(item->len);
    if (!p)
    {
        return RES_MALLOC_FAILED;
    }

    if (raw_read(item->pos + ITEM_HEADER_SIZE, p, item->len) != RES_OK)
    {
        kv_al_free(p);
        return RES_FLASH_READ_ERR;
    }

    if (item->hdr.crc == utils_crc8((uint8_t *)p, item->len))
    {
        if ((item->hdr.origin_off != 0) && (item->pos != item->hdr.origin_off))
        {
            kv_item_del(item, KV_ORIG_REMOVE);
        }
    }
    else
    {
        kv_item_del(item, KV_SELF_REMOVE);
    }

    kv_al_free(p);
    return RES_CONT;
}

static int __item_find_cb(kv_item_t *item, const char *key)
{
    if (item->hdr.key_len != strlen(key))
    {
        return RES_CONT;
    }

    item->store = (char *)kv_al_malloc(item->hdr.key_len + item->hdr.val_len);
    if (!item->store)
    {
        return RES_MALLOC_FAILED;
    }

    if (raw_read(item->pos + ITEM_HEADER_SIZE, item->store, item->len) != RES_OK)
    {
        return RES_FLASH_READ_ERR;
    }

    if (memcmp(item->store, key, strlen(key)) == 0)
    {
        return RES_OK;
    }

    return RES_CONT;
}

static int __item_gc_cb(kv_item_t *item, const char *key)
{
    char *p;
    int ret;
    uint16_t len;
    uint8_t index;

    len = (ITEM_HEADER_SIZE + item->len + ~KV_ALIGN_MASK) & KV_ALIGN_MASK;
    p = (char *)kv_al_malloc(len);
    if (!p)
    {
        return RES_MALLOC_FAILED;
    }

    if (raw_read(item->pos, p, len) != RES_OK)
    {
        ret = RES_FLASH_READ_ERR;
        goto err;
    }

    if (raw_write(g_kv_mgr.write_pos, p, len) != RES_OK)
    {
        ret = RES_FLASH_WRITE_ERR;
        goto err;
    }

    g_kv_mgr.write_pos += len;
    index = (g_kv_mgr.write_pos) >> BLK_BITS;
    g_kv_mgr.block_info[index].space -= len;

    ret = RES_CONT;

err:
    kv_al_free(p);
    return ret;
}

static int __item_del_by_prefix_cb(kv_item_t *item, const char *prefix)
{
    char *key = NULL;
    if (item->hdr.key_len < strlen(prefix))
        return RES_CONT;

    key = (char *)kv_al_malloc(item->hdr.key_len + 1);
    if (!key)
        return RES_MALLOC_FAILED;

    memset(key, 0, item->hdr.key_len + 1);
    raw_read(item->pos + ITEM_HEADER_SIZE, key, item->hdr.key_len);

    if (strncmp(key, prefix, strlen(prefix)) == 0)
    {
        kv_item_del(item, KV_SELF_REMOVE);
    }

    kv_al_free(key);
    return RES_CONT;
}

static int __item_del_all_cb(kv_item_t *item, const char *key)
{
    if (item->hdr.key_len > 0)
    {
        kv_item_del(item, KV_SELF_REMOVE);
    }

    return RES_CONT;
}

static kv_item_t *kv_item_traverse(item_func func, uint8_t blk_index, const char *key)
{
    kv_item_t *item;
    item_hdr_t *hdr;
    uint16_t pos = (blk_index << BLK_BITS) + BLK_HEADER_SIZE;
    uint16_t end = (blk_index << BLK_BITS) + BLK_SIZE;
    uint16_t len = 0;
    int ret;

    do
    {
        item = (kv_item_t *)kv_al_malloc(sizeof(kv_item_t));
        if (!item)
        {
            return NULL;
        }
        memset(item, 0, sizeof(kv_item_t));
        hdr = &(item->hdr);

        if (raw_read(pos, hdr, ITEM_HEADER_SIZE) != RES_OK)
        {
            kv_item_free(item);
            return NULL;
        }

        if (hdr->magic != ITEM_MAGIC_NUM)
        {
            if ((hdr->magic == 0xFF) && (hdr->state == 0xFF))
            {
                kv_item_free(item);
                break;
            }
            hdr->val_len = 0xFFFF;
        }

        if (hdr->val_len > ITEM_MAX_VAL_LEN || hdr->key_len > ITEM_MAX_KEY_LEN ||
            hdr->val_len == 0 || hdr->key_len == 0)
        {
            pos += ITEM_HEADER_SIZE;
            kv_item_free(item);
            if (g_kv_mgr.block_info[blk_index].state == BLK_STATE_USED)
            {
                kv_state_set((blk_index << BLK_BITS), BLK_STATE_DIRTY);
                g_kv_mgr.block_info[blk_index].state = BLK_STATE_DIRTY;
            }
            continue;
        }

        len = (ITEM_HEADER_SIZE + hdr->key_len + hdr->val_len + ~KV_ALIGN_MASK) & KV_ALIGN_MASK;

        if (hdr->state == ITEM_STATE_NORMAL)
        {
            item->pos = pos;
            item->len = hdr->key_len + hdr->val_len;
            ret = func(item, key);
            if (ret == RES_OK)
            {
                return item;
            }
            else if (ret != RES_CONT)
            {
                kv_item_free(item);
                return NULL;
            }
        }
        else
        {
            if (g_kv_mgr.block_info[blk_index].state == BLK_STATE_USED)
            {
                kv_state_set((blk_index << BLK_BITS), BLK_STATE_DIRTY);
                g_kv_mgr.block_info[blk_index].state = BLK_STATE_DIRTY;
            }
        }

        kv_item_free(item);
        pos += len;
    } while (end > (pos + ITEM_HEADER_SIZE));

    g_kv_mgr.block_info[blk_index].space = (end > pos) ? (end - pos) : ITEM_HEADER_SIZE;

    return NULL;
}

static kv_item_t *kv_item_get(const char *key)
{
    kv_item_t *item;
    uint8_t i;

    for (i = 0; i < BLK_NUMS; i++)
    {
        if (g_kv_mgr.block_info[i].state != BLK_STATE_CLEAN)
        {
            item = kv_item_traverse(__item_find_cb, i, key);
            if (item)
            {
                return item;
            }
        }
    }

    return NULL;
}

static int kv_item_store(const char *key, const void *val, int len, uint16_t origin_off)
{
    kv_storeage_t store;
    item_hdr_t hdr;
    char *p;
    uint16_t pos;
    uint8_t index;

    hdr.magic = ITEM_MAGIC_NUM;
    hdr.state = ITEM_STATE_NORMAL;
    hdr.key_len = strlen(key);
    hdr.val_len = len;
    hdr.origin_off = origin_off;

    store.len = (ITEM_HEADER_SIZE + hdr.key_len + hdr.val_len + ~KV_ALIGN_MASK) & KV_ALIGN_MASK;

    store.p = (char *)kv_al_malloc(store.len);
    if (!store.p)
    {
        return RES_MALLOC_FAILED;
    }

    memset(store.p, 0, store.len);
    p = store.p + ITEM_HEADER_SIZE;
    memcpy(p, key, hdr.key_len);
    p += hdr.key_len;
    memcpy(p, val, hdr.val_len);
    p -= hdr.key_len;
    hdr.crc = utils_crc8((uint8_t *)p, hdr.key_len + hdr.val_len);
    memcpy(store.p, &hdr, ITEM_HEADER_SIZE);

    pos = kv_item_calc_pos(store.len);
    if (pos > 0)
    {
        store.ret = raw_write(pos, store.p, store.len);
        if (store.ret == RES_OK)
        {
            g_kv_mgr.write_pos = pos + store.len;
            index = g_kv_mgr.write_pos >> BLK_BITS;
            g_kv_mgr.block_info[index].space -= store.len;
        }
    }
    else
    {
        store.ret = RES_NO_SPACE;
    }

    if (store.p)
    {
        kv_al_free(store.p);
    }
    return store.ret;
}

static int kv_item_update(kv_item_t *item, const char *key, const void *val, int len)
{
    int ret;

    if (item->hdr.val_len == len)
    {
        if (!memcmp(item->store + item->hdr.key_len, val, len))
        {
            return RES_OK;
        }
    }

    ret = kv_item_store(key, val, len, item->pos);
    if (ret != RES_OK)
    {
        return ret;
    }

    ret = kv_item_del(item, KV_SELF_REMOVE);

    return ret;
}

static int kv_init(void)
{
    block_hdr_t hdr;
    int ret, nums = 0;
    uint8_t i, next;
    uint8_t unclean[BLK_NUMS] = {0};

    for (i = 0; i < BLK_NUMS; i++)
    {
        memset(&hdr, 0, sizeof(block_hdr_t));
        raw_read((i << BLK_BITS), &hdr, BLK_HEADER_SIZE);
        if (hdr.magic == BLK_MAGIC_NUM)
        {
            if (INVALID_BLK_STATE(hdr.state))
            {
                if ((ret = kv_block_format(i)) != RES_OK)
                {
                    return ret;
                }
                else
                {
                    continue;
                }
            }

            g_kv_mgr.block_info[i].state = hdr.state;
            kv_item_traverse(__item_recovery_cb, i, NULL);
            if (hdr.state == BLK_STATE_CLEAN)
            {
                if (g_kv_mgr.block_info[i].space != (BLK_SIZE - BLK_HEADER_SIZE))
                {
                    unclean[nums] = i;
                    nums++;
                }
                else
                {
                    (g_kv_mgr.clean_blk_nums)++;
                }
            }
        }
        else
        {
            if ((ret = kv_block_format(i)) != RES_OK)
            {
                return ret;
            }
        }
    }

    while (nums > 0)
    {
        i = unclean[nums - 1];
        if (g_kv_mgr.clean_blk_nums >= KV_GC_RESERVED)
        {
            if ((ret = kv_state_set((i << BLK_BITS), BLK_STATE_DIRTY)) != RES_OK)
            {
                return ret;
            }
            g_kv_mgr.block_info[i].state = BLK_STATE_DIRTY;
        }
        else
        {
            if ((ret = kv_block_format(i)) != RES_OK)
            {
                return ret;
            }
        }
        nums--;
    }

    if (g_kv_mgr.clean_blk_nums == 0)
    {
        if ((ret = kv_block_format(0)) != RES_OK)
        {
            return ret;
        }
    }

    if (g_kv_mgr.clean_blk_nums == BLK_NUMS)
    {
        g_kv_mgr.write_pos = BLK_HEADER_SIZE;
        if (!kv_state_set((g_kv_mgr.write_pos & BLK_OFF_MASK), BLK_STATE_USED))
        {
            g_kv_mgr.block_info[0].state = BLK_STATE_USED;
            (g_kv_mgr.clean_blk_nums)--;
        }
    }
    else
    {
        for (i = 0; i < BLK_NUMS; i++)
        {
            if ((g_kv_mgr.block_info[i].state == BLK_STATE_USED) ||
                (g_kv_mgr.block_info[i].state == BLK_STATE_DIRTY))
            {
                next = ((i + 1) == BLK_NUMS) ? 0 : (i + 1);
                if (g_kv_mgr.block_info[next].state == BLK_STATE_CLEAN)
                {
                    g_kv_mgr.write_pos = (i << BLK_BITS) + BLK_SIZE - g_kv_mgr.block_info[i].space;
                    break;
                }
            }
        }
    }

    return RES_OK;
}

static void *aos_kv_gc(void *arg)
{
    uint8_t i;
    uint8_t gc_index;
    uint8_t gc_copy = 0;
    uint16_t origin_pos;

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_lock(g_kv_mgr.kv_mutex);
    }

    origin_pos = g_kv_mgr.write_pos;
    if (g_kv_mgr.clean_blk_nums == 0)
    {
        goto exit;
    }

    for (gc_index = 0; gc_index < BLK_NUMS; gc_index++)
    {
        if (g_kv_mgr.block_info[gc_index].state == BLK_STATE_CLEAN)
        {
            g_kv_mgr.write_pos = (gc_index << BLK_BITS) + BLK_HEADER_SIZE;
            break;
        }
    }

    if (gc_index == BLK_NUMS)
    {
        goto exit;
    }

    i = (origin_pos >> BLK_BITS) + 1;
    while (1)
    {
        if (i == BLK_NUMS)
        {
            i = 0;
        }

        if (g_kv_mgr.block_info[i].state == BLK_STATE_DIRTY)
        {
            kv_item_traverse(__item_gc_cb, i, NULL);

            gc_copy = 1;
            if (kv_block_format(i) != RES_OK)
            {
                goto exit;
            }

            kv_state_set((g_kv_mgr.write_pos & BLK_OFF_MASK), BLK_STATE_USED);
            g_kv_mgr.block_info[gc_index].state = BLK_STATE_USED;
            (g_kv_mgr.clean_blk_nums)--;
            break;
        }
        if (i == (origin_pos >> BLK_BITS))
        {
            break;
        }
        i++;
    }

    if (gc_copy == 0)
    {
        g_kv_mgr.write_pos = origin_pos;
    }

exit:
    g_kv_mgr.gc_triggered = 0;
    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_unlock(g_kv_mgr.kv_mutex);
    }

    aiot_al_thread_delete(gc_thread);

    return NULL;
}

static int aiot_kv_del(const char *key)
{
    kv_item_t *item;
    int ret;

#ifndef BUILD_AOS
#ifdef COMPATIBLE_LK_KV
    if (g_kv_mgr.is_lk_kv == 1)
    {
        return aiot_al_lk_kv_del(key);
    }
#endif
#endif

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_lock(g_kv_mgr.kv_mutex);
    }

    item = kv_item_get(key);
    if (!item)
    {
        if (g_kv_mgr.kv_initialize)
        {
            aiot_al_mutex_unlock(g_kv_mgr.kv_mutex);
        }
        return RES_ITEM_NOT_FOUND;
    }

    ret = kv_item_del(item, KV_SELF_REMOVE);
    kv_item_free(item);
    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_unlock(g_kv_mgr.kv_mutex);
    }
    return ret;
}

static int aiot_kv_del_by_prefix(const char *prefix)
{
    int i = 0;

#ifndef BUILD_AOS
#ifdef COMPATIBLE_LK_KV
    if (g_kv_mgr.is_lk_kv == 1)
    {
        return 1;
    }
#endif
#endif

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_lock(g_kv_mgr.kv_mutex);
    }

    for (i = 0; i < BLK_NUMS; i++)
    {
        kv_item_traverse(__item_del_by_prefix_cb, i, prefix);
    }

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_unlock(g_kv_mgr.kv_mutex);
    }

    return RES_OK;
}

int aiot_kv_del_all(void)
{
    int i = 0;

#ifndef BUILD_AOS
#ifdef COMPATIBLE_LK_KV
    if (g_kv_mgr.is_lk_kv == 1)
    {
        return 1;
    }
#endif
#endif

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_lock(g_kv_mgr.kv_mutex);
    }

    for (i = 0; i < BLK_NUMS; i++)
    {
        kv_item_traverse(__item_del_all_cb, i, NULL);
    }
    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_unlock(g_kv_mgr.kv_mutex);
    }
    return RES_OK;
}

static int aiot_kv_set(const char *key, const void *val, int len, int sync)
{
    kv_item_t *item;
    int ret;
    if (!key || !val || len <= 0 || strlen(key) > ITEM_MAX_KEY_LEN || len > ITEM_MAX_VAL_LEN)
    {
        return RES_INVALID_PARAM;
    }

#ifndef BUILD_AOS
#ifdef COMPATIBLE_LK_KV
    if (g_kv_mgr.is_lk_kv == 1)
    {
        return aiot_al_lk_kv_set(key, val, len);
    }
#endif
#endif

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_lock(g_kv_mgr.kv_mutex);
    }

    item = kv_item_get(key);
    if (item)
    {
        ret = kv_item_update(item, key, val, len);
        kv_item_free(item);
    }
    else
    {
        ret = kv_item_store(key, val, len, 0);
    }

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_unlock(g_kv_mgr.kv_mutex);
    }

    return ret;
}

static int aiot_kv_get(const char *key, void *buffer, int *buffer_len)
{
    kv_item_t *item = NULL;

    if (!key || !buffer || !buffer_len || *buffer_len <= 0)
    {
        return RES_INVALID_PARAM;
    }

#ifndef BUILD_AOS
#ifdef COMPATIBLE_LK_KV
    if (g_kv_mgr.is_lk_kv == 1)
    {
        return aiot_al_lk_kv_get(key, buffer, buffer_len);
    }
#endif
#endif

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_lock(g_kv_mgr.kv_mutex);
    }
    item = kv_item_get(key);

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_unlock(g_kv_mgr.kv_mutex);
    }

    if (!item)
    {
        return RES_ITEM_NOT_FOUND;
    }

    if (*buffer_len < item->hdr.val_len)
    {
        *buffer_len = item->hdr.val_len;
        kv_item_free(item);
        return RES_NO_SPACE;
    }
    else
    {
        memcpy(buffer, (item->store + item->hdr.key_len), item->hdr.val_len);
        *buffer_len = item->hdr.val_len;
    }

    kv_item_free(item);
    return RES_OK;
}

/* CLI Support */
#ifdef CONFIG_AOS_CLI
#ifdef SUPPORT_KV_LIST_CMD
static int _is_printable_string(const char *string, unsigned int len)
{
    while (len)
    {
        if (!isprint(string[len - 1]))
        {
            return 0;
        }
        len--;
    }
    return 1;
}

static int __item_print_cb(kv_item_t *item, const char *key)
{
    int index = 0;
    char *p_key = NULL;
    char *p_val = NULL;
    p_key = (char *)kv_al_malloc(item->hdr.key_len + 1);
    if (!p_key)
    {
        return RES_MALLOC_FAILED;
    }
    memset(p_key, 0, item->hdr.key_len + 1);
    raw_read(item->pos + ITEM_HEADER_SIZE, p_key, item->hdr.key_len);

    p_val = (char *)kv_al_malloc(item->hdr.val_len + 1);
    if (!p_val)
    {
        kv_al_free(p_key);
        return RES_MALLOC_FAILED;
    }
    memset(p_val, 0, item->hdr.val_len + 1);
    raw_read(item->pos + ITEM_HEADER_SIZE + item->hdr.key_len, p_val, item->hdr.val_len);

    if (_is_printable_string(p_val, item->hdr.val_len))
    {
        aos_cli_printf("\r\nkey:%s vaule[%d]:%s\r\n", p_key, item->hdr.val_len, p_val);
    }
    else
    {
        aos_cli_printf("\r\nkey:%s binary value[%d] is\r\n", p_key, item->hdr.val_len);

        for (index = 0; index < item->hdr.val_len; index++)
        {
            aos_cli_printf("0x%02x ", (unsigned char)p_val[index]);
            if ((index + 1) % 16 == 0)
            {
                aos_cli_printf("\r\n");
            }
        }
    }

    kv_al_free(p_key);
    kv_al_free(p_val);

    return RES_CONT;
}
#endif

static int _kv_getx_cmd(const char *key, kv_get_type_e get_type)
{
    int ret = 0;
    int index = 0;
    int len = ITEM_MAX_LEN;
    unsigned char *buffer = NULL;
    kv_item_t *item = NULL;

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_lock(g_kv_mgr.kv_mutex);
    }

    item = kv_item_get(key);

    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_unlock(g_kv_mgr.kv_mutex);
    }

    if (!item)
    {
        aos_cli_printf("key:%s not found\r\n", key);
        return -1;
    }

    len = item->hdr.val_len + 1;

    buffer = kv_al_malloc(len + 1);
    kv_item_free(item);

    if (!buffer)
    {
        aos_cli_printf("no mem\r\n");
        return -1;
    }

    memset(buffer, 0, len);

    ret = aiot_kv_get(key, buffer, &len);
    if (ret != 0)
    {
        aos_cli_printf("cli: no paired kv\r\n");
    }
    else
    {
        switch (get_type)
        {
        case KV_GET_TYPE_STRING:
        {
            aos_cli_printf("value is %s\r\n", buffer);
        }
        break;
        case KV_GET_TYPE_BINARY:
        {
            aos_cli_printf("\r\nkv key:%s binary value is\r\n", key);

            for (index = 0; index < len; index++)
            {
                aos_cli_printf("0x%02x ", buffer[index]);
                if ((index + 1) % 16 == 0)
                {
                    aos_cli_printf("\r\n");
                }
            }
        }
        break;
        default:
            break;
        }
    }

    if (buffer)
    {
        kv_al_free(buffer);
    }

    return 0;
}

#ifndef BUILD_AOS
#ifdef COMPATIBLE_LK_KV
static void handle_lk_kv_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";
    int ret = 0;
    int vaule_len = 0;
    unsigned char *buffer = NULL;

    if (strcmp(rtype, "set") == 0)
    {
        if (argc != 4)
        {
            return;
        }
        ret = aiot_kv_set(argv[2], argv[3], strlen(argv[3]), 1);
        if (ret != 0)
        {
            aos_cli_printf("cli set kv failed\r\n");
        }
    }
    else if (strcmp(rtype, "get") == 0)
    {
        if (argc != 3)
        {
            return;
        }

        vaule_len = aiot_al_lk_kv_get_value_len();
        if (vaule_len > 0)
        {
            buffer = kv_al_malloc(vaule_len);
            if (!buffer)
            {
                aos_cli_printf("no mem\r\n");
                return -1;
            }

            memset(buffer, 0, vaule_len);

            ret = aiot_kv_get(argv[2], buffer, &vaule_len);
            if (ret != 0)
            {
                aos_cli_printf("cli: no paired kv\r\n");
            }

            kv_al_free(buffer);
        }
    }
    else if (strcmp(rtype, "del") == 0)
    {
        if (argc != 3)
        {
            return;
        }
        ret = aiot_kv_del(argv[2]);
        if (ret != 0)
        {
            aos_cli_printf("cli kv del failed\r\n");
        }
    }
    else
    {
        aos_cli_printf("\"kv %s\" not support!\r\n", rtype);
    }

    return;
}
#endif
#endif

static void handle_kv_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";
    int ret = 0;

#ifndef BUILD_AOS
#ifdef COMPATIBLE_LK_KV
    if (g_kv_mgr.is_lk_kv == 1)
    {
        return handle_lk_kv_cmd(pwbuf, blen, argc, argv);
    }
#endif
#endif

#ifdef SUPPORT_KV_LIST_CMD
    int i = 0;
#endif

    if (strcmp(rtype, "set") == 0)
    {
        if (argc != 4)
        {
            return;
        }
        ret = aiot_kv_set(argv[2], argv[3], strlen(argv[3]), 1);
        if (ret != 0)
        {
            aos_cli_printf("cli set kv failed\r\n");
        }
    }
    else if (strcmp(rtype, "get") == 0)
    {
        if (argc != 3)
        {
            return;
        }

        _kv_getx_cmd(argv[2], KV_GET_TYPE_STRING);
    }
    else if (strcmp(rtype, "getb") == 0)
    {
        if (argc != 3)
        {
            return;
        }

        _kv_getx_cmd(argv[2], KV_GET_TYPE_BINARY);
    }
    else if (strcmp(rtype, "del") == 0)
    {
        if (argc != 3)
        {
            return;
        }
        ret = aiot_kv_del(argv[2]);
        if (ret != 0)
        {
            aos_cli_printf("cli kv del failed\r\n");
        }
#ifdef SUPPORT_KV_LIST_CMD
    }
    else if (strcmp(rtype, "list") == 0)
    {
        for (i = 0; i < BLK_NUMS; i++)
        {
            kv_item_traverse(__item_print_cb, i, NULL);
        }
#endif
    }
    else if (strcmp(rtype, "clear") == 0)
    {
        aiot_kv_del_all();
    }
    else
    {
        aos_cli_printf("\"kv %s\" not support!\r\n", rtype);
    }

    return;
}

static struct cli_command ncmd = {
    "kv",
#ifdef SUPPORT_KV_LIST_CMD
    "kv [set key value | get key | getb key | del key | list | clear]",
#else
    "kv [set key value | get key | getb key | del key | clear]",
#endif
    handle_kv_cmd};
#endif

static void kv_version_init(void)
{
    int8_t kv_version[KV_VERSION_MAX_LEN];
    int kv_version_len = KV_VERSION_MAX_LEN;

    memset(kv_version, 0, KV_VERSION_MAX_LEN);

    if (aiot_kv_get(KV_VERSION_KEY, kv_version, &kv_version_len) != 0)
    {
        aiot_kv_set(KV_VERSION_KEY, KV_VERSION, strlen(KV_VERSION) + 1, 0);
    }
    else
    {
        log_info(KV_MODULE_NAME, "kv version:%s", kv_version);
    }
}

int aiot_kv_init(void)
{
    uint8_t blk_index;
    int ret;

    if (g_kv_mgr.kv_initialize)
    {
        return RES_OK;
    }

    if (BLK_NUMS <= KV_GC_RESERVED)
    {
        return -EINVAL;
    }

    memset(&g_kv_mgr, 0, sizeof(g_kv_mgr));

    g_kv_mgr.kv_mutex = aiot_al_mutex_init();
    if (g_kv_mgr.kv_mutex == NULL)
    {
        return RES_INIT_FAILED;
    }

#ifdef CONFIG_AOS_CLI
    aos_cli_register_command(&ncmd);
#endif

#ifndef BUILD_AOS
#ifdef COMPATIBLE_LK_KV
    if (aiot_al_is_lk_kv())
    {
        g_kv_mgr.is_lk_kv = 1;
        g_kv_mgr.kv_initialize = 1;
        kv_version_init();

        return RES_OK;
    }
#endif
#endif

    if ((ret = kv_init()) != RES_OK)
    {
        return ret;
    }

    g_kv_mgr.kv_initialize = 1;

    blk_index = (g_kv_mgr.write_pos >> BLK_BITS);
    if (((g_kv_mgr.block_info[blk_index].space) < ITEM_MAX_LEN) &&
        (g_kv_mgr.clean_blk_nums < KV_GC_RESERVED + 1))
    {
        trigger_gc();
    }

    kv_version_init();

    return RES_OK;
}

static void aiot_kv_deinit(void)
{
    if (g_kv_mgr.kv_initialize)
    {
        aiot_al_mutex_deinit(&(g_kv_mgr.kv_mutex));
    }

    g_kv_mgr.kv_initialize = 0;

#ifndef BUILD_AOS
#ifdef COMPATIBLE_LK_KV
    if (aiot_al_is_lk_kv())
    {
        g_kv_mgr.is_lk_kv = 0;
    }
#endif
#endif
}

int aos_kv_init(void)
{
    return aiot_kv_init();
}

int aos_kv_set(const char *key, const void *value, int len, int sync)
{
    return aiot_kv_set(key, value, len, sync);
}

int aos_kv_get(const char *key, void *buffer, int *buffer_len)
{
    return aiot_kv_get(key, buffer, buffer_len);
}

int aos_kv_del(const char *key)
{
    return aiot_kv_del(key);
}

int aos_kv_del_by_prefix(const char *prefix)
{
    return aiot_kv_del_by_prefix(prefix);
}
