ifeq ($(BOARD_HAS_IMX_SENSOR),true)
    include $(call all-subdir-makefiles,marvell)
endif
