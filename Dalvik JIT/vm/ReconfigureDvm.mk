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

include $(CLEAR_VARS)

# Variables used in the included Dvm.mk.
dvm_os := $(TARGET_OS)
dvm_arch := $(TARGET_ARCH)
dvm_arch_variant := $(TARGET_ARCH_VARIANT)

# uncomment to disable x86-atom variant
#ifeq ($(dvm_arch_variant),x86-atom)
#dvm_arch_variant := x86
#endif

# Use DALVIK_FI to make change to FI build version.
ifeq ($(TARGET_ARCH),x86)
    ifeq ($(DALVIK_FI),x86)
        dvm_arch_variant := $(DALVIK_FI)
    endif
    ifeq ($(DALVIK_FI),x86-atom)
        dvm_arch_variant := $(DALVIK_FI)
    endif
endif

# X86 JIT is available only with x86-atom FI
ifneq ($(dvm_arch_variant),x86-atom)
    WITH_JIT := false
endif

include $(LOCAL_PATH)/Dvm.mk

ifeq ($(INTEL_HOUDINI),true)
    LOCAL_CFLAGS += -DWITH_HOUDINI
endif

LOCAL_SHARED_LIBRARIES += liblog libcutils libnativehelper libz libdl libcorkscrew

ifeq ($(HAVE_SELINUX),true)
LOCAL_C_INCLUDES += external/libselinux/include
LOCAL_SHARED_LIBRARIES += libselinux
LOCAL_CFLAGS += -DHAVE_SELINUX
endif # HAVE_SELINUX

LOCAL_STATIC_LIBRARIES += libdex

ifeq ($(INTEL_HOUDINI),true)
    LOCAL_STATIC_LIBRARIES += libhoudini_hook
endif

LOCAL_C_INCLUDES += external/stlport/stlport bionic/ bionic/libstdc++/include
LOCAL_SHARED_LIBRARIES += libstlport

# Don't install on any build by default
LOCAL_MODULE_TAGS := optional
