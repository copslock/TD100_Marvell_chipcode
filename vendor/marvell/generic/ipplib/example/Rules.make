
# T: tool chain selection; mavell gcc / arm gcc
#    valuse: -gcc
# CPU_TYPE: CFLAGS selection; -mcpu=iwmmxt -mtune=iwmmxt / -mcpu=xscale -mtune=xscale
#    values: xsc_linux
# PGO_FLAGS: pgo switch; 
#    values: -lgcov
#    if PGO, toolchain should be the same with codec lib build
# USE_STATICLIB: use static or dynamic lib switch; y-use static lib / n-use dynamic lib
#    values: y, n


USE_STATICLIB=y

#==============================================================================
# GNU pathes                          (server admin update)
#==============================================================================
ifeq ($(T), -gcc)
PATH_GNU_BIN=/usr/local/arm-linux/bin
TOOLCHAIN_PREFIX=arm-iwmmxt-linux-gnueabi
#cpu: wmmx2_linux, wmmx_linux, xsc_linux
ifeq ($(CPU_TYPE), xsc_linux)
CFLAGS= -mcpu=xscale -mtune=xscale
else
CFLAGS= -mcpu=iwmmxt -mtune=iwmmxt
endif
else
#PATH_GNU_BIN=/usr/local/arm-marvell-linux-gnueabi/bin
PATH_GNU_BIN=/usr/local/arm-marvell-linux-gnueabi-4.4.4/bin
TOOLCHAIN_PREFIX=arm-marvell-linux-gnueabi
CFLAGS= -mcpu=marvell-f 
endif

#==============================================================================
# GNU           binaries             (server admin update)
#==============================================================================
CC=$(PATH_GNU_BIN)/$(TOOLCHAIN_PREFIX)-gcc
CXX=$(PATH_GNU_BIN)/$(TOOLCHAIN_PREFIX)-gcc
AR=$(PATH_GNU_BIN)/$(TOOLCHAIN_PREFIX)-ar
AS=$(PATH_GNU_BIN)/$(TOOLCHAIN_PREFIX)-as
LN=$(PATH_GNU_BIN)/$(TOOLCHAIN_PREFIX)-gcc

#==============================================================================
# GNU build options:                (build engineer update)
#==============================================================================
CFLAGS+= -O3 -Wall -mabi=aapcs-linux -fPIC -D_IPP_LINUX
CXXFLAGS= $(CFLAGS)
ASFLAGS= -k -mcpu=iwmmxt2
ARFLAGS=
LNFLAGS= -lpthread -ldl -lm -lrt $(PGO_FLAGS) -Wl,-R ./

#==============================================================================
# User root path												(user update)
#==============================================================================
PATH_USR_ROOT=../../../..
PATH_USR_LIB=$(PATH_USR_ROOT)/lib
PATH_USR_BUILD=$(PATH_USR_ROOT)/example/$(CODEC_NAME)/build
PATH_USR_SRC=$(PATH_USR_ROOT)/example/$(CODEC_NAME)/src
PATH_USR_BIN=$(PATH_USR_ROOT)/example/$(CODEC_NAME)/bin

#==============================================================================
# External include option 											(user update)
#==============================================================================
OPT_INC_EXT=\
-I $(PATH_USR_ROOT)/include\
-I $(PATH_USR_SRC)\

#==============================================================================
# User libraries          										(user update)
#==============================================================================
ifeq ($(USE_STATICLIB), y)
USR_LIB_SUFFIX=a
USR_LIBS=\
$(PATH_USR_LIB)/libcodec$(CODEC_NAME).$(USR_LIB_SUFFIX)\
$(PATH_USR_LIB)/libmiscgen.$(USR_LIB_SUFFIX)
else
USR_LIB_SUFFIX=so
USR_LIBS= \
 -L$(PATH_USR_LIB) -lcodec$(CODEC_NAME) -lmiscgen
endif
