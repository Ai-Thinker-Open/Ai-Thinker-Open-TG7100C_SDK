#include <platform/platform_stdlib.h>
#include <osdep_service.h>
#include <rtl8710b_ota.h>
#include <flash_api.h>
#include <device_lock.h>
#include "hal/soc/flash.h"


typedef enum {
    OTA_FINISH,
    OTA_BREAKPOINT
} OTA_RES_TYPE_E;

typedef struct  {
    unsigned int dst_adr;
    unsigned int src_adr;
    unsigned int len;
    unsigned short crc;
    unsigned int  upg_flag;
    unsigned char boot_count;
    unsigned int  splict_size;
    int off_bp;   /*Break point offset*/
    OTA_RES_TYPE_E  res_type; /*result type: OTA_FINISH, OTA_BREAKPOINT*/
    unsigned short param_crc; /*Parameter crc*/
} __attribute__((packed)) ota_boot_param_t;

typedef struct ota_hal_module_s {
    int (*init)(void *something);
    int (*write)(int *off_set, char *in_buf , int in_buf_len);
    int (*read)(int *off_set, char *out_buf , int out_buf_len);
    int (*boot)(void *something);
    int (*rollback)(void *something);
    const char *(*version)(unsigned char dev_type);
}ota_hal_module_t;

static u32 alinknewImg2Addr = 0xFFFFFFFF;

char *HeadBuffer = NULL;
#define OTA_HEADER_BUF_SIZE  1024
int alink_size = 0;
u8 alink_signature[9] = {0};
extern const update_file_img_id OtaImgId[2];
uint32_t alink_ota_target_index = OTA_INDEX_2;
update_ota_target_hdr OtaTargetHdr;

static int aliota_total_len = 0, aliota_flag = 1, aliota_count = 0;
static uint32_t aliota_address;
static int aliota_RemainBytes = 0;
static int aliota_ota_exit = 0;
static int aliota_ota_flag = 1;
static unsigned long aliota_tick1, aliota_tick2;
static u32 aliota_OtaFg = 0;
static u32 aliota_SigCnt = 0;
static int aliota_end_sig = 0;

hal_logic_partition_t *partition_info = NULL;
#ifndef OTA2_DEFAULT_ADDR
#define OTA2_DEFAULT_ADDR   (0x08100000)
#endif

int check_ota_index()
{
	uint32_t ota_index = -1;
	/* check OTA index we should update */
	if (ota_get_cur_index() == OTA_INDEX_1) {
		ota_index = OTA_INDEX_2;
	} else {
		ota_index = OTA_INDEX_1;
	}
        printf("OTA idx:%d upgrade.\n",ota_index);
	return ota_index;
}

bool prepare_ota_address(u32 ota_target_index, u32 * new_addr)
{
	IMAGE_HEADER *OTA1Hdr = NULL;
	uint32_t OTA1Len = 0;
	IMAGE_HEADER *FlashImgDataHdr = NULL;
	uint32_t ota2_addr;

	ota2_addr = HAL_READ32(SPI_FLASH_BASE, OFFSET_DATA);
	printf("ota2_addr = %x\n", ota2_addr);
	/*if the OTA2 address is not programmed in system data zone, the default OTA2
	address is used.	This operation is just used in the local OTA update demo. For the
	cloud OTA upgrade based on this demo, this operation may not be used.*/
	if(ota2_addr == 0xffffffff) {
		ota_write_ota2_addr(OTA2_DEFAULT_ADDR);
		ota2_addr = HAL_READ32(SPI_FLASH_BASE, OFFSET_DATA);
	}

	if((ota2_addr%4096) != 0) {
		printf(" ota addr not 4k aligned 0x%x", ota2_addr);
		goto error;
	}

	if(ota_target_index == OTA_INDEX_2) {
		/* OAT2 address should not in OTA1 image & should 4K alignment */
		OTA1Hdr = (IMAGE_HEADER *)(OTA1_ADDR);
		OTA1Len = OTA1Hdr->image_size;
		FlashImgDataHdr = (IMAGE_HEADER *)((u32)OTA1Hdr + OTA1Len + IMAGE_HEADER_LEN);
		if ((ota2_addr < ((u32)FlashImgDataHdr + FlashImgDataHdr->image_size + IMAGE_HEADER_LEN)) && ((ota2_addr & 0xfff) == 0)) {
			printf("\n\r[%s] illegal ota addr 0x%x", __FUNCTION__, ota2_addr);
			goto error;
		}
		*new_addr = ota2_addr;
	} else {
		*new_addr = OTA1_ADDR;
	}

	if(*new_addr == 0xFFFFFFFF) {
		printf("update addr invalid.\n");
		goto error;
	}
	printf("OTA addr: 0x%x\n", *new_addr);
	return TRUE;

error:
	return FALSE;
}

