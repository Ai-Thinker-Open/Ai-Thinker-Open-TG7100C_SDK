NAME = tg7100c

HOST_OPENOCD := tg7100c

# Host architecture is RISC-V
HOST_ARCH := RISC-V
use_private_lwip := 1
$(NAME)_TYPE := kernel
CONFIG_USE_STD_DRIVER := 1

#
# #CONFIG_TOOLPREFIX :=
#  #CONFIG_OPTIMIZATION_LEVEL_RELEASE := 1
#CONFIG_M4_SOFTFP := 1

CONFIG_COMBOAPP :=1
btstack := bouffalolab
GLOBAL_CFLAGS += -DBLE_5_0
GLOBAL_CFLAGS += -DCFG_STDLIB_ENABLE
GLOBAL_CFLAGS += -DTG7100C_VERSION_PRINT
#GLOBAL_CFLAGS += -DCFG_WIFI_DEBUG

$(NAME)_COMPONENTS += platform/arch/risc-v/risc_v32I
#$(NAME)_COMPONENTS += libc rhino hal middleware.common
$(NAME)_COMPONENTS += libc rhino hal vfs yloop vcall pwrmgmt
$(NAME)_COMPONENTS += platform/mcu/tg7100c/wifi
$(NAME)_COMPONENTS += platform/mcu/tg7100c/network
$(NAME)_COMPONENTS += platform/mcu/tg7100c/hal_drv
$(NAME)_COMPONENTS += platform/mcu/tg7100c/utils
$(NAME)_COMPONENTS += platform/mcu/tg7100c/stage
$(NAME)_COMPONENTS += platform/mcu/tg7100c/sys

GLOBAL_INCLUDES += ../../arch/risc-v/risc_v32I/gcc
GLOBAL_INCLUDES += ../../../include/hal/soc/
GLOBAL_INCLUDES += ./uartrb
#GLOBAL_INCLUDES += ../../../kernel/protocols/net/include/
GLOBAL_INCLUDES += ./aos   \
					./tg7100c_bsp_driver/RISCV/Device/Bouffalo/TG7100C/Startup/    \
					./tg7100c_bsp_driver/Device/Bouffalo/TG7100C/Peripherals/      \
					./tg7100c_bsp_driver/RISCV/Core/Include/                     \
					./tg7100c_bsp_driver/StdDriver/Inc/                          \
					./tg7100c_bsp_driver/Common/platform_print/                  \
					./tg7100c_bsp_driver/Common/ring_buffer/                     \
					./tg7100c_bsp_driver/Common/soft_crc/                        \
					./tg7100c_bsp_driver/Common/partition/                        \
					./network/lwip/src/include																 \
					./network/lwip/lwip-port/config/													 \
					./network/lwip/lwip-port/Standalone/											 \
					./network/lwip/src/include/compat/posix   \
					./network/lwip/src/include/lwip/apps    \

GLOBAL_CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
#GLOBAL_CFLAGS += -save-temps=obj
GLOBAL_CFLAGS += -msmall-data-limit=4
# -Werror=all
GLOBAL_CFLAGS += -Wno-error=restrict -Wno-error=format-truncation -fstrict-volatile-bitfields \
	  -fshort-enums -Wno-error=unused-variable -Wno-error=implicit-function-declaration -Wno-error=enum-compare

#--specs=nosys.specs
GLOBAL_LDFLAGS += -Wl,--gc-sections \
				  -nostartfiles \
				  --specs=nosys.specs \
				  -usystem_vectors \

$(NAME)_CFLAGS  += -Wall -Werror -Wno-error=comment
$(NAME)_CFLAGS  += -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing -Wno-undef
$(NAME)_CFLAGS	+= -Werror=incompatible-pointer-types

