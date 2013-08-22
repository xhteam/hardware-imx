atheros_dirs := compat-wireless
ifeq ($(BOARD_HAS_ATH_WLAN),true)
    include $(call all-subdir-makefiles,$(atheros_dirs))
endif
ifeq ($(BOARD_WLAN_DEVICE),mrvl8787)
    include $(call all-subdir-makefiles,marvell)
endif
