/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <math.h>
#include <string.h>

#include <aos/aos.h>
#include <aos/init.h>
#include <aos/kernel.h>
#include <aos_main.h>
#include <hal/wifi.h>
#include <k_api.h>

#include <flash.h>

#include <tg7100c.h>
#include <bl_irq.h>
#include <blog.h>
#include <clic.h>
#include <lwip/tcpip.h>
#include <lwip/stats.h>
#include <hal/soc/adc.h>
#include <hal/soc/spi.h>
#include <hal/soc/pwm.h>
#include "hal/soc/uart.h"
#include <hal/soc/flash.h>
#include <adc.h>
#include <gpio.h>
#include <spi.h>
#include <pwm.h>

#define AOS_INIT_STACK (2048) // 512 * 4 = 2048 Byte

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

ktask_t *g_aos_init = NULL;
ktask_t *g_aos_loop = NULL;

extern hal_wifi_module_t sim_aos_wifi_tg7100c;

int app_download_addr = 0x10000;// fixme !!!!!!!!!!!!!
int kernel_download_addr = 0xD8000;// fixme !!!!!!!!!!!!!

extern void system_timer_start(void);

#if 0
hal_wifi_module_t *wifi_default_if;

static void _scan_aps(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    // wifi_scan_test
    wifi_default_if = hal_wifi_get_default_module();
    wifi_default_if->start_scan(wifi_default_if);

    krhino_task_sleep(16000);
}

static void _get_mac(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    // wifi_scan_test
    uint8_t mac[6];
    wifi_default_if = hal_wifi_get_default_module();
    wifi_default_if->get_mac_addr(wifi_default_if, mac);
}

static void cmd_netstat(char *buf, int len, int argc, char **argv)
{
    tcpip_callback(stats_netstat, NULL);
}

static void wifi_capcode_cmd(char *buf, int len, int argc, char **argv)
{
    int capcode = 0;

    if (2 != argc && 1 != argc) {
        printf("Usage: %s capcode\r\n", argv[0]);
        return;
    }

    /*get capcode*/
    if (1 == argc) {
        printf("Capcode %u is being used\r\n", hal_sys_capcode_get());
        return;
    }

    /*set capcode*/
    capcode = atoi(argv[1]);
    printf("Setting capcode to %d\r\n", capcode);

    if (capcode > 0) {
        hal_sys_capcode_update(capcode, capcode);
    }
}

void cmd_wdt(char *buf, int len, int argc, char **argv)
{
    bl_wdt_init(10000);
}

void cmd_adc_deinit(char *buf, int len, int argc, char **argv)
{
    hal_adc_finalize(&dev);
    adc_flag = 0;
}

void cmd_pwm(char *buf, int len, int argc, char **argv)
{
    int i;
    pwm_dev_t pwm[5];
    pwm_dev_t *pwm_p;

#if 0
    for (i = 0; i < 5; i++) {
        pwm_p = &pwm[i];
        pwm_p->port = i;
        pwm_p->config.freq = 20000;
        pwm_p->config.duty_cycle = 50;

        printf("pwm init ch = %d\r\n", i);
        hal_pwm_init(&pwm[i]);
        hal_pwm_start(&pwm[i]);
    }
#else
        i = 2;
        pwm_p = &pwm[i];
        pwm_p->port = i;
        pwm_p->config.freq = 20000;
        pwm_p->config.duty_cycle = 0.2;
        printf("pwm init ch = %d\r\n", i);
        hal_pwm_init(&pwm[i]);
        hal_pwm_start(&pwm[i]);
#endif
}

static int count_flag = 0;
spi_dev_t spi_dev;
void cmd_spi_tol(char *buf, int len, int argc, char **argv)
{
    int ret;
    int i;
    uint8_t data[512];

    for (i = 0; i < 512; i++) {
        data[i] = 0x0f;
    }

    if (count_flag == 0) {
        spi_dev.port = 0;
        spi_dev.config.mode = 1;
        spi_dev.config.freq = 2000000;
        hal_spi_init(&spi_dev);
        count_flag = 1;
    }

    ret = hal_spi_send(&spi_dev, data, 512, 2000000);
    if (ret == 0) {
        printf("transfer success. \r\n");
    } else {
        printf("transfer failed. \r\n");
    }
}

void cmd_spi_fin(char *buf, int len, int argc, char **argv)
{
    hal_spi_finalize(&spi_dev);
    printf("deinit spi \r\n");
    count_flag = 0;

    return;
}