$(NAME)_SOURCES :=  ./tg7100c_bsp_driver/StdDriver/Src/tg7100c_common.c                           \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_adc.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_aon.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_dac.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_dma.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_glb.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_pds.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_hbn.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_i2c.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_ir.c                               \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_uart.c                             \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_pwm.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_sdu.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_spi.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_timer.c                            \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_l1c.c                              \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_ef_ctrl.c                          \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_sec_dbg.c                          \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_sec_eng.c                          \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_tzc_sec.c                          \
					./tg7100c_bsp_driver/Common/platform_print/platform_device.c                \
					./tg7100c_bsp_driver/Common/platform_print/platform_gpio.c                  \
					./tg7100c_bsp_driver/Common/ring_buffer/ring_buffer.c                       \
					./tg7100c_bsp_driver/Common/soft_crc/softcrc.c                       \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_romapi.c           \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_sflash_ext.c       \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_xip_sflash_ext.c   \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_sf_cfg_ext.c       \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_mfg_efuse.c \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_mfg_flash.c \
					./tg7100c_bsp_driver/StdDriver/Src/tg7100c_mfg_media.c \
					./tg7100c_bsp_driver/version.c \
					./gcc/start.S    \
					./gcc/os_trap_entry.S    \
					./aos/init.c   \
					./aos/aos_main.c   \
					./aos/hook_impl.c   \
					./aos/soc_impl.c   \
					./aos/soc_init.c \
					./aos/global_interrupts.c \
					./hal/hal_interpt_tg7100c.c \
					./hal/hal_ota_tg7100c.c \
					./hal/hal_uart_tg7100c.c \
					./hal/hal_adc_tg7100c.c \
					./hal/hal_spi_tg7100c.c \
					./hal/hal_flash_tg7100c.c \
					./hal/hal_gpio_tg7100c.c \
					./hal/hal_pwm_tg7100c.c \
					./hal/hal_wdg_tg7100c.c \
					./hal/hal_sys_tg7100c.c \
					./hal/hal_rng_tg7100c.c	\
					./hal/hal_i2c_tg7100c.c	\
					./hal/hal_rtc_tg7100c.c \
					./hal/hal_hw_tg7100c.c \
					./hal/hal_pwrmgmt_tg7100c.c \
					./hal/wifi_port.c \
					./hal/buffer.c \
					./hal/app_uart.c \
					./aos/bl_vfprintf.c \

HAL_TEST_SOURCES := hal_test/hal_test_entry.c \
					hal_test/flash_test.c  \
					hal_test/adc_test.c  \
					hal_test/uart_slaver_test.c  \
					hal_test/uart_master_test.c \
					hal_test/rtc_test.c  \
					hal_test/spi_master_test.c  \
					hal_test/spi_slave_test.c	\
					hal_test/pwm_test.c	\
					hal_test/rng_test.c	\
					hal_test/i2c_master_test.c	\
					hal_test/wdt_test.c	\
					hal_test/gpio_test.c	\
					hal_test/timer_test.c	\
					hal_test/timer_test_us.c \

$(NAME)_SOURCES += $(HAL_TEST_SOURCES)

GLOBAL_LDS_FILES += platform/mcu/tg7100c/gcc/flash_rom.ld

#GLOBAL_DEFINES += EN_COMBO_HAL_TEST
GLOBAL_DEFINES += CONFIG_ARM
GLOBAL_DEFINES += WITH_LWIP
GLOBAL_DEFINES += CONFIG_NET_LWIP #CONFIG_NO_TCPIP
GLOBAL_DEFINES += AOS_OTA_DISABLE_MD5
GLOBAL_DEFINES += CONFIG_AOS_CLI_STACK_SIZE=3072
GLOBAL_DEFINES += TG7100C_COREDUMP
vfs_DEFINES    += CONFIG_NO_TCPIP

GLOBAL_CFLAGS += -DUSE_M_TIME -DARCH_RISCV

GLOBAL_CFLAGS   += -fno-omit-frame-pointer -march=rv32imfc -mabi=ilp32f -g -Os -save-temps=obj
GLOBAL_CFLAGS   += -fstack-usage -Wstack-usage=1536
GLOBAL_ASMFLAGS += -march=rv32imfc -mabi=ilp32f -g -Os
GLOBAL_LDFLAGS  += -march=rv32imfc -mabi=ilp32f -g -Os

