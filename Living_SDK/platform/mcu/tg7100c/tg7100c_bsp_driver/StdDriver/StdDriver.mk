#clear_vars
LOCAL_SRCS_FILE:=

MODULE_DIR:= BSP/StdDriver
SUBMODULE_OUT_DIR:= $(TARGET_OUT_PATH)/BSP/StdDriver
SUBMODULE_SRC_DIR := $(MODULE_DIR)/Src

STDDRIVER_CFLAGS:=
STDDRIVER_INCLUDE:=  -I $(MODULE_DIR)/Inc \

stddriver_sources := tg7100c_common.c tg7100c_uart.c tg7100c_dma.c tg7100c_glb.c tg7100c_timer.c tg7100c_ef_ctrl.c  \
                     tg7100c_aon.c tg7100c_hbn.c tg7100c_sec_eng.c tg7100c_sdu.c tg7100c_dac.c \
                     tg7100c_l1c.c tg7100c_pwm.c tg7100c_i2c.c tg7100c_sec_dbg.c tg7100c_ir.c tg7100c_spi.c \
                     tg7100c_pds.c tg7100c_tzc_sec.c tg7100c_adc.c tg7100c_acomp.c

ifeq ($(ROMAPI),y)
ifeq ($(ROMAPI_TEST),y)
	stddriver_sources += tg7100c_romapi.c
    STDDRIVER_CFLAGS += -DTG7100C_USE_ROM_DRIVER
else
    stddriver_sources += tg7100c_romapi.c tg7100c_sflash_ext.c tg7100c_sf_cfg_ext.c
    STDDRIVER_CFLAGS += -DTG7100C_USE_ROM_DRIVER
endif
else
	stddriver_sources += tg7100c_sf_ctrl.c tg7100c_sflash.c tg7100c_sf_cfg.c  tg7100c_xip_sflash.c tg7100c_sflash_ext.c \
	 					 tg7100c_sf_cfg_ext.c
endif

stddriver_objs := $(addprefix $(SUBMODULE_OUT_DIR)/, $(subst .c,.o,$(stddriver_sources)))
base_objects += $(stddriver_objs)

$(SUBMODULE_OUT_DIR)/%.o:$(SUBMODULE_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(STDDRIVER_CFLAGS) $(GLOBAL_INCLUDE) $(COMMON_INCLUDE) $(STDDRIVER_INCLUDE) $< -o $@

.PHONY: stddriver
stddriver: $(stddriver_objs)
	@echo  "stddriver_objs is $(stddriver_objs)"