static void cmd_wifi_connect(char *buf, int len, int argc, char **argv)
{// fixme
    uint8_t *ssid = argv[1];
    uint8_t *passwd = argv[2];

    if (3 != argc) {
        printf("[USAGE]: %s [ssid] [password]\r\n", argv[0]);
        return;
    }

    HAL_Awss_Connect_Ap(0,
                        ssid,
                        passwd,
                        0,
                        0,
                        0,
                        0);
}

static void cmd_wifi_sta_info(char *buf, int len, int argc, char **argv)
{
    ip4_addr_t ip, gw, mask;
    int rssi;
    int8_t power_rate_table[38];


    wifi_mgmr_sta_ip_get(&ip.addr, &gw.addr, &mask.addr);
    wifi_mgmr_rssi_get(&rssi);
    bl_tpc_power_table_get(power_rate_table);
    printf("RSSI:   %ddbm\r\n", rssi);
    printf("IP  :   %s \r\n", ip4addr_ntoa(&ip) );
    printf("MASK:   %s \r\n", ip4addr_ntoa(&mask));
    printf("GW  :   %s \r\n", ip4addr_ntoa(&gw));
    puts(  "Power Table (dbm):\r\n");
    puts(  "--------------------------------\r\n");
    printf("  11b: %u %u %u %u             (1Mbps 2Mbps 5.5Mbps 11Mbps)\r\n",
        power_rate_table[0],
        power_rate_table[1],
        power_rate_table[2],
        power_rate_table[3]
    );
    printf("  11g: %u %u %u %u %u %u %u %u (6Mbps 9Mbps 12Mbps 18Mbps 24Mbps 36Mbps 48Mbps 54Mbps)\r\n",
        power_rate_table[0 + 8],
        power_rate_table[1 + 8],
        power_rate_table[2 + 8],
        power_rate_table[3 + 8],
        power_rate_table[4 + 8],
        power_rate_table[5 + 8],
        power_rate_table[6 + 8],
        power_rate_table[7 + 8]
    );
    printf("  11n: %u %u %u %u %u %u %u %u (MCS0 ~ MCS7)\r\n",
        power_rate_table[0 + 16],
        power_rate_table[1 + 16],
        power_rate_table[2 + 16],
        power_rate_table[3 + 16],
        power_rate_table[4 + 16],
        power_rate_table[5 + 16],
        power_rate_table[6 + 16],
        power_rate_table[7 + 16]
    );
    puts(  "--------------------------------\r\n");

}

static void wifi_rc_fixed_enable(char *buf, int len, int argc, char **argv)
{
    uint8_t mode = 0;
    uint8_t mcs = 0;
    uint8_t gi = 0;
    uint16_t rc = 0x0000; //format mode is HT_MF only

    if (argc != 4) {
        printf("rc_fix_en [b/g/n] [MCS] [GI]");
        return;
    }
    mode = atoi(argv[1]);
    mcs = atoi(argv[2]);
    gi = atoi(argv[3]);

    printf("wifi set mode:%s, mcs:%d, gi:%d\r\n", (mode == 1?"n mode":"b/g mdoe"), mcs, gi);

    if (mode == 1) {
        rc |= mode << 12 | gi << 9 | mcs;
    } else if(mode == 0) {
        rc |= (1 << 9) | (1 << 10) | mcs;
    }

    printf("wifi rc:0x%x\r\n", rc);

    wifi_mgmr_rate_config(rc);
}

