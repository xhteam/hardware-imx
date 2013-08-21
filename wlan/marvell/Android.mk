#
# Copyright (C) 2013 Quester Tech.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

ifeq ($(BOARD_WLAN_DEVICE),mrvl8787)
LOCAL_PATH:= $(call my-dir)

MRVL_ANDROID_SRC_BASE:=$(LOCAL_PATH)
MRVL_ANDROID_SRC_WLAN:= $(MRVL_ANDROID_SRC_BASE)/wlan_src
MRVL_ANDROID_SRC_BLUETOOTH:= $(MRVL_ANDROID_SRC_BASE)/mbt_src
MRVL_ANDROID_ROOT:= $(ANDROID_SRC_ROOT)


MRVL_LINUXPATH:=$(MRVL_ANDROID_ROOT)/kernel_imx
MRVL_CROSS_COMPILE:=$(MRVL_ANDROID_ROOT)/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-

wlan_mod_cleanup := $(MRVL_ANDROID_ROOT)/$(MRVL_ANDROID_WLAN_SRC)/wlandummy

$(wlan_mod_cleanup) :
	$(MAKE) -C $(MRVL_ANDROID_SRC_WLAN) ARCH=arm CROSS_COMPILE=$(MRVL_CROSS_COMPILE) KERNELDIR=$(MRVL_LINUXPATH) INSTALLDIR=$(TARGET_OUT)/lib/modules/	clean 
	$(MAKE) -C $(MRVL_ANDROID_SRC_WLAN) ARCH=arm CROSS_COMPILE=$(MRVL_CROSS_COMPILE) KERNELDIR=$(MRVL_LINUXPATH) INSTALLDIR=$(TARGET_OUT)/lib/modules/	distclean 	
	mkdir -p $(TARGET_OUT)/lib/modules/mrvl
	mkdir -p $(TARGET_OUT)/etc/firmware/mrvl

wlan_module_file :=wlan_src/mlan.ko
$(MRVL_ANDROID_SRC_BASE)/$(wlan_module_file):$(wlan_mod_cleanup) $(TARGET_PREBUILT_KERNEL) $(ACP)
	$(MAKE) -C $(MRVL_ANDROID_SRC_WLAN) ARCH=arm SOURCE_DIR=$(MRVL_ANDROID_ROOT)/$(MRVL_ANDROID_SRC_WLAN) CROSS_COMPILE=$(MRVL_CROSS_COMPILE) KERNELDIR=$(MRVL_LINUXPATH) INSTALLDIR=$(TARGET_OUT)/lib/modules/	default 
	$(ACP) -fpt $(MRVL_ANDROID_SRC_WLAN)/sd8xxx.ko $(TARGET_OUT)/lib/modules/mrvl
	$(ACP) -fpt $(MRVL_ANDROID_SRC_BASE)/FwImage/sd8787_uapsta.bin  $(TARGET_OUT)/etc/firmware/mrvl

include $(CLEAR_VARS)
LOCAL_MODULE := mlan.ko
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/modules/mrvl
LOCAL_SRC_FILES := $(wlan_module_file)
include $(BUILD_PREBUILT)


bluetooth_mod_cleanup := $(MRVL_ANDROID_ROOT)/$(MRVL_ANDROID_WLAN_SRC)/bluetoothdummy

$(bluetooth_mod_cleanup) :
	$(MAKE) -C $(MRVL_ANDROID_SRC_BLUETOOTH) ARCH=arm CROSS_COMPILE=$(MRVL_CROSS_COMPILE) KERNELDIR=$(MRVL_LINUXPATH) INSTALLDIR=$(TARGET_OUT)/lib/modules/	clean 
	$(MAKE) -C $(MRVL_ANDROID_SRC_BLUETOOTH) ARCH=arm CROSS_COMPILE=$(MRVL_CROSS_COMPILE) KERNELDIR=$(MRVL_LINUXPATH) INSTALLDIR=$(TARGET_OUT)/lib/modules/	distclean 	
	mkdir -p $(TARGET_OUT)/lib/modules/mrvl

bluetooth_module_file :=mbt_src/bt8xxx.ko
$(MRVL_ANDROID_SRC_BASE)/$(bluetooth_module_file):$(bluetooth_mod_cleanup) $(TARGET_PREBUILT_KERNEL) $(ACP)
	$(MAKE) -C $(MRVL_ANDROID_SRC_BLUETOOTH) ARCH=arm SOURCE_DIR=$(MRVL_ANDROID_ROOT)/$(MRVL_ANDROID_SRC_BLUETOOTH) CROSS_COMPILE=$(MRVL_CROSS_COMPILE) KERNELDIR=$(MRVL_LINUXPATH) INSTALLDIR=$(TARGET_OUT)/lib/modules/	default 

include $(CLEAR_VARS)
LOCAL_MODULE := bt8xxx.ko
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/modules/mrvl
LOCAL_SRC_FILES := $(bluetooth_module_file)
include $(BUILD_PREBUILT)


#
#mlan2040coex
#
include $(CLEAR_VARS)
LOCAL_MODULE_PATH:=$(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE := mlan2040coex
LOCAL_SRC_FILES := \
	wlan_src/mapp/mlan2040coex/mlan2040coex.c \
	wlan_src/mapp/mlan2040coex/mlan2040misc.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

#
#mlanconfig
#
include $(CLEAR_VARS)
LOCAL_MODULE_PATH:=$(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE := mlanconfig
LOCAL_SRC_FILES := \
	wlan_src/mapp/mlanconfig/mlanconfig.c \
	wlan_src/mapp/mlanconfig/mlanhostcmd.c \
	wlan_src/mapp/mlanconfig/mlanmisc.c 
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

#
#mlanevent
#
include $(CLEAR_VARS)
LOCAL_MODULE_PATH:=$(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE := mlanevent
LOCAL_SRC_FILES := \
	wlan_src/mapp/mlanevent/mlanevent.c 
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

#
#mlanutl
#
include $(CLEAR_VARS)
LOCAL_CFLAGS:= -DSTA_SUPPORT
LOCAL_MODULE_PATH:=$(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE := mlanutl
LOCAL_SRC_FILES := \
	wlan_src/mapp/mlanutl/mlanutl.c 
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)


#
#uaputl
#
include $(CLEAR_VARS)
LOCAL_MODULE_PATH:=$(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE := uaputl
LOCAL_SRC_FILES := \
	wlan_src/mapp/uaputl/uaputl.c \
	wlan_src/mapp/uaputl/uapcmd.c \
	wlan_src/mapp/uaputl/uaphostcmd.c 
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)


#
#wifidirectutl
#
include $(CLEAR_VARS)
LOCAL_MODULE_PATH:=$(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE := wifidirectutl
LOCAL_SRC_FILES := \
	wlan_src/mapp/wifidirectutl/wifidirectutl.c \
	wlan_src/mapp/wifidirectutl/wifi_display.c 
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)


#
#Extra wpa_supplicant_8_lib
#
include $(call all-makefiles-under,$(LOCAL_PATH))

endif

