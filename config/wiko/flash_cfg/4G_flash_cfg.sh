#!/bin/bash

echo "4GB+4Gb config start copy .."

cp mediatek/config/s4011/flash_cfg/custom_MemoryDevice_4G.h  mediatek/custom/s4011/preloader/inc/custom_MemoryDevice.h 
cp mediatek/config/s4011/flash_cfg/project_4G mediatek/config/s4011/autoconfig/kconfig/project
cp mediatek/config/s4011/flash_cfg/ProjectConfig_4G.mk mediatek/config/s4011/ProjectConfig.mk

echo "4GB+4Gb config copy done!"