void cmd_media(char *buf, int len, int argc, char **argv)
{
    extern int ali_factory_media_print(void);
    int res;
    uint8_t mac[6] = {0};

    bl_efuse_read_mac(mac);

    aos_msleep(200);
    printf("mac address is %02X:%02X:%02X:%02X:%02X:%02X\r\n",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    aos_msleep(200);
    bl_efuse_read_mac_factory(mac);

    aos_msleep(200);
    printf("factory mac address is %02X:%02X:%02X:%02X:%02X:%02X\r\n",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    aos_msleep(200);
    res = ali_factory_media_print();
    if (0 != res) {
        printf("ali_factory_media_print error, res = %d\r\n", res);
    }
}
#endif

void cmd_mfg(char *buf, int len, int argc, char **argv)
{
    union _reg_t {
        uint8_t byte[4];
        uint32_t word;
    } mfg = {
        .byte = "0mfg",
    };

    *(volatile uint32_t*)(0x4000F100) = mfg.word;

    hal_reboot();
}

#if TG7100C_ADC_TEST
static void cmd_adc(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    static adc_dev_t g_adc_dev;
    static uint8_t inited = 0;
    uint32_t val = 0;
 
    if (!inited) {
        inited = 1;
        g_adc_dev.port = 4;
        g_adc_dev.config.sampling_cycle = 1000;
        g_adc_dev.config.mode = CONTINUE;
        g_adc_dev.config.type = VOLT_ADC;
        hal_adc_init(&g_adc_dev); 
    }
  
    hal_adc_value_get(&g_adc_dev, &val, 10);
    printf("adc val %ld\r\n", val); 
}
#endif

#if TG7100C_VERSION_PRINT

extern const char *tg7100c_wifi_version_get(void);
extern const char *tg7100c_ble_version_get(void);
extern const char *tg7100c_bsp_version_get(void);

static void cmd_version(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    printf("TG7100C WIFI:%s\r\n", tg7100c_wifi_version_get()); 
    printf("TG7100C BLE:%s\r\n", tg7100c_ble_version_get()); 
    printf("TG7100C BSP:%s\r\n", tg7100c_bsp_version_get()); 
}
#endif


static void cmd_freemm(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    extern void show_mm();
    show_mm();
    //stats_display();
}

static void cmd_uart1_send(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    extern uart_dev_t uart_1;
    static int g_uart_init_flag = 0;
    if (!g_uart_init_flag)
    {
        uart1_rbuf_init(2000000);
        g_uart_init_flag = 1;
    }
    
    hal_uart_send(&uart_1, argv[1], strlen(argv[1]), 1000);
}

struct cli_command cmd[] = {
    {"free", "free mem", cmd_freemm},
    {"mfg", "mfg test", cmd_mfg},
    {"uart1_send", "uart 1 send test", cmd_uart1_send},
#if TG7100C_ADC_TEST
    {"adc_test", "adc test", cmd_adc},
#endif
#if TG7100C_VERSION_PRINT
    {"version_bsp", "version_bsp print", cmd_version},
#endif
#if 0
    {"media", "media print", cmd_media},
    {"scan", "scan ssid", _scan_aps},
    {"get_mac","get mac", _get_mac},
    {"netstat", "show current net states", cmd_netstat},
    {"wifi_capcode", "wifi capcode", wifi_capcode_cmd},
    {"wifi_sta_info", "wifi_sta_info", cmd_wifi_sta_info},
    {"wifi_sta_connect", "wifi_sta_connect ssid pwd", cmd_wifi_connect},//fixme
    {"rc_fix_en", "wifi rate control fixed rate enable", wifi_rc_fixed_enable},
    {"alifactory", "alifactory", cmd_alifactory},
    {"wdt", "wdt test", cmd_wdt},
    {"adc_deinit", "adc test", cmd_adc_deinit},
    {"pwm", "pwm test", cmd_pwm},
    {"spi_test", "spi test", cmd_spi_tol},
    {"spi_fin", "spi fin", cmd_spi_fin},
#endif
};

void trace_start(void)
{
#ifdef EN_COMBO_HAL_TEST
    extern void hal_test_entry(void);
    hal_test_entry();
#endif
    aos_cli_register_commands(cmd, sizeof(cmd)/sizeof(cmd[0]));
}

static void app_loop_entry(void *arg)
{
    uart0_rbuf_init(2000000);
    hal_wifi_init();
    aos_kernel_init(&kinit);
}

static void aos_init_entry(void *p_arg)
{
  memset((void *)0x4203fc00, 0x55, 0x400);
  system_timer_start();
  bl_sec_init();
  bl_sec_test();
  bl_dma_init();
  hal_boot2_init();
  hal_flash_init(HAL_PARTITION_PARAMETER_2);

  /* board config is set after system is init*/
  hal_board_cfg(0);

  // regist wifi hal
  hal_wifi_register_module(&sim_aos_wifi_tg7100c);

  krhino_task_dyn_create(&g_aos_loop, "tsk_loop", 0, 5, 0, (1024 + 128), app_loop_entry, 1);

  krhino_task_dyn_del(NULL);
}

extern void handle_m_time_interrupt(void);
void bfl_main(void)
{
  //GLB_Set_System_CLK_Div(3,0);
  //SystemCoreClockSet(40*1000*1000);
  char print_info[40];

  _init();

  bl_sys_early_init();
  /*Init UART In the first place*/
  bl_uart_init(0, 16, 7, 255, 255, 2 * 1000 * 1000);

  bl_irq_init();

  bl_sys_init();
  
  puts("Boot Reason:        ");
  bl_sys_rstinfo_getsting(print_info);
  puts(print_info);
  puts("\r\n");
  
  aos_init();

  bl_irq_register(MTIME_IRQn, handle_m_time_interrupt);

  log_info("Starting tg7100c now....\r\n");
  printf("start-----------hal\r\n");

  krhino_task_dyn_create(&g_aos_init, "aos_entry", 0, 1, 0, AOS_INIT_STACK, aos_init_entry, 1);

  aos_start();
}

