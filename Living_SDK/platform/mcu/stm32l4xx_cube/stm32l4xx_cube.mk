NAME := stm32l4xx_cube
HOST_OPENOCD := stm32l4xx
$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal vfs digest_algorithm

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

GLOBAL_INCLUDES += \
                   Drivers/STM32L4xx_HAL_Driver/Inc \
                   Drivers/STM32L4xx_HAL_Driver/Inc/Legacy \
                   Drivers/CMSIS/Include \
                   Drivers/CMSIS/Device/ST/STM32L4xx/Include
                   
$(NAME)_SOURCES := Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_can.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_comp.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc_ex.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cryp.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cryp_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dcmi.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma2d.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dsi.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_firewall.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gfxmmu.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hash.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hash_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hcd.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_irda.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lcd.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ltdc.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ltdc_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nand.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nor.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_opamp.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_opamp_ex.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ospi.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd_ex.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c   \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sai.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smartcard.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smartcard_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smbus.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sram.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_swpmi.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tsc.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_wwdg.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_adc.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_comp.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_crc.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_crs.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dac.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma2d.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.c    \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_fmc.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_i2c.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_lptim.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_lpuart.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_opamp.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_pwr.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rng.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rtc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_spi.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usb.c \
				   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.c \
                   Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/system_stm32l4xx.c

$(NAME)_SOURCES += aos/soc_impl.c \
                   aos/trace_impl.c \
                   aos/aos.c \
                   hal/hal_uart_stm32l4.c \
                   hal/flash_l4.c \
                   hal/flash_port.c \
                   hal/hw.c \
                   hal/hal_sd_stm32l4.c \
                   hal/hal_adc_stm32l4.c \
                   hal/hal_i2c_stm32l4.c \
                   hal/hal_gpio_stm32l4.c \
                   hal/hal_rtc_stm32l4.c \
                   hal/hal_spi_stm32l4.c \
                   hal/hal_qspi_stm32l4.c \
                   hal/hal_nand_stm32l4.c \
                   hal/hal_nor_stm32l4.c

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M4 --apcs=/hardfp --fpu=vfpv4_sp_d16 -D__MICROLIB -g --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m  \
                 -mlittle-endian \
                 -mthumb -mthumb-interwork \
                 -mfloat-abi=hard \
                 -mfpu=fpv4-sp-d16 \
                 -w
GLOBAL_CFLAGS  += -D__VFP_FP__
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M4 -g --apcs=/hardfp --fpu=vfpv4_sp_d16 --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4 \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4 \
                   -mlittle-endian \
                   -mthumb \
                   -mfloat-abi=hard \
                   -mfpu=fpv4-sp-d16 \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M4   \
                  -L --fpu=vfpv4_sp_d16 \
                  -L --apcs=/hardfp \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M4.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m4  \
                  -mlittle-endian  \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs \
                  -mfloat-abi=hard \
                  -mfpu=fpv4-sp-d16 \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif
