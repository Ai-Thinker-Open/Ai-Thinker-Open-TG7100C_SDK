
ifeq ($(HOST_OS),Win32)
XZ := $(TOOLS_ROOT)/cmd/win32/xz.exe
CP := $(TOOLS_ROOT)/cmd/win32/cp.exe
else  # Win32
ifeq ($(HOST_OS),Linux32)
XZ := $(TOOLS_ROOT)/cmd/linux32/xz
CP := $(TOOLS_ROOT)/cmd/linux32/cp
else # Linux32
ifeq ($(HOST_OS),Linux64)
XZ := $(TOOLS_ROOT)/cmd/linux64/xz
CP := $(TOOLS_ROOT)/cmd/linux64/cp
else # Linux64
ifeq ($(HOST_OS),OSX)
XZ := $(TOOLS_ROOT)/cmd/osx/xz
CP := $(TOOLS_ROOT)/cmd/osx/cp
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

XZ_CMD := if [ -f $(XZ) ]; then $(XZ) --format=lzma -A -z -k -f -v $(BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
OTA_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_ota$(BIN_OUTPUT_SUFFIX))
ALI_OTA_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_ali_ota$(BIN_OUTPUT_SUFFIX))
BOOTLOADER_FILE := platform/mcu/$(HOST_MCU_FAMILY)/bootloader/bootloader.bin
FACTORY_FILE := board/$(PLATFORM)/factory/factory.bin
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/scripts/gen_firmware_img.py"

ifneq ($(findstring $(PLATFORM),  hf-lpb130 hf-lpb135 hf-lpt130 hf-lpt230),)
HF_XZ_CMD := if [ -f $(XZ) ]; then $(XZ) --format=lzma -a -z -k -f -v $(BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
endif

EXTRA_POST_BUILD_TARGETS += gen_crc_bin
gen_crc_bin:
	@$(CP) $(BIN_OUTPUT_FILE) $(BIN_OUTPUT_FILE).org
	@$(XZ_CMD)
	$(PYTHON) $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 $(BOOTLOADER_FILE) NONE $(FACTORY_FILE)
	$(PYTHON) $(SCRIPTS_PATH)/ota_gen_md5_bin.py $(OTA_BIN_OUTPUT_FILE)
ifneq ($(HF_XZ_CMD), )
	@$(CP) $(BIN_OUTPUT_FILE).lzma $(BIN_OUTPUT_FILE).ali.lzma
	@$(CP) $(OTA_BIN_OUTPUT_FILE) $(ALI_OTA_BIN_OUTPUT_FILE)
	@$(HF_XZ_CMD)
	$(PYTHON) $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 $(BOOTLOADER_FILE) HF $(FACTORY_FILE)
	$(PYTHON) $(SCRIPTS_PATH)/ota_gen_md5_bin.py $(OTA_BIN_OUTPUT_FILE)
endif