static int ota_init(void *something)
{
    int ret = 0;
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    uint32_t part = HAL_PARTITION_OTA_TEMP;
    uint32_t offset = 0;
    param->off_bp = 0;
    printf("ota_init off:0x%x \n", offset);
    alink_ota_target_index = check_ota_index();
    if(prepare_ota_address(alink_ota_target_index, &alinknewImg2Addr) != TRUE){
        printf("get addr failed\n");
        ret = -1;
        goto RTL8710BN_INIT_ERRO;
    }
    if(alink_ota_target_index == OTA_INDEX_1) {
        part = HAL_PARTITION_APPLICATION;
    }
    else if(alink_ota_target_index == OTA_INDEX_2) {
         part = HAL_PARTITION_OTA_TEMP;
    }
    else {
         printf("flash failed\n");
         ret = -1;
         goto RTL8710BN_INIT_ERRO;
    }
    partition_info = hal_flash_get_info(part);
    if(partition_info == NULL) {
        printf("flash info failed\n");
        ret = -1;
        goto RTL8710BN_INIT_ERRO;
    }
    /**dual image pack size + info head(asume 512bytes)**/
    if((partition_info->partition_length * 2 + 512) < param->len || param->len == 0) {
        ret = -17;
        goto RTL8710BN_INIT_ERRO;
    }
    if(offset == 0) {
          alink_size = 0;
          aliota_total_len = 0;
          aliota_flag = 1;
          aliota_count = 0;
          aliota_address = 0;
          aliota_RemainBytes = 0;
          aliota_ota_exit = 0;
          aliota_ota_flag = 1;
          aliota_tick1 = 0;
          aliota_tick2 = 0;
          aliota_OtaFg = 0;
          aliota_SigCnt = 0;
          aliota_end_sig = 0;
          /*-------------------erase flash space for new firmware--------------*/
          erase_ota_target_flash(alinknewImg2Addr, partition_info->partition_length);
          DBG_INFO_MSG_OFF(_DBG_SPI_FLASH_);
          HeadBuffer = rtw_malloc(OTA_HEADER_BUF_SIZE);
          if (HeadBuffer == NULL) {
              printf("malloc head failed\n");
              ret = -1;
              goto RTL8710BN_INIT_ERRO;
          }
    }
RTL8710BN_INIT_ERRO:
    printf("ota init over!\r\n");
    return ret;
}

