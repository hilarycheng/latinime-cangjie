# Copyright (C) 2009 The Android Open Source Project
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
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# LOCAL_ARM_MODE  := arm

LOCAL_MODULE    := chinese_table
LOCAL_SRC_FILES := chinese_table.c input_method.c phrase.c 

LOCAL_LDLIBS    := -lm -llog 

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

# LOCAL_ARM_MODE  := arm
LOCAL_MODULE    := quick
LOCAL_SRC_FILES := quick_method.c
LOCAL_LDLIBS    := -lm -llog 

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS    += -DCANGJIE5
LOCAL_MODULE    := cangjie5
LOCAL_SRC_FILES := cangjie_method.c
LOCAL_LDLIBS    := -lm -llog 

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

# LOCAL_ARM_MODE  := arm
LOCAL_CFLAGS    += -DCANGJIE3
LOCAL_MODULE    := cangjie3
LOCAL_SRC_FILES := cangjie_method.c
LOCAL_LDLIBS    := -lm -llog 

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

# LOCAL_ARM_MODE  := arm
LOCAL_MODULE    := stroke
LOCAL_SRC_FILES := stroke_method.c
LOCAL_LDLIBS    := -lm -llog 

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE    := cantonese
LOCAL_SRC_FILES := cantonese_method.c
LOCAL_LDLIBS    := -lm -llog 

include $(BUILD_SHARED_LIBRARY)
