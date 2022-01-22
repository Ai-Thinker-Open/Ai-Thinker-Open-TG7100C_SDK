
m_cross_tmp=$CROSS_PREFIX
tempapp="example/$2"

function files_cp()
{
	cd -
	rm -rf prebuild/*
	mkdir -p prebuild/lib; mkdir -p prebuild/include
	cp Living_SDK/out/$2@$3/libraries/*.a prebuild/lib/
	cp Living_SDK/out/$2@$3/libraries/board_bk7231u.a prebuild/lib/board_bk7231udevkitc.a
	#cp Living_SDK/out/$2@$3/ld/* prebuild/lib/
	cp Living_SDK/platform/mcu/bk7231u/bk7231u.ld prebuild/lib/bk7231u.ld.S
	cp Living_SDK/platform/mcu/bk7231u/beken/beken.a  prebuild/lib/
	cp Living_SDK/platform/mcu/bk7231u/beken/ip/ip.a  prebuild/lib/
	cp Living_SDK/platform/mcu/bk7231u/beken/driver/ble/ble.a  prebuild/lib/
	cp Living_SDK/platform/mcu/bk7231u/beken/driver/ble/ble_lib.a  prebuild/lib/
	#cp Living_SDK/platform/mcu/bk7231u/beken/alios/entry/entry.a  prebuild/lib/
	cp Living_SDK/security/irot/km/lib/ARM968E-S/libkm.a prebuild/lib
	cp Living_SDK/framework/protocol/linkkit/sdk/lib/ARM968E-S/*.a  prebuild/lib

	rm -f prebuild/lib/$2.a

	cp -rfa Living_SDK/include/* prebuild/include
	cp -rfa Living_SDK/utility/cjson/include/* prebuild/include
	cp -rfa Living_SDK/framework/uOTA/inc/ota_service.h prebuild/include
	cp -rfa Living_SDK/framework/protocol/linkkit/sdk/iotx-sdk-c_clone/include/* prebuild/include
	cp -rfa Living_SDK/framework/protocol/linkkit/sdk/iotx-sdk-c_clone/src/infra/utils/misc/*.h prebuild/include
	cp -rfa Living_SDK/framework/protocol/linkkit/sdk/iotx-sdk-c_clone/src/infra/log/*.h prebuild/include
	cp -rfa Living_SDK/framework/bluetooth/breeze/api/breeze_export.h prebuild/include
	cp -rfa Living_SDK/framework/bluetooth/breeze/include/bzopt.h prebuild/include
	cp --parents Living_SDK/kernel/rhino/core/include/* prebuild/include
	cp --parents Living_SDK/kernel/vfs/include/*.h prebuild/include
	mv prebuild/include/Living_SDK/kernel prebuild/include
	cp --parents Living_SDK/framework/netmgr/include/*.h prebuild/include
	mv prebuild/include/Living_SDK/framework prebuild/include
	cp -rfa Living_SDK/framework/uOTA/hal/*.h prebuild/include

	#cp --parents Living_SDK/platform/mcu/bk7231u/include/*.h prebuild/include
	cp --parents Living_SDK/platform/arch/arm/armv7m/gcc/m4/*.h prebuild/include
	mv prebuild/include/Living_SDK/platform prebuild/include

	cp --parents Living_SDK/board/$3/*.h prebuild/include
	mv prebuild/include/Living_SDK/board prebuild/include

	rm -rf prebuild/include/Living_SDK
	cp -f Products/$1/$2/$2.mk_old Products/$1/$2/$2.mk
}

function build_bin()
{
	export CROSS_PREFIX=arm-none-eabi-
	mkdir -p out
	OUT=out/$2@$3
	rm -rf $OUT
	mkdir -p $OUT

	FW_COMPILE_DATE=$(date +%Y%m%d.%H%M%S)
	echo "make fw_app_version.a"
	cd Service/version
	make APP_TYPE=$1 APP=$2 PLATFORM=$3 clean; make APP_TYPE=$1 APP=$2 PLATFORM=$3 BOARD=$3 CONFIG_SERVER_REGION=$4 CONFIG_SERVER_ENV=$5 CONFIG_DEBUG=$6 CONFIG_COMPILE_DATE=$FW_COMPILE_DATE $7
	cd -

	echo "make app  APP=$2 PLATFORM=$3 BOARD=$3 CONFIG_SERVER_REGION=$4 CONFIG_SERVER_ENV=$5 CONFIG_DEBUG=$6 $7"
	cd Products/$1/$2
	make clean; make APP_TYPE=$1 APP=$2 PLATFORM=$3 clean; make APP_TYPE=$1 APP=$2 PLATFORM=$3 BOARD=$3 CONFIG_SERVER_REGION=$4 CONFIG_SERVER_ENV=$5 CONFIG_DEBUG=$6 CONFIG_COMPILE_DATE=$FW_COMPILE_DATE $7
	cd -
	##  build elf
	cd prebuild/lib

	CHIP=bk7231u
	CC=arm-none-eabi-gcc
	OBJCOPY=arm-none-eabi-objcopy
	STRIP=arm-none-eabi-strip
	ARCH=arch_armv5
	p=prebuild/lib
	CHIP_LIBS="$p/framework_runtime.a $p/app_runtime.a $p/bk7231u.a $p/beken.a $p/ip.a $p/ble.a $p/ble_lib.a $p/entry.a $p/libkm.a -Wl,--end-group -Wl,-no-whole-archive -Wl,--gc-sections -Wl,--cref -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian --specs=nosys.specs -nostartfiles --specs=nano.specs -u _printf_float -Wl,--wrap=boot_undefined -Wl,--wrap=boot_pabort -Wl,--wrap=boot_dabort -uframework_info -uapp_info -T $p/bk7231u.ld -L ./$p "

	if [[ "$2" == comboapp ]] || [[ "$2" == smart_led_bulb ]] || [[ "$2" == smart_outlet ]] || [[ "$2" == smart_led_strip ]] || [[ "$2" == smart_outlet_meter ]];then
		BREEZE_LIB="$p/breeze.a"
	fi

	if [[ "$2" == httpapp ]] || [[ "$2" == coapapp ]];then
		COMMON_LIBS="$p/fw_app_version.a $p/$2.a  $p/board_bk7231u.a  $p/vcall.a  $p/kernel_init.a  $p/auto_component.a  $p/libiot_sdk.a $p/iotx-hal.a  $p/hal_init.a  $p/netmgr.a  $p/framework.a  $p/cjson.a   $p/cli.a   $p/$ARCH.a  $p/newlib_stub.a  $p/rhino.a  $p/digest_algorithm.a  $p/net.a  $p/log.a  $p/activation.a  $p/chip_code.a  $p/imbedtls.a $p/yloop.a  $p/hal.a   $p/alicrypto.a  $p/vfs.a  $p/vfs_device.a   $p/libawss_security.a $p/libaiotss.a "
	elif [[ "$2" == living_platform ]];then
		COMMON_LIBS="$p/fw_app_version.a $p/$2.a  $p/board_bk7231u.a  $p/vcall.a  $p/kernel_init.a  $p/auto_component.a  $p/libiot_sdk.a $p/iotx-hal.a  $p/hal_init.a  $p/netmgr.a  $p/framework.a  $p/cjson.a  $p/ota.a  $p/cli.a  $p/ota_hal.a  $p/$ARCH.a  $p/newlib_stub.a  $p/rhino.a  $p/digest_algorithm.a  $p/net.a  $p/log.a  $p/activation.a  $p/chip_code.a  $p/imbedtls.a $p/yloop.a  $p/hal.a  $p/ota_transport.a  $p/ota_download.a  $p/ota_verify.a  $p/base64.a  $p/alicrypto.a  $p/vfs.a  $p/vfs_device.a   $p/libawss_security.a $p/libaiotss.a $p/breeze.a"
	else
		COMMON_LIBS="$p/fw_app_version.a $p/$2.a  $p/board_bk7231u.a  $p/vcall.a  $p/kernel_init.a  $p/auto_component.a  $p/libiot_sdk.a $p/iotx-hal.a  $p/hal_init.a  $p/netmgr.a  $p/framework.a  $p/cjson.a  $p/ota.a  $p/cli.a  $p/ota_hal.a  $p/$ARCH.a  $p/newlib_stub.a  $p/rhino.a  $p/digest_algorithm.a  $p/net.a  $p/log.a  $p/activation.a  $p/chip_code.a  $p/imbedtls.a $p/yloop.a  $p/hal.a  $p/ota_transport.a  $p/ota_download.a  $p/ota_verify.a  $p/base64.a  $p/alicrypto.a  $p/vfs.a  $p/vfs_device.a   $p/libawss_security.a $p/libaiotss.a "
	fi
	cd -
	## generate map & strip elf
	./tools/cmd/linux64/awk '{ gsub("'INCLUDE\ \board/bk7231udevkitc/bk7231udevkitc.ld'","INCLUDE Living_SDK/board/bk7231udevkitc/bk7231udevkitc.ld"); print $0 }' $p/bk7231u.ld.S > $p/bk7231u.ld
	$CC -Wl,-Map,$OUT/$2@$3.map -Wl,--whole-archive -Wl,--start-group ${COMMON_LIBS} ${BREEZE_LIB} ${CHIP_LIBS} -o $OUT/$2@$3.elf
	## generate map & strip elf
	/usr/bin/python ./tools/scripts/map_parse_gcc.py $OUT/$2@$3.map > $OUT/$2@$3_map.csv
	$STRIP  -o $OUT/$2@$3.stripped.elf  $OUT/$2@$3.elf
	/usr/bin/python ./tools/scripts/map_parse_gcc.py $OUT/$2@$3.map

	## gen bin
	$OBJCOPY -O binary -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes  $OUT/$2@$3.stripped.elf $OUT/$2@$3.bin
	$OBJCOPY -O ihex -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes  $OUT/$2@$3.stripped.elf $OUT/$2@$3.hex

	if [[ "$CHIP"  == "bk7231u" ]]; then
		./tools/bk7231u/encrypt_linux $OUT/$2@$3.bin 0 0 0 0
		cp -f $OUT/$2@$3_crc.bin $OUT/$2@$3.bin
	fi
	./Living_SDK/build/cmd/linux64/xz -f --lzma2=dict=32KiB --check=crc32 -k $OUT/$2@$3.bin
	cp $OUT/$2@$3.bin.xz $OUT/$2@$3_ota.bin
	/usr/bin/python tools/scripts/ota_gen_md5_bin.py $OUT/$2@$3_ota.bin
	## gen all bin (bootloader + application)
	cp Living_SDK/platform/mcu/bk7231u/bootloader_bk7231u.bin $OUT/
	/usr/bin/python tools/bk7231u/gen_firmware_img.py $OUT/$2@$3_crc.bin $OUT/bootloader_bk7231u.bin

	#mv out/readme.txt $OUT
}

## check toolchain ##
if [ ! -f Living_SDK/build/compiler/gcc-arm-none-eabi/Linux64/bin/arm-none-eabi-gcc ];then
	echo "download toolchain"
	mkdir -p Living_SDK;mkdir -p Living_SDK/build;mkdir -p Living_SDK/build/compiler;mkdir -p Living_SDK/build/compiler/gcc-arm-none-eabi
	cd tools && git clone --depth=1 https://gitee.com/alios-things/gcc-arm-none-eabi-linux.git && mv gcc-arm-none-eabi-linux/main ../Living_SDK/build/compiler/gcc-arm-none-eabi/ && mv ../Living_SDK/build/compiler/gcc-arm-none-eabi/main ../Living_SDK/build/compiler/gcc-arm-none-eabi/Linux64 && rm -rf gcc-arm-none-eabi-linux && cd -
fi

##  build aos  ##
if [ ! -d Living_SDK/example/$2 ] || [ ! -d Living_SDK/out/$2@$3 ]; then
	if [ -d Living_SDK/example ] && [ -d Products/$1 ] && [ -d Products/$1/$2 ]; then
		echo "do cp"
		rm -rf Living_SDK/example/$2
		#cp -rf Products/$1/$2 Living_SDK/example/$2
		mv -f Products/$1/$2/$2.mk Products/$1/$2/$2.mk_old
		cd Living_SDK/example
		ln -s ../../Products/$1/$2 .
		cd -
		./tools/cmd/linux64/awk '{ gsub("'./make.settings'","example/${APP_FULL}/make.settings"); gsub("'"\?= MAINLAND"'","'"?= $4"'"); gsub("'"\?= ONLINE"'","'"?= $5"'"); gsub("'"CONFIG_DEBUG\ \?=\ 0"'","'"CONFIG_DEBUG ?= $6"'");   print $0 }' Products/$1/$2/$2.mk_old > Living_SDK/${tempapp}/$2.mk

		echo "make aos sdk"
		cd Living_SDK
		aos make clean
		aos make $2@$3 CONFIG_SERVER_REGION=$4 CONFIG_SERVER_ENV=$5 CONFIG_DEBUG=$6 $7
		echo "cp libs and incs..."
		files_cp $1 $2 $3
	else
		echo "folder Living_SDK/example or Products/$1/$2 is not existed!"
	fi
else
	if [ -d Living_SDK/example ]; then
		echo "$2 folder is existed! check $2.mk file."

		diff Living_SDK/${tempapp}/$2.mk Living_SDK/${tempapp}/$2.mk_old > Living_SDK/diff.txt

		if [[ -s Living_SDK/diff.txt ]] || [[ ! -f prebuild/lib/board_bk7231u.a ]]; then
			echo "need rebuild all again!"
			mv -f Products/$1/$2/$2.mk Products/$1/$2/$2.mk_old
			./tools/cmd/linux64/awk '{ gsub("'./make.settings'","example/${APP_FULL}/make.settings"); gsub("'"\?= MAINLAND"'","'"?= $4"'"); gsub("'"\?= ONLINE"'","'"?= $5"'"); gsub("'"CONFIG_DEBUG\ \?=\ 0"'","'"CONFIG_DEBUG ?= $6"'");   print $0 }' Products/$1/$2/$2.mk_old > Living_SDK/${tempapp}/$2.mk
			cd Living_SDK
			aos make clean
			aos make $2@$3 CONFIG_SERVER_REGION=$4 CONFIG_SERVER_ENV=$5 CONFIG_DEBUG=$6 $7
			echo "cp libs and incs..."
			files_cp $1 $2 $3
		fi
	fi
fi

echo "start to build app......"
build_bin $1 $2 $3 $4 $5 $6 "$7"

export CROSS_PREFIX=$m_cross_tmp
