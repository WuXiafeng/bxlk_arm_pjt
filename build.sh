#!/bin/bash
#Linux Only
#Add by Wuxiafeng 20181202 First version

#Set xilinx env
source /opt/Xilinx/SDK/2015.4/settings64.sh
if [ $1 == work ]; then
	CROSS=
else
	CROSS=arm-xilinx-linux-gnueabi-
fi
	
GNU=g
ARCH=arm
export TARGET_OS=linux
export CROSS
export GNU
export SUFFIX=
export ARCH

if [ $1 == clean ]; then
	make clean
	make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- -C ./modules clean
else
	make -B
	make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- -C ./modules
fi

exit 0

