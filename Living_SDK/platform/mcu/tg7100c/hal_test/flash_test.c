/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <aos/kernel.h>
#include <aos/aos.h>
#include "hal/soc/flash.h"
#include "hal_test_entry.h"

#define TAG "TEST_HAL_EFLASH"
extern int hal_flash_init(hal_partition_t part);

#define HAL_TEST_PARTITION HAL_PARTITION_PARAMETER_4

static void aos_hal_flash_info_get(){
    int32_t ret = -1;
    hal_logic_partition_t partition_info;
    //hal_flash_init();

    ret = hal_flash_info_get(HAL_TEST_PARTITION,&partition_info);
    //partition_info = hal_flash_get_info(HAL_TEST_PARTITION);
    switch (partition_info.partition_owner){
        case 0:
            printf("partition ower is: HAL_FLASH_EMBEDDED\r\n");
            break;
        case 1:
            printf("partition ower is: HAL_FLASH_SPI\r\n");
            break;
        case 2:
            printf("partition ower is: HAL_FLASH_QSPI\r\n");
            break;
        case 3:
            printf("partition ower is: HAL_FLASH_MAX\r\n");
        case 4:
            printf("partition ower is: HAL_FLASH_NONE\r\n");
            break;
        default:
            break;
    }
    printf("partitiondescription is: %s\r\n",partition_info.partition_description);
    printf("partition start addr is: %x\r\n",(unsigned int)partition_info.partition_start_addr);
    printf("partition length is: %d\r\n",(int)partition_info.partition_length);
    printf("partition op is: PAR_OPT_READ_EN & PAR_OPT_WRITE_EN\r\n");

    if (ret == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_erase_write(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    uint32_t off = 0;
    char buf[512] = {0};

    memset(buf,0,512);
    ret1 = hal_flash_erase_write(HAL_TEST_PARTITION,&off,buf,512);
    memset(buf,0,0);
    ret2 = hal_flash_erase_write(HAL_TEST_PARTITION,&off,buf,0);
    memset(buf,0,21);
    ret3 = hal_flash_erase_write(HAL_TEST_PARTITION,&off,buf,21);

    if (ret1 == 0 && ret2 == 0 && ret3 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_erase(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    int32_t ret4 = -1;
    
    //偏移量为0，擦除字节数为0
    ret1 = hal_flash_erase(HAL_TEST_PARTITION,0,0);

    //偏移量为0，擦除字节数为最大值
    ret2 = hal_flash_erase(HAL_TEST_PARTITION,0,512);

    //偏移量为23，擦除字节数为26
    ret3 = hal_flash_erase(HAL_TEST_PARTITION,23,26);

    //偏移量为最大值，擦除字节数为0
    ret4 = hal_flash_erase(HAL_TEST_PARTITION,511,0);

    if (ret1 == 0 && ret2 == 0 && ret3 == 0 && ret4 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_write(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    char     tx_buf[512] = {0};
    uint32_t off = 0;
    memset(tx_buf, 0xaa, 512);

    ret1 = hal_flash_write(HAL_TEST_PARTITION,&off,tx_buf,512);
    
    ret2 = hal_flash_write(HAL_TEST_PARTITION,&off,tx_buf,0);

    if (ret1 == 0 && ret2 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_read(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    int32_t ret4 = -1;
    int i;
    char     tx_buf[512] = {0};
    char     rx_buf[512] = {0};
    uint32_t off = 0;
    memset(tx_buf, 10, 512);

    hal_flash_erase(HAL_TEST_PARTITION,0,512);
    hal_flash_write(HAL_TEST_PARTITION,&off,tx_buf,512);
    //写入之后，off变更，需重新置0
    off = 0;
    ret1 = hal_flash_read(HAL_TEST_PARTITION,&off,rx_buf,512);
    for(i=0;i<512;i++){
        printf("tx_buf[%d] =%d,rx_buf[%d] =%d\r\n",i,tx_buf[i],i,rx_buf[i]);
    }
    if(memcmp(tx_buf, rx_buf, 512) == 0){
        ret2 = 0;
        LOGD(TAG,"data compare success!");
    } else {
  
        LOGD(TAG,"read data not equal write data!");
    }

    off = 0;
    memset(tx_buf, 11, 512);
    hal_flash_erase_write(HAL_TEST_PARTITION,&off,tx_buf,512);
    off = 0;
    ret3 = hal_flash_read(HAL_TEST_PARTITION,&off,rx_buf,512);
    for(i=0;i<512;i++){
        printf("tx_buf[%d] =%d,rx_buf[%d] =%d\r\n",i,tx_buf[i],i,rx_buf[i]);
    }
    if(memcmp(tx_buf, rx_buf, 512) == 0){
        ret4 = 0;
        LOGD(TAG,"data compare success!");
    } else {
        LOGD(TAG,"read data not equal write data!");
    }

    if (ret1 == 0 && ret2 == 0 && ret3 == 0 && ret4 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_erase_write_error(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    int32_t ret4 = -1;
    uint32_t off = 0;
    char buf1[513] = {0};

    ret1 = hal_flash_erase_write(HAL_TEST_PARTITION,&off,buf1,512+1);
    printf("ret = %d\r\n",(int)ret1);
    //擦除字节数超过最大
    ret2 = hal_flash_erase(HAL_TEST_PARTITION,0,512+1);
    printf("ret = %d\r\n",(int)ret2);
    //偏移量超过最大
    ret3 = hal_flash_erase(HAL_TEST_PARTITION,512,5);
    printf("ret = %d\r\n",(int)ret3);

    //写入数据为NULL
    ret4 = hal_flash_write(HAL_TEST_PARTITION,&off,NULL,512);
    printf("ret = %d\r\n",(int)ret4);

    if (ret1 != 0 && ret2 != 0 && ret3 != 0 && ret4 != 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

void test_hal_eflash(char *cmd, int type, char *data)
{
    //hal_logic_partition_t partition_info = {0};
    hal_logic_partition_t partition_info;

    hal_flash_init(HAL_TEST_PARTITION);
    hal_flash_info_get(HAL_TEST_PARTITION,&partition_info);
    //partition_info = hal_flash_get_info(HAL_TEST_PARTITION);
    if (strcmp((const char *)data, "'info_get'\0") == 0) {
        aos_hal_flash_info_get();
    }else if (strcmp((const char *)data, "'erase_write'\0") == 0) {
        aos_hal_flash_erase_write();
    }else if (strcmp((const char *)data, "'toerase'\0") == 0) {
        aos_hal_flash_erase();
    }else if (strcmp((const char *)data, "'towrite'\0") == 0) {
        aos_hal_flash_write();
    }else if (strcmp((const char *)data, "'toread'\0") == 0) {
        aos_hal_flash_read();
    }else if (strcmp((const char *)data, "'error'\0") == 0) {
        aos_hal_flash_erase_write_error();
    } else if (strcmp((const char *)data, "TEST") == 0) {
        aos_hal_flash_info_get();
        aos_hal_flash_erase_write();
        aos_hal_flash_erase();
        aos_hal_flash_write();
        aos_hal_flash_read();
        aos_hal_flash_erase_write_error();
    }
}
