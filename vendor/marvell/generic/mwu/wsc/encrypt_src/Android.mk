# Derived from the Makefile in this directory
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE:=encrypt_pxa

CONFIG_OS_LINUX=y

# Compliler option for sha256.o
LOCAL_CFLAGS += -DINTERNAL_SHA256

# Compliler option for not including unused function in aes_wrap.o
LOCAL_CFLAGS += -DCONFIG_NO_AES_WRAP
LOCAL_CFLAGS += -DCONFIG_NO_AES_OMAC1
LOCAL_CFLAGS += -DCONFIG_NO_AES_CTR
LOCAL_CFLAGS += -DCONFIG_NO_AES_EAX

# Compliler option for DH big number math
LOCAL_CFLAGS += -DCONFIG_CRYPTO_INTERNAL
LOCAL_CFLAGS += -DCONFIG_INTERNAL_LIBTOMMATH
LOCAL_CFLAGS += -DLTM_FAST
LOCAL_CFLAGS += -DCONFIG_WPS

ifeq ($(CONFIG_OS_LINUX), y)
LOCAL_CFLAGS += -DOS_LINUX
endif

ENC_LIB_SUPP_SRCS =	\
	sha256.c	\
	aes.c	\
	aes_wrap.c

ENC_LIB_SRCS =	\
	key_algorim.c	\
	bignum.c	\
	crypto_internal.c	\
	dh.c \
	encrypt.c

LOCAL_CFLAGS += -DINTERNAL_SHA1 -DINTERNAL_MD5
ENC_LIB_SUPP_SRCS += sha1.c md5.c

LOCAL_C_INCLUDES = $(LOCAL_PATH) external/openssl/include \
        $(LOCAL_PATH)/../../os/include \
        $(LOCAL_PATH)/../../mwu

# crypto.c borrowed from wpa_supplicant needs some defines
LOCAL_CFLAGS += -DEAP_TLS_FUNCS -DEAP_FAST
FROM_WPA_SUPPLICANT_SRC = \
    crypto.c \
	sha256_wpasupp.c

LOCAL_SRC_FILES := \
	$(ENC_LIB_SUPP_SRCS) \
	$(ENC_LIB_SRCS) \
	$(FROM_WPA_SUPPLICANT_SRC)

ifeq ($(CONFIG_OS_LINUX), y)
LOCAL_SRC_FILES += ../../os/linux/mwu_timer.c
endif

include $(BUILD_STATIC_LIBRARY)
