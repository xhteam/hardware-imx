ifeq ($(BOARD_HAS_IMX_SENSOR),true)
LOCAL_PATH := $(my-dir)
include $(call first-makefiles-under,$(LOCAL_PATH))
endif
