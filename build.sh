#!/bin/bash

##------default config-----------------------------------------------------------------------------------##

default_type="example"
default_app="smart_outlet"
default_board="tg7100cevb"
default_region=MAINLAND
default_env=ONLINE
default_debug=0
default_args=""

##-------------------------------------------------------------------------------------------------------##

export LC_ALL=en_US.utf-8
export LANG=en_US.utf-8

path_tmp=$PATH
export PATH=$path_tmp:$(pwd)/Living_SDK/build/compiler/gcc-arm-none-eabi/Linux64/bin/
start_time=$(date +%s)

if [ "$(uname)" = "Linux" ]; then
    CUR_OS="Linux"
elif [ "$(uname)" = "Darwin" ]; then
    CUR_OS="OSX"
elif [ "$(uname | grep NT)" != "" ]; then
    CUR_OS="Windows"
else
    echo "error: unkonw OS"
    exit 1
fi

app=$2
if [ xx"$2" == xx ]; then
        app=$default_app
fi

if [[ xx"$1" == xxclean ]]; then
	rm -rf Living_SDK/example/$app
	rm -rf out
	rm -rf Living_SDK/out
	#git checkout Living_SDK/example
	exit 0
fi

if [[ xx"$1" == xx--help ]] || [[ xx"$1" == xxhelp ]] || [[ xx"$1" == xx-h ]] || [[ xx"$1" == xx-help ]]; then
	echo "./build.sh $default_type $default_app $default_board $default_region $default_env $default_debug $default_args "
	exit 0
fi

type=$1
if [ xx"$1" == xx ]; then
	type=$default_type
fi

board=$3
if [ xx"$3" == xx ]; then
	board=$default_board
fi

# REGION=MAINLAND or SINGAPORE
if [[ xx$4 == xx ]]; then 
	echo "REGION SET AS MAINLAND"
	REGION=$default_region
else
	REGION=$4
fi

# ENV=ONLINE
if [[ xx$5 == xx ]]; then 
	echo "ENV SET AS ONLINE"
	ENV=$default_env
else
	ENV=$5
fi

# CONFIG_DEBUG=0
if [[ xx$6 == xx ]]; then 
	echo "CONFIG_DEBUG SET AS 0"
	CONFIG_DEBUG=$default_debug
else
	CONFIG_DEBUG=$6
fi

if [[ xx$7 == xx ]]; then 
	echo "ARGS SET AS NULL"
	ARGS=$default_args
else
	ARGS="$7"
fi

function update_golden_product()
{
	gp_type=$1
	gp_app=$2
	git submodule update --init --remote Products/$gp_type/$gp_app
	if [ $? -ne 0 ]; then
		echo 'code download or update error!'
		exit 1
	fi
}

if [[ "${type}-${app}" == "Smart_outlet-smart_outlet_meter" ]] || [[ "${type}-${app}" == "Smart_lighting-smart_led_strip" ]] || [[ "${type}-${app}" == "Smart_lighting-smart_led_bulb" ]]; then
	echo 'golden sample product--------------------'
	update_golden_product $type $app
fi

if [[ -d Products/$type/$app ]] && [[ ! -f prebuild/lib/board_$board.a ]]; then
	rm -rf Living_SDK/example/$app
	# git checkout Living_SDK/example
fi

echo "----------------------------------------------------------------------------------------------------"
echo "PATH=$PATH"
echo "OS: ${CUR_OS}"
echo "Product type=$type app_name=$app board=$board REGION=$REGION ENV=$ENV CONFIG_DEBUG=$CONFIG_DEBUG ARGS=$ARGS"
echo "----------------------------------------------------------------------------------------------------"
if [[ -d Products/$type/$app ]] && [[ ! -f Products/$type/$app/makefile ]]; then
	./tools/cmd/linux64/awk '{ gsub("'"smart_outlet"'","'"$app"'");  print $0 }' Products/Smart_outlet/smart_outlet/makefile > Products/$type/$app/makefile
fi
function build_end()
{
	end_time=$(date +%s)
	cost_time=$[ $end_time-$start_time ]
	echo "build time is $(($cost_time/60))min $(($cost_time%60))s"
	export PATH=$path_tmp
}

