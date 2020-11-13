ifeq ($(HOST_OS),Win32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_win.exe"
IMG_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/wifi_img_win.exe"
OTA_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/wifi_ota_win.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_linux"
IMG_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/wifi_img_linux"
OTA_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/wifi_ota_linux"
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_linux"
IMG_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/wifi_img_linux"
OTA_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/wifi_ota_linux"
else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_osx"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

ota1_offset = 69632 # 0x11000
ota2_offset = 1114112 # 0x110000

CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
CRC_BIN_OUTPUT_FILE_XIP2 :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.xip2_crc$(BIN_OUTPUT_SUFFIX))

CRC_BIN_BOOT := $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/bootloader_bk7231u_uart1.bin
CRC_BIN_BASIC := $(OUTPUT_DIR)/binary/image_basic.bin
CRC_BIN_ALL := $(OUTPUT_DIR)/binary/image_all.bin
CRC_BIN_OTA := $(OUTPUT_DIR)/binary/ota_appliances.bin

EXTRA_POST_BUILD_TARGETS += gen_crc_bin gen_standard_images
gen_crc_bin:
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 0 0 0))
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE_XIP2) 0 0 0 0))
	
gen_standard_images:
	@echo $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_OUTPUT_FILE_XIP2) $(ota2_offset) 
	$(eval OUT_MSG := $(shell $(IMG_TOOL) $(CRC_BIN_BOOT) 0 $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_BASIC)))
	$(eval OUT_MSG := $(shell $(IMG_TOOL) $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_OUTPUT_FILE_XIP2) $(ota2_offset) $(CRC_BIN_ALL)))
	$(eval OUT_MSG := $(shell $(OTA_TOOL) $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_OUTPUT_FILE_XIP2) $(ota2_offset) $(CRC_BIN_OTA)))