static int ota_write(int* off_set,char *buffer,int len)
{
    char *buf;
    flash_t	flash;
    update_file_hdr OtaFileHdr;

    if (aliota_ota_flag) {
        aliota_tick1 = rtw_get_current_time();
        aliota_ota_flag = 0;
    }
    /*-----read 4 Dwords from server, get image header number and header length*/
    buf = HeadBuffer + aliota_total_len;
    aliota_total_len += len;
    if (aliota_total_len < 128) {
	    memcpy(buf, buffer, len);
	    aliota_count += len;
	    printf("copy len:%d \n",len);
	    return 0;
    } else if (aliota_total_len >= 128 && aliota_total_len <= OTA_HEADER_BUF_SIZE) {
	    if (aliota_flag == 1) {
		    buf = HeadBuffer;
		    memcpy(buf+aliota_count, buffer, aliota_total_len - aliota_count);
		    memcpy((u8*)(&OtaTargetHdr.FileHdr), buf, sizeof(OtaTargetHdr.FileHdr));
		    memcpy((u8*)(&OtaTargetHdr.FileImgHdr), buf+8, 8);
		    /*------acquire the desired Header info from buffer*/
		    buf = HeadBuffer;
		    u8 *TempBuf = NULL;
		    TempBuf = (u8 *)(&OtaImgId[alink_ota_target_index]);
		    printf("TempBuf = %s\n",TempBuf);
		    if (!get_ota_tartget_header(buf, aliota_total_len, &OtaTargetHdr, TempBuf)) {
			    printf("get header failed\n");
			    goto update_ota_exit;
		    }

		    if (alinknewImg2Addr == 0xFFFFFFFF) {
			    /*get new image addr and check new address validity*/
			    if (!get_ota_address(alink_ota_target_index, &alinknewImg2Addr, &OtaTargetHdr)) {
				    printf("get addr failed\n");
				    goto update_ota_exit;
			    }
		    } else {
			    /*Image size should not bigger than OTA_SIZE_MAX */
			    if(partition_info != NULL) {
				    if(OtaTargetHdr.FileImgHdr.ImgLen > partition_info->partition_length){
					    printf("illegal image len: 0x%x", OtaTargetHdr.FileImgHdr.ImgLen);
					    goto update_ota_exit;
				    }
			    }
		    }
		    /*get new image length from the firmware header*/
		    uint32_t NewImg2Len = 0, NewImg2BlkSize = 0;
		    NewImg2Len = OtaTargetHdr.FileImgHdr.ImgLen;
		    NewImg2BlkSize = ((NewImg2Len - 1)/4096) + 1;
		    /*the upgrade space should be masked, because the encrypt firmware is used 
			for checksum calculation*/
			OTF_Mask(1, (alinknewImg2Addr - SPI_FLASH_BASE), NewImg2BlkSize, 1);
			/* get OTA image and Write New Image to flash, skip the signature, 
			not write signature first for power down protection*/
			aliota_address = alinknewImg2Addr -SPI_FLASH_BASE + 8;
			printf("NewImg2Addr = %x\n", alinknewImg2Addr);
			aliota_RemainBytes = OtaTargetHdr.FileImgHdr.ImgLen - 8;
			aliota_flag = 0;
			if (HeadBuffer != NULL) {
				rtw_free(HeadBuffer);
			}
		}
	}

	/*skip the signature*/
	/*download the new firmware from server*/
	if(aliota_RemainBytes > 0) {
		if(aliota_total_len > OtaTargetHdr.FileImgHdr.Offset) {
			if(!aliota_OtaFg) {
				u32 Cnt = 0;
				/*reach the the desired image, the first packet process*/
				aliota_OtaFg = 1;
				Cnt = aliota_total_len -OtaTargetHdr.FileImgHdr.Offset;
				if(Cnt < 8) {
					aliota_SigCnt = Cnt;
				} else {
					aliota_SigCnt = 8;
				}
				memcpy(alink_signature, buffer + len -Cnt, aliota_SigCnt);
				if((aliota_SigCnt < 8) || (Cnt -8 == 0)) {
					return 0;
				}				
				device_mutex_lock(RT_DEV_LOCK_FLASH);
				if(flash_stream_write(&flash, aliota_address + alink_size, Cnt -8, buffer + (len -Cnt + 8)  ) < 0){
					printf("Write failed\r\n");
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					goto update_ota_exit;
				}
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				alink_size += (Cnt - 8);
				aliota_RemainBytes -= alink_size;
			} else {					
				/*normal packet process*/
				if(aliota_SigCnt < 8) {
					if(len < (8 -aliota_SigCnt)) {
						memcpy(alink_signature + aliota_SigCnt, buffer, len);
						aliota_SigCnt += len;
						return 0;
					} else {
						memcpy(alink_signature + aliota_SigCnt, buffer, (8 -aliota_SigCnt));
						aliota_end_sig = 8 -aliota_SigCnt;
						len -= (8 -aliota_SigCnt) ;
						aliota_SigCnt = 8;
						if(!len) {
							return 0;
						}
						aliota_RemainBytes -= len;
						if (aliota_RemainBytes <= 0) {
							len = len - (-aliota_RemainBytes);
						}
						device_mutex_lock(RT_DEV_LOCK_FLASH);
						if (flash_stream_write(&flash, aliota_address+alink_size, len, buffer+aliota_end_sig) < 0){
							printf("Write sector failed\r\n");
							device_mutex_unlock(RT_DEV_LOCK_FLASH);
							goto update_ota_exit;
						}
						device_mutex_unlock(RT_DEV_LOCK_FLASH);
						alink_size += len;
						return 0;
					}
				}
				aliota_RemainBytes -= len;
				int end_ota = 0;
				if(aliota_RemainBytes <= 0) {
					len = len - (-aliota_RemainBytes);
					end_ota = 1;
				}
				device_mutex_lock(RT_DEV_LOCK_FLASH);
				if(flash_stream_write(&flash, aliota_address + alink_size, len, buffer) < 0){
					printf("Write sec failed\r\n");
					device_mutex_unlock(RT_DEV_LOCK_FLASH);
					goto update_ota_exit;
				}
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				alink_size += len;
				if (end_ota) {
					printf("size = %x\n", alink_size);
				}
				aliota_tick2 = rtw_get_current_time();
				if (aliota_tick2 - aliota_tick1 > 2000) {
					printf("Download OTA file: %d B, RemainBytes = %d\n", (alink_size), aliota_RemainBytes);
					aliota_ota_flag = 1;
				}
			}
		}
	}

    return 0;
update_ota_exit:
	if (HeadBuffer != NULL) {
		rtw_free(HeadBuffer);
	}
        aliota_ota_exit = 1;
	printf("Update task exit");
	return -1;
}