function build_sdk()
{
	rm -rf out/$app@/@${board}/*
	if [[ -d Living_SDK/example ]] && [[ -d Products/$type ]] && [[ -d Products/$type/$app ]]; then
		rm -rf Living_SDK/example/$app
		cp -rf Products/$type/$app Living_SDK/example/$app
		
		./tools/cmd/linux64/awk '{ gsub("'./make.settings'","example/${APP_FULL}/make.settings"); gsub("'"\?= MAINLAND"'","'"?= ${REGION}"'"); gsub("'"\?= ONLINE"'","'"?= ${ENV}"'"); gsub("'"CONFIG_DEBUG\ \?=\ 0"'","'"CONFIG_DEBUG ?= $CONFIG_DEBUG"'");   print $0 }' Products/$type/$app/$app.mk > Living_SDK/example/${app}/$app.mk
		cd Living_SDK
		aos make clean
		echo -e "aos make $app@${board} CONFIG_SERVER_REGION=${REGION} CONFIG_SERVER_ENV=${ENV} CONFIG_DEBUG=${CONFIG_DEBUG} $ARGS"
		aos make $app@${board} CONFIG_SERVER_REGION=${REGION} CONFIG_SERVER_ENV=${ENV} CONFIG_DEBUG=${CONFIG_DEBUG} "$ARGS"
		cd -
		if [[ -f Living_SDK/out/$app@${board}/binary/$app@${board}.bin ]]; then
			cp -rfa Living_SDK/out/$app@${board}/binary/* out/$app@${board}
			build_end
			exit 0
		else
			echo "build failed!"
			exit 1
		fi
	fi
}

mkdir -p out;mkdir -p out/$app@${board}
rm -rf out/$app@${board}/*

if [[ ! -d Products/$type ]] || [[ ! -d Products/$type/$app ]]; then
	echo "path of Products/$type or Products/$type/$app don't exist!"
	if [[ ! -d Living_SDK/example/$app ]]; then
		echo "path of Living_SDK/example  don't exist!"
		exit 1
	else
		cd Living_SDK
		aos make clean
		echo -e "aos make $app@${board} CONFIG_SERVER_REGION=${REGION} CONFIG_SERVER_ENV=${ENV} CONFIG_DEBUG=${CONFIG_DEBUG} $ARGS"
		aos make $app@${board} CONFIG_SERVER_REGION=${REGION} CONFIG_SERVER_ENV=${ENV} CONFIG_DEBUG=${CONFIG_DEBUG} "$ARGS"
		cd -
		if [[ -f Living_SDK/out/$app@${board}/binary/$app@${board}.bin ]]; then
			cp -rfa Living_SDK/out/$app@${board}/binary/* out/$app@${board}
			build_end
			exit 0
		else
			echo "build failed!"
			exit 1
		fi
	fi
fi

##  build app  ##
if [[ "${board}" == "hf-lpb130" ]] || [[ "${board}" == "hf-lpb135" ]] || [[ "${board}" == "hf-lpt230" ]] || [[ "${board}" == "hf-lpt130" ]] || [[ "${board}" == "uno-91h" ]]; then
	./tools/5981a.sh  $type $app ${board} ${REGION} ${ENV} ${CONFIG_DEBUG} "${ARGS}"
elif [[ "${board}" == "mk3060" ]] || [[ "${board}" == "mk3061" ]]; then
	##  moc108  ##
	./tools/mk3060.sh $type $app ${board} ${REGION} ${ENV} ${CONFIG_DEBUG} "${ARGS}"
elif [[ "${board}" == "mk3080" ]] || [[ "${board}" == "mk3092" ]] || [[ "${board}" == "mk5080" ]]; then
	##  rtl8710bn  ##
	./tools/mk3080.sh  $type $app ${board} ${REGION} ${ENV} ${CONFIG_DEBUG} "${ARGS}"
elif [[ "${board}" == "mx1270" ]]; then
	./tools/mx1270.sh  $type $app ${board} ${REGION} ${ENV} ${CONFIG_DEBUG} "${ARGS}"
elif [[ "${board}" == "asr5501" ]]; then
	./tools/asr5501.sh  $type $app ${board} ${REGION} ${ENV} ${CONFIG_DEBUG} "${ARGS}"
elif [[ "${board}" == "bk7231udevkitc" ]]; then
	./tools/bk7231udevkitc.sh  $type $app ${board} ${REGION} ${ENV} ${CONFIG_DEBUG} "${ARGS}"
elif [[ "${board}" == "amebaz_dev" ]]; then
	./tools/amebaz_dev.sh  $type $app ${board} ${REGION} ${ENV} ${CONFIG_DEBUG} "${ARGS}"
elif [[ "${board}" == "tg7100cevb" ]]; then
	./tools/tg7100cevb.sh  $type $app ${board} ${REGION} ${ENV} ${CONFIG_DEBUG} "${ARGS}"
else
	# echo -e "build paras error: you can build board ${board} with follow step: \n cd Living_SDK\n aos make $app@${board} CONFIG_SERVER_REGION=${REGION} CONFIG_SERVER_ENV=${ENV} CONFIG_DEBUG=${CONFIG_DEBUG}"
	# exit 1
	build_sdk
fi

if [[ ! -f out/$app@${board}/$app@${board}.bin ]]; then
	echo "build failed!"
	exit 1
fi

build_end
log=$(ls -al out/$app@${board}/$app@${board}.bin -h|awk '{print $5}')
echo -e "\033[41;36mFirmware $app@${board}.bin Size=${log} \033[0m"
exit 0


