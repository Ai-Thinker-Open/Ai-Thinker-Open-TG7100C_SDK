
m_path_tmp=$PATH
export PATH=$PATH:$(pwd)/Living_SDK/build/compiler/riscv64_unkown_elf_gcc8.3.0/Linux64/bin
m_cross_tmp=$CROSS_PREFIX
m_chip_tmp=$CHIP
m_cc_tmp=$CC
m_objcopy_tmp=$OBJCOPY
m_strip_tmp=$STRIP
m_ld_tmp=$LD
m_ar_tmp=$AR
m_arch_tmp=$ARCH

tempapp="example/$2"

function remove_old_binary()
{
	rm -rf tools/tg7100c/tg7100c/buildout/$1@$2.bin
}

function config_firmware_file()
{
	cp tools/tg7100c/custom.toml.org tools/tg7100c/custom.toml
	echo "new app $1"
	sed -i "s/##APP##/$1/g" tools/tg7100c/custom.toml
}

function files_cp()
{
	cd -
	rm -rf prebuild/*
	mkdir -p prebuild/lib; mkdir -p prebuild/include
	cp Living_SDK/out/$2@$3/libraries/*.a prebuild/lib/
	cp Living_SDK/framework/protocol/linkkit/sdk/lib/RISC-V/libawss_security.a prebuild/lib/
	cp Living_SDK/framework/protocol/linkkit/sdk/lib/RISC-V/libaiotss.a prebuild/lib/
	cp Living_SDK/platform/mcu/tg7100c/wifi/firmware/firmware.a prebuild/lib/
	cp Living_SDK/platform/mcu/tg7100c/network/ble/blecontroller/blecontroller.a prebuild/lib/
	cp Living_SDK/platform/mcu/tg7100c/gcc/flash_rom.ld prebuild/lib/
	rm -f prebuild/lib/$2.a

	# cp Living_SDK/platform/mcu/tg7100c/hal_drv/tg7100c_hal/bl_flash.h prebuild/lib/
	cp -rfa Living_SDK/platform/mcu/tg7100c/hal_drv/tg7100c_hal/* prebuild/include
	cp -rfa Living_SDK/platform/mcu/tg7100c/sys/blmtd/include/* prebuild/include
	cp -rfa Living_SDK/platform/mcu/tg7100c/utils/include/* prebuild/include

	cp -rfa Living_SDK/include/* prebuild/include
	cp -rfa Living_SDK/utility/cjson/include/* prebuild/include
	cp -rfa Living_SDK/framework/uOTA/inc/ota_service.h prebuild/include
	cp -rfa Living_SDK/framework/uOTA/hal/*.h prebuild/include
	cp -rfa Living_SDK/framework/protocol/linkkit/sdk/iotx-sdk-c_clone/include/* prebuild/include
	cp -rfa Living_SDK/framework/protocol/linkkit/sdk/iotx-sdk-c_clone/src/infra/utils/misc/*.h prebuild/include
	cp -rfa Living_SDK/framework/protocol/linkkit/sdk/iotx-sdk-c_clone/src/infra/log/*.h prebuild/include
	cp -rfa Living_SDK/platform/arch/risc-v/risc_v32I/gcc/*.h prebuild/include
	cp -rfa Living_SDK/framework/bluetooth/breeze/api/breeze_export.h prebuild/include
	cp -rfa Living_SDK/framework/bluetooth/breeze/include/*.h prebuild/include

	cp --parents Living_SDK/kernel/rhino/core/include/* prebuild/include
	cp --parents Living_SDK/kernel/vfs/include/*.h prebuild/include
	mv prebuild/include/Living_SDK/kernel prebuild/include

	cp --parents Living_SDK/framework/netmgr/include/*.h prebuild/include
	mv prebuild/include/Living_SDK/framework prebuild/include

	cp --parents Living_SDK/platform/mcu/include/* prebuild/include
	cp --parents Living_SDK/platform/arch/risc-v/risc_v32I/gcc/*.h prebuild/include
	mv prebuild/include/Living_SDK/platform prebuild/include

	cp --parents Living_SDK/board/$3/*.h prebuild/include
	mv prebuild/include/Living_SDK/board prebuild/include

	rm -rf prebuild/include/Living_SDK
	cp -f Products/$1/$2/$2.mk_old Products/$1/$2/$2.mk
}

function build_bin()
{
	mkdir -p out
	OUT=out/$2@$3
	rm -rf $OUT
	mkdir -p $OUT
	echo "make version--framework.a"
	cd Service/version
	#make clean; make APP=$2 PLATFORM=$3 BOARD=$3 CONFIG_SERVER_REGION=$4 CONFIG_SERVER_ENV=$5 CONFIG_DEBUG=$6 $7
	cd -

	echo "make app"
	cd Products/$1/$2
	export CROSS_PREFIX=riscv64-unknown-elf-
    export CHIP=tg7100c
	export CC=${CROSS_PREFIX}gcc
	export OBJCOPY=${CROSS_PREFIX}objcopy
	export STRIP=${CROSS_PREFIX}strip
    export LD=${CROSS_PREFIX}ld
    export AR=${CROSS_PREFIX}ar
	export ARCH=risc_v32I
	make APP=$2 PLATFORM=$3 BOARD=$3 CONFIG_SERVER_REGION=$4 CONFIG_SERVER_ENV=$5 CONFIG_DEBUG=$6 $7
	cd -
	
	p=prebuild/lib

	CHIP_LIBS="-Wl,--end-group -Wl,-no-whole-archive -Wl,--gc-sections -Wl,--cref -L ./$p/ -T ./$p/flash_rom.ld -march=rv32imfc -mabi=ilp32f -Os -nostartfiles -Wl,--gc-sections -Wl,--cref --specs=nano.specs -u _printf_float -lm "
	
	COMMON_LIBS="$p/$2.a  $p/board_$3.a  $p/$CHIP.a  $p/vcall.a  $p/kernel_init.a  $p/auto_component.a   $p/libiot_sdk.a $p/iotx-hal.a  $p/netmgr.a  $p/framework.a  $p/cjson.a  $p/ota.a  $p/cli.a  $p/ota_hal.a  $p/$ARCH.a  $p/newlib_stub.a  $p/rhino.a  $p/digest_algorithm.a  $p/log.a  $p/activation.a  $p/imbedtls.a  $p/kv.a  $p/yloop.a  $p/hal.a  $p/ota_transport.a  $p/ota_download.a  $p/ota_verify.a  $p/base64.a  $p/alicrypto.a  $p/vfs.a  $p/vfs_device.a   $p/libawss_security.a $p/libaiotss.a $p/breeze.a $p/breeze_adapter.a $p/blecontroller.a $p/blestack.a $p/tg7100c_wifi_driver.a $p/tg7100c_dhcpd.a $p/tg7100c_utils.a $p/blfdt.a $p/tg7100c_lwip.a $p/chip_code.a $p/hal_drv.a $p/firmware.a $p/blmtd.a"
	$CC -Wl,-Map,$OUT/$2@$3.map -Wl,--whole-archive -Wl,--start-group  ${COMMON_LIBS} ${CHIP_LIBS} -o $OUT/$2@$3.elf

	## generate map & strip elf
	/usr/bin/python ./Living_SDK/build/scripts/map_parse_gcc_riscv.py $OUT/$2@$3.map > $OUT/$2@$3_map.csv
	cat $OUT/$2@$3_map.csv
	$STRIP -o $OUT/$2@$3.stripped.elf  $OUT/$2@$3.elf
	$OBJCOPY -O binary $OUT/$2@$3.stripped.elf $OUT/$2@$3.bin
  
  mkdir -p ./tools/tg7100c/tg7100c/buildout
  cp $OUT/$2@$3.bin ./tools/tg7100c/tg7100c/buildout/$2@$3.bin

  cd ./tools/tg7100c/
  ./bflb_build_tg7100c custom.toml
  cd -
  cp ./tools/tg7100c/buildout/ota.bin $OUT/$3_ota.bin
  cp Living_SDK/out/$2@$3/binary/readme.txt $OUT/
}

## check toolchain ##
TOOLCHAIN_URL="https://static.dev.sifive.com/dev-tools/riscv64-unknown-elf-gcc-8.3.0-2019.08.0-x86_64-linux-ubuntu14.tar.gz"
if [ ! -f Living_SDK/build/compiler/riscv64_unkown_elf_gcc8.3.0/Linux64/bin/riscv64-unknown-elf-gcc ];then
  echo "download toolchain"
  mkdir -p Living_SDK/build/compiler/riscv64_unkown_elf_gcc8.3.0 && cd $_
  #if hash axel 2>/dev/null; then
  #  axel -aj 6 $TOOLCHAIN_URL
  #else
  #  wget $TOOLCHAIN_URL
  #fi
  #tar -xvf ./riscv64-unknown-elf-gcc-8.3.0-2019.08.0-x86_64-linux-ubuntu14.tar.gz
  git clone -b dev_bl --depth=1 https://gitee.com/sandstorm/toolchain_for_linux.git toolchain_repo
  ln -s toolchain_repo/main Linux64
  cd -
fi

## remove old binary file
echo "remove old binary file"
remove_old_binary $2 $3
## config firmware file
echo "config firmware file"
config_firmware_file $2
##  build aos  ##
if [[ ! -d Living_SDK/example/$2 ]] || [[ ! -d Living_SDK/out/$2@$3 ]]; then
	if [[ -d Living_SDK/example ]] && [[ -d Products/$1 ]] && [[ -d Products/$1/$2 ]]; then
		echo "do cp"
		rm -rf Living_SDK/example/$2
		#cp -rf Products/$1/$2 Living_SDK/example/$2
		mv -f Products/$1/$2/$2.mk Products/$1/$2/$2.mk_old
		cd Living_SDK/example
		ln -s ../../Products/$1/$2 .
		cd -

		./tools/cmd/linux64/awk '{ gsub("'./make.settings'","example/${APP_FULL}/make.settings"); gsub("'"\?= MAINLAND"'","'"?= $4"'"); gsub("'"\?= ONLINE"'","'"?= $5"'"); gsub("'"CONFIG_DEBUG\ \?=\ 0"'","'"CONFIG_DEBUG ?= $6"'");   print $0 }' Products/$1/$2/$2.mk_old > Living_SDK/${tempapp}/$2.mk

		echo "make aos sdk: $2@$3 CONFIG_SERVER_REGION=$4 CONFIG_SERVER_ENV=$5 CONFIG_DEBUG=$6 $7"
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

		if [[ -s Living_SDK/diff.txt ]] || [[ ! -f prebuild/lib/board_$3.a ]]; then
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
export CHIP=$m_chip_tmp
export CC=$m_cc_tmp
export OBJCOPY=$m_objcopy_tmp
export STRIP=$m_strip_tmp
export LD=$m_ld_tmp
export AR=$m_ar_tmp
export ARCH=$m_arch_tmp
export PATH=$m_path_tmp