static int ota_boot(void *something)
{
    ota_boot_param_t *param = (ota_boot_param_t *)something;
    if (param == NULL) {
        return -1;
    }
    if (param->res_type == OTA_FINISH) {
        flash_t flash;
        printf("size = %d, ImgLen = %d sign: %s\n", alink_size, OtaTargetHdr.FileImgHdr.ImgLen, alink_signature);
         /*------------- verify checksum and update signature-----------------*/
        if(verify_ota_checksum(alinknewImg2Addr, alink_size, alink_signature, &OtaTargetHdr)){
                if(!change_ota_signature(alinknewImg2Addr, alink_signature, alink_ota_target_index)) {
                        printf("\n%s: change signature failed\n");
                        return -1;
                }
                return 0;
        } else {
                /*if checksum error, clear the signature zone which has been 
                written in flash in case of boot from the wrong firmware*/
                device_mutex_lock(RT_DEV_LOCK_FLASH);
                flash_erase_sector(&flash, alinknewImg2Addr - SPI_FLASH_BASE);
                device_mutex_unlock(RT_DEV_LOCK_FLASH);
                printf("chack_sum error\n");
                return -1;
        }
    }

    return 0;
}

static int ota_read(int* off, char* out_buf, int out_buf_len)
{
    return hal_flash_read(HAL_PARTITION_OTA_TEMP, (uint32_t*)off, out_buf, out_buf_len);
}

static int ota_rollback(void *something)
{
    return 0;
}

const char   *aos_get_app_version(void);
static const char *ota_get_version(unsigned char dev_type)
{
    if(dev_type) {
        return "v1.0.0-20180101-1000";//SYSINFO_APP_VERSION;
    } else {
        return aos_get_app_version();
    }
}

ota_hal_module_t ota_hal_module = {
    .init     = ota_init,
    .write    = ota_write,
    .read     = ota_read,
    .boot     = ota_boot,
    .rollback = ota_rollback,
    .version  = ota_get_version
};
