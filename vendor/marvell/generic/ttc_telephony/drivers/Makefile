PWD  := $(shell pwd)
include $(PWD)/.config

ifeq ($(MAKERULES),)
ifeq "$(CONFIG_BIONIC)" "y"
include Rules.make
else
include ../../../Rules.make
endif
else
include $(MAKERULES)
endif

ifeq "$(CONFIG_BIONIC)" "y"
# strip tool from android tool-chain
STRIP = $(BUILDROOT)/../../../../../out/host/linux-x86/bin/soslim
STRIP_FLAGS = --strip --shady --quiet
endif

COMCFG_CSV = configuration/ttc/COMCfg.csv

BUILDROOT=$(PWD)
ifeq "$(CONFIG_BIONIC)" "y"
include $(PXA_SRC_DIR)/Bionic.make
CCOMPILER=$(CPP)
SUDO=
else
COMLIBS=-lpthread -lutil -lrt
CCOMPILER=$(CC)
endif

DIAG_LINUX_OBJ_TEL = diag/diag_al/diagDBtel.o
DIAG_LINUX_SRC_TEL = diag/diag_al/diagDBtel.c
DIAG_LINUX_OBJ_AUD = linux_audio/diagDBaud.o
DIAG_LINUX_SRC_AUD = linux_audio/diagDBaud.c
DIAG_LINUX_OBJ_ATCMD = linux_telephony/atcmdsrv/src/diagDBatcmd.o
DIAG_LINUX_SRC_ATCMD = linux_telephony/atcmdsrv/src/diagDBatcmd.c

ATCMDSRVOBJS =\
	linux_telephony/atcmdsrv/src/telcc.o\
	linux_telephony/atcmdsrv/src/cc_api.o\
	linux_telephony/atcmdsrv/src/telmm.o\
	linux_telephony/atcmdsrv/src/mm_api.o\
	linux_telephony/atcmdsrv/src/telps.o\
	linux_telephony/atcmdsrv/src/ps_api.o\
	linux_telephony/atcmdsrv/src/telpb.o\
	linux_telephony/atcmdsrv/src/pb_api.o\
	linux_telephony/atcmdsrv/src/telss.o\
	linux_telephony/atcmdsrv/src/ss_api.o\
	linux_telephony/atcmdsrv/src/teldev.o\
	linux_telephony/atcmdsrv/src/dev_api.o\
	linux_telephony/atcmdsrv/src/teldat.o\
	linux_telephony/atcmdsrv/src/telmsg.o\
	linux_telephony/atcmdsrv/src/msg_api.o\
	linux_telephony/atcmdsrv/src/telsim.o\
	linux_telephony/atcmdsrv/src/sim_api.o\
	linux_telephony/atcmdsrv/src/telatci.o\
	linux_telephony/atcmdsrv/src/telcontroller.o\
	linux_telephony/atcmdsrv/src/teldbg.o\
	linux_telephony/atcmdsrv/src/telutl.o\
	linux_telephony/atcmdsrv/src/atcmdsvr_audio.o\
	linux_telephony/atcmdsrv/src/MarvellAmixer.o\
	genlib/atparser/src/atParser.o\
	genlib/atparser/src/convert.o\
	genlib/atparser/src/error.o\
	genlib/atparser/src/eventHandler.o \
	genlib/atparser/src/linkedList.o\
	genlib/atparser/src/malloc.o\
	genlib/atparser/src/semaphore.o\
	genlib/atparser/src/time.o\
	genlib/atparser/src/timer.o\
	genlib/atparser/src/vString.o\
	errorhandler/src/eeh_assert_notify.o\
	linux_audio/audio_ipc.o\


RTCSRVOBJS =\
	rtc_server/src/rtct_server.o\
	rtc_server/src/rtct_interface.o\
	rtc_server/src/rtct_calibration.o\
	gdd/gdd.o

.EXPORT_ALL_VARIABLES:
CFLAGS+=-I./inc/ -I$(BUILDROOT)/osa/inc \
	-I$(BUILDROOT)/include\
	-I$(BUILDROOT)/pca_components/pca_components/inc\
	-I$(BUILDROOT)/pca_components/inc\
	-I$(BUILDROOT)/pca_components/ci/inc\
	-I$(BUILDROOT)/pca_components/ci_stub/inc\
	-I$(BUILDROOT)/pca_components/ci_stub/src\
	-I$(BUILDROOT)/linux_telephony/atcmdsrv/inc\
	-I$(BUILDROOT)/genlib/atparser/inc\
	-I$(PXA_HOST_DIR)/include\
	-I$(BUILDROOT)/NVM_Proxy/include\
	-I$(BUILDROOT)/linux_audio\
	-I$(BUILDROOT)/shmem/wrapper\
	-I$(BUILDROOT)/errorhandler/inc\
	-I$(BUILDROOT)/errorhandler/src\
	-I$(BUILDROOT)/diag/diag_al\
    -I$(BUILDROOT)/diag/diag_kernel\
	-I$(BUILDROOT)/cimodemtty/src/modem_al\
	-I$(BUILDROOT)/aud_sw/audio_stub/src\
	-I$(BUILDROOT)/aud_sw/acm/inc


CFLAGS+=-DOSA_LINUX\
	-DOSA_USE_ASSERT\
	-DOSA_MSG_QUEUE\
	-DOSA_QUEUE_NAMES\
	-D_FDI_USE_OSA_\
	-DUSE_OSA_SEMA\
	-DENV_LINUX\
	-DMSL_TAVOR_LINUX\
	-DCI_STUB_CLIENT_INCLUDE\
	-DUPGRADE_R99\
	-DCCI_POSITION_LOCATION\
	-D_MICCO_B0_ \
	-DDATA_STUB_IN_KERNEL\
	-DCCI_DAT_CONTIGUOUS\
	-DCCI_CC_CONTIGUOUS\
	-DCCI_MM_CONTIGUOUS\
	-DCCI_DEV_CONTIGUOUS\
	-DCCI_PS_CONTIGUOUS\
	-DCCI_CMN_CONTIGUOUS\
	-DCCI_SIM_CONTIGUOUS\
	-DCCI_SS_CONTIGUOUS\
	-DCCI_MSG_CONTIGUOUS\
	-DCCI_PB_CONTIGUOUS\
	-DENABLE_EEH

ifeq "$(TARGET_PRODUCT)" "dkbtd"
CFLAGS+=-DTD_SUPPORT
#CP image address, and it can be used in module make process
SEAGULL_ADDR=0x7a00000
SEAGULL_SIZE=0x500000
MSA_ADDR=0x7f00000
RDATA_ADDR=0x07ee0000
else
SEAGULL_ADDR=0x7000000
SEAGULL_SIZE=0x700000
MSA_ADDR=0x7f00000
RDATA_ADDR=0x076e0000
endif



ifeq "$(CONFIG_GEN_DEBUG)" "y"
CFLAGS+=-DDEBUG_BUILD\
	-DMSL_TRACE_ON \
MAKERULES=$(PXA_SRC_DIR)/Rules.make	-DACI_KERNEL_DEBUG \
	-DDEBUG_BUILD\
	-DATCMDSRV_DEBUG\
	-DutlDEBUG\
	-DDEBUG_BUILD\
	-DMSL_TRACE_ON \
	-DMSLDL_DIRECT_ACCESS\
	-DMSL_DEBUG
endif

# additional libraries AT command server require to link
ATCMDSRV_ADD_LIBS=

# additional dependences AT command server require
ATCMDSRV_ADD_DEP=

# additional libraries sm require to link
SM_ADD_LIBS=

# additional dependences sm require
SM_ADD_DEP=

# additional libraries audio server require to link
AUDIO_ADD_LIBS=

# additional dependences audio require
AUDIO_ADD_DEP=

# enable MRD AT commands
ENABLE_MRD_CMNDS=y

# export ENABLE_MRD_CMNDS, and it can be used in diag/diag_al/Makefile
export ENABLE_MRD_CMNDS

ifeq "$(ENABLE_MRD_CMNDS)" "y"
CFLAGS+=-DAT_PRODUCTION_CMNDS\
	-I$(BUILDROOT)/MRD/inc

ATCMDSRVOBJS+=linux_telephony/atcmdsrv/src/telprod.o
ATCMDSRVOBJS+=linux_telephony/atcmdsrv/src/prod_api.o

ATCMDSRV_ADD_LIBS+=-lmrd -ldl
ATCMDSRV_ADD_DEP+=libmrd

SM_ADD_LIBS+=linux_telephony/atcmdsrv/src/prod_api.o 
SM_ADD_DEP+=linux_telephony/atcmdsrv/src/prod_api.o 

AUDIO_ADD_LIBS+=linux_telephony/atcmdsrv/src/prod_api.o
AUDIO_ADD_DEP+=linux_telephony/atcmdsrv/src/prod_api.o
endif

SM_ADD_LIBS+=diag/diag_al/diag_aud.o
SM_ADD_DEP+=diag/diag_al/diag_aud.o

# enable Labtool AT commands
ENABLE_LABTOOL_CMNDS=y

ifeq "$(ENABLE_LABTOOL_CMNDS)" "y"
CFLAGS+=-DAT_LABTOOL_CMNDS

ATCMDSRVOBJS+=linux_telephony/atcmdsrv/src/telmisc.o

ATCMDSRV_ADD_LIBS+=-llabtool
endif


.PHONY: all compile install-host install-target atcmdsrv_clean clean clean-local distclean \
	uninstall-host uninstall-target hwmap pxafs

ifeq "$(CONFIG_BIONIC)" "y"
all: compile install-host install-target pxafs
else
all: compile install-host install-target
endif

install-host:

install-target:
	$(SUDO) rm $(PXA_TARGET_FS_DIR)/tel -rf
	$(SUDO) mkdir -p $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f busybox $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f atcmdsrv $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f audioserver $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f sm $(PXA_TARGET_FS_DIR)/tel
	 $(SUDO) cp -f telmon $(PXA_TARGET_FS_DIR)/tel
ifeq "$(CONFIG_BIONIC)" "y"
	$(SUDO) rm $(PXA_TARGET_DIR)/symbols -rf
	$(SUDO) mkdir -p $(PXA_TARGET_DIR)/symbols
	$(SUDO) cp -f atcmdsrv_symbol $(PXA_TARGET_DIR)/symbols/atcmdsrv
	$(SUDO) cp -f audioserver_symbol $(PXA_TARGET_DIR)/symbols/audioserver
	$(SUDO) cp -f sm_symbol $(PXA_TARGET_DIR)/symbols/sm
	$(SUDO) cp -f telmon_symbol $(PXA_TARGET_DIR)/symbols/telmon
endif
	$(SUDO) cp -f citty/src/citty.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f ccinetdev/ccinetdev.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f cidatastub/cci_datastub.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f cidatatty/cidatattydev.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f cimodemtty/build/gs_modem.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f shmem/msocket/msocketk.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f shmem/test/msocket_testport $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag/diag_al/diag_usb_test $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag/diag_al/diag_thp_test $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag/diag_kernel/gs_diag.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag/diag_kernel/diag.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f cp_load/cploaddev.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f errorhandler/build/seh.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag_bsp.cfg $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag_usb.cfg $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag_sd.cfg $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag_fs.cfg $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag_uart.cfg $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag_udp.cfg $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f diag_tcp.cfg $(PXA_TARGET_FS_DIR)/tel
ifeq "$(TARGET_PRODUCT)" "dkbtd"
	$(SUDO) cp -f agps/gsd4t_cfg/cmcc_supl_srv/AGPS_CA.pem $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f agps/gsd4t_cfg/cmcc_supl_srv/AgpsConfig.xml $(PXA_TARGET_FS_DIR)/tel
else
	$(SUDO) cp -f agps/gsd4t_cfg/sirf_supl_srv/AGPS_CA.pem $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f agps/gsd4t_cfg/sirf_supl_srv/AgpsConfig.xml $(PXA_TARGET_FS_DIR)/tel
endif
	$(SUDO) cp -f nvm_uncheck_file.cfg $(PXA_TARGET_FS_DIR)/tel
ifeq "$(CONFIG_BIONIC)" "y"
	$(SUDO) cp -af run*.sh $(PXA_TARGET_FS_DIR)/tel
else
	if [ ! -e $(PXA_TARGET_FS_DIR)/root ];then mkdir $(PXA_TARGET_FS_DIR)/root -p;fi
	$(SUDO) cp -f linux_audio/.audiorc $(PXA_TARGET_FS_DIR)/root
	if [ ! -e $(PXA_TARGET_FS_DIR)/etc ]; then $(SUDO) mkdir -p $(PXA_TARGET_FS_DIR)/etc; fi
	$(SUDO) cp -af linux_audio/asound.conf $(PXA_TARGET_FS_DIR)/etc
	$(SUDO) cp -af ttc_run*.sh $(PXA_TARGET_FS_DIR)/tel
endif

	$(SUDO) cp -f exit_composite.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f serial_client $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f serial_cfun0 $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f vt_test $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f launch* $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f hwmap/hw.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f hwmap/hwacc $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f hwmap/hwmap.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f dropcaches $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f dvfm_control.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f backup_log.sh $(PXA_TARGET_FS_DIR)/tel
#	$(SUDO) cp -f ../cp/tavor_p_ulc/Arbel.bin $(PXA_TARGET_FS_DIR)/tel
#	$(SUDO) cp -f ../cp/tavor_p_ulc/TTC1_M05_A0_AI_Flash.bin $(PXA_TARGET_FS_DIR)/tel
#	$(SUDO) cp -f ../cp/tavor_p_ulc/TTC1_M05_A0_AI_DDR.bin $(PXA_TARGET_FS_DIR)/tel
#	if [ ! -e $(PXA_TARGET_FS_DIR)/Linux/Marvell/NVM ]; then $(SUDO) mkdir -p $(PXA_TARGET_FS_DIR)/Linux/Marvell/NVM; fi
ifeq "$(CONFIG_BIONIC)" "y"
	if [ ! -e $(PXA_TARGET_FS_DIR)/tel/configuration ]; then $(SUDO) mkdir -p $(PXA_TARGET_FS_DIR)/tel/configuration; fi
	$(SUDO) cp -f configuration/ttc/* $(PXA_TARGET_FS_DIR)/tel/configuration
else
	$(SUDO) cp -f configuration/ttc/* $(PXA_TARGET_FS_DIR)/Linux/Marvell/NVM
endif
	$(SUDO) cp -f Boerne_DIAG.mdb.txt $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f ramdump_loglist $(PXA_TARGET_FS_DIR)/tel
#  For ttc product, the 3G/GSM Com cfg is same	
ifeq "$(CONFIG_BIONIC)" "y"
ifeq ($(filter $(TARGET_PRODUCT), td_jil, td_dkb, dkbtd),)
	$(SUDO) cp -f $(PXA_TARGET_FS_DIR)/tel/configuration/COMCfg.csv.gsm $(PXA_TARGET_FS_DIR)/tel/configuration/COMCfg.csv.td2g
	$(SUDO) cp -f configuration/ttc/COMCfg.csv.3g $(PXA_TARGET_FS_DIR)/tel/configuration/COMCfg.csv.gsm
endif
else
ifeq "$(CONFIG_CPU_TAVOR_L)" "y"
	$(SUDO) cp -f $(PXA_TARGET_FS_DIR)/Linux/Marvell/NVM/COMCfg.csv.gsm $(PXA_TARGET_FS_DIR)/Linux/Marvell/NVM/COMCfg.csv.td2g
	$(SUDO) cp -f configuration/ttc/COMCfg.csv.3g $(PXA_TARGET_FS_DIR)/Linux/Marvell/NVM/COMCfg.csv.gsm
endif
endif
ifneq "$(CONFIG_BIONIC)" "y"
	if [ ! -e $(PXA_TARGET_FS_DIR)/usr/lib ];then mkdir $(PXA_TARGET_FS_DIR)/usr/lib -p;fi
	$(SUDO) cp -f linux_audio/alsa_plugin/phone/.libs/libasound_module_ctl_phone.so $(PXA_TARGET_FS_DIR)/usr/lib
	$(SUDO) cp -f linux_audio/alsa_plugin/phone/.libs/libasound_module_pcm_phone.so $(PXA_TARGET_FS_DIR)/usr/lib
endif
	$(SUDO) cp -f load_mfg_8787.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f load_prod_8787.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f unload_8787.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f open_aud_path_fm_headset.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f open_aud_path_fm_speaker.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f close_aud_path_fm.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f open_aud_path_hifi_headset.sh $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f close_aud_path_hifi_headset.sh $(PXA_TARGET_FS_DIR)/tel

compile: cp_load_drv msocketk cci_datastub_drv citty_drv cidatatty_drv ccinetdev_drv gs_modem_drv diag_drv seh_drv vt_test serial_client serial_cfun0 diag_usb_test  atcmdsrv audioserver sm telmon libeeh libasound_module_ctl_phone.so diagdb msocket_testport hwmap

atcmdsrv_clean:
	-rm $(ATCMDSRVOBJS)
	-rm linux_telephony/atcmdsrv/src/main.o
	make -C pca_components -f Makefile.cci clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C NVM_Proxy clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C osa clean MAKERULES=$(PXA_SRC_DIR)/Rules.make

serial_client:
	make -C linux_telephony/atcmdsrv/test MAKERULES=$(PXA_SRC_DIR)/Rules.make
	mv linux_telephony/atcmdsrv/test/serial_client .
	
serial_cfun0:
	make -C linux_telephony/atcmdsrv/test MAKERULES=$(PXA_SRC_DIR)/Rules.make
	mv linux_telephony/atcmdsrv/test/serial_cfun0 .

vt_test:
	 make -C linux_telephony/atcmdsrv/vttest MAKERULES=$(PXA_SRC_DIR)/Rules.make
	 mv linux_telephony/atcmdsrv/vttest/vt_test .

msocketk:
	make -C shmem/msocket PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)

msocket_testport:
	make -C shmem/test -f Makefile.testport MAKERULES=$(PXA_SRC_DIR)/Rules.make

diag_drv:
	make -C diag/diag_kernel PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)

diag_usb_test:
	 make -C diag/diag_al -f Makefile.test MAKERULES=$(PXA_SRC_DIR)/Rules.make
	
citty_drv:
	make -C citty PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	
ccinetdev_drv:
	make -C ccinetdev PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	
cci_datastub_drv:
	make -C cidatastub PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)

cidatatty_drv:
	make -C cidatatty PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)

gs_modem_drv:
	make -C cimodemtty PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	
libcci:
	make -C pca_components -f Makefile.cci MAKERULES=$(PXA_SRC_DIR)/Rules.make

libosa:
	make -C osa MAKERULES=$(PXA_SRC_DIR)/Rules.make

libnvmproxy:
	make -C NVM_Proxy MAKERULES=$(PXA_SRC_DIR)/Rules.make
	
libshm:
	make -C shmem/wrapper MAKERULES=$(PXA_SRC_DIR)/Rules.make

libdiag:
	make -C diag/diag_al MAKERULES=$(PXA_SRC_DIR)/Rules.make
libmodem:
	make -C cimodemtty/src/modem_al MAKERULES=$(PXA_SRC_DIR)/Rules.make

libmaudio:
	make -C aud_sw/audio_stub MAKERULES=$(PXA_SRC_DIR)/Rules.make

libacm:
	make -C aud_sw/acm  MAKERULES=$(PXA_SRC_DIR)/Rules.make

libaudioserver:
	make -C linux_audio -f Makefile.audioserver MAKERULES=$(PXA_SRC_DIR)/Rules.make

ifeq "$(CONFIG_BIONIC)" "y"
libasound_module_ctl_phone.so:
else
libasound_module_ctl_phone.so: audioserver
	make -C linux_audio/alsa_plugin/phone -f Makefile.phone MAKERULES=$(PXA_SRC_DIR)/Rules.make
endif

atcmdsrv:linux_telephony/atcmdsrv/src/atcmdsvr_audio.i atcmdsrv_clean libcci libosa libshm libmodem libdiag $(ATCMDSRVOBJS) linux_telephony/atcmdsrv/src/main.o $(BIONIC_EXTRA_OBJECTS) $(ATCMDSRV_ADD_DEP) atcmd_diag
	$(CC) $(BIONIC_LINK) $(CFLAGS) $(COMLIBS) -o atcmdsrv linux_telephony/atcmdsrv/src/main.o $(ATCMDSRVOBJS) $(DIAG_LINUX_OBJ_ATCMD) -L. -lcci -losa -lshm -lmodem -lasound -ldiag $(BIONIC_EXTRA_OBJECTS) $(BIONIC_POSTLINK) $(ATCMDSRV_ADD_LIBS)
ifeq  "$(CONFIG_BIONIC)" "y"
	$(SUDO) mv $@ $@_symbol
	$(STRIP) $(STRIP_FLAGS) $@_symbol --outfile $@
endif
	@echo "DONE"

audioserver: libacm libmaudio libosa libaudioserver libshm libdiag linux_audio/audio_server.o errorhandler/src/eeh_assert_notify.o errorhandler/src/fdi_file.o $(BIONIC_EXTRA_OBJECTS) aud_diag $(AUDIO_ADD_DEP)
	$(CC) $(BIONIC_LINK) $(CFLAGS) $(COMLIBS) -o audioserver linux_audio/audio_server.o errorhandler/src/eeh_assert_notify.o errorhandler/src/fdi_file.o $(DIAG_LINUX_OBJ_AUD) -lstdc++ -laudioserver -L$(PXA_HOST_DIR)/lib -lasound -L. -losa -lacm -lmaudio -lshm -ldiag $(BIONIC_EXTRA_OBJECTS) $(BIONIC_POSTLINK) $(AUDIO_ADD_LIBS)
ifeq  "$(CONFIG_BIONIC)" "y"
	$(SUDO) mv $@ $@_symbol
	$(STRIP) $(STRIP_FLAGS) $@_symbol --outfile $@
endif
	@echo "DONE"

sm: libosa libnvmproxy libshm libeeh libdiag errorhandler/src/eeh_main.o errorhandler/src/eeh_assert_notify.o linux_telephony/atcmdsrv/src/MarvellAmixer.o  $(BIONIC_EXTRA_OBJECTS) sm_diag $(SM_ADD_DEP)
	$(CCOMPILER) $(BIONIC_LINK) $(CFLAGS) $(COMLIBS) -o sm errorhandler/src/eeh_main.o errorhandler/src/eeh_assert_notify.o linux_telephony/atcmdsrv/src/MarvellAmixer.o $(DIAG_LINUX_OBJ_TEL) -lstdc++ -L. -lnvmproxy -lshm -losa -leeh -ldiag -lasound $(SM_ADD_LIBS) $(BIONIC_POSTLINK) $(BIONIC_EXTRA_OBJECTS)
ifeq  "$(CONFIG_BIONIC)" "y"
	$(SUDO) mv $@ $@_symbol
	$(STRIP) $(STRIP_FLAGS) $@_symbol --outfile $@
endif
	@echo "DONE"

telmon: libeeh telephony_monitor/main.o
	$(CCOMPILER) $(BIONIC_LINK) $(CFLAGS) $(COMLIBS) -o $@ telephony_monitor/main.o -L. -leeh $(BIONIC_POSTLINK) $(BIONIC_EXTRA_OBJECTS)
ifeq  "$(CONFIG_BIONIC)" "y"
	$(SUDO) mv $@ $@_symbol
	$(STRIP) $(STRIP_FLAGS) $@_symbol --outfile $@
endif
	@echo "DONE"

sm_diag:
	 make -C prepass compile_tel
	 $(CC) $(CFLAGS) -c -o $(DIAG_LINUX_OBJ_TEL) $(DIAG_LINUX_SRC_TEL)
aud_diag:
	 make -C prepass compile_aud
	 $(CC) $(CFLAGS) -c -o $(DIAG_LINUX_OBJ_AUD) $(DIAG_LINUX_SRC_AUD)
atcmd_diag:
	 make -C prepass compile_atcmd
	 $(CC) $(CFLAGS) -c -o $(DIAG_LINUX_OBJ_ATCMD) $(DIAG_LINUX_SRC_ATCMD)
diagdb:
	make -C prepass merge
libeeh:
	make -C errorhandler -f Makefile.app MAKERULES=$(PXA_SRC_DIR)/Rules.make

hwmap:
	make -C hwmap MAKERULES=$(PXA_SRC_DIR)/Rules.make

seh_drv:
	make -C errorhandler -f Makefile.drv PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	
cp_load_drv:
	make -C cp_load PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)

ifeq "$(ENABLE_MRD_CMNDS)" "y"
libmrd:
	make -C MRD MAKERULES=$(PXA_SRC_DIR)/Rules.make
endif

pxafs:
	@echo "***********************************************************************"
	@echo "**                    Create pxafs.img Yaffs2 FS                     **"
	@echo "***********************************************************************"
	$(SUDO) $(BUILDROOT)/../../../../../out/host/linux-x86/bin/mkyaffs2image $(PXA_TARGET_FS_DIR) $(PXA_TARGET_DIR)/pxafs.img;
	$(SUDO) chmod a+r $(PXA_TARGET_DIR)/pxafs.img
ifeq  "$(CONFIG_BIONIC)" "y"
	$(SUDO) tar czf $(PXA_TARGET_DIR)/pxa_symbols.tgz -C $(PXA_TARGET_DIR) symbols/
	$(SUDO) chmod a+r $(PXA_TARGET_DIR)/pxa_symbols.tgz
endif



clean:
	make -C NVM_Proxy clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C osa clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C pca_components -f Makefile.cci clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C shmem/wrapper -f Makefile clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C linux_telephony/atcmdsrv/test clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C shmem/msocket clean
	make -C shmem/test -f Makefile.testport clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C diag/diag_al -f Makefile.test clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C diag/diag_kernel clean
	make -C cimodemtty clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C cimodemtty/src/modem_al clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C diag/diag_al clean
	make -C errorhandler -f Makefile.app clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C errorhandler -f Makefile.drv clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C hwmap clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
	make -C ccinetdev clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C cidatastub clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C cidatatty clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C citty clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C cp_load clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C linux_audio -f Makefile.audioserver clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C aud_sw/acm clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C aud_sw/audio_stub clean PXA_SRC_PVK_LNX_DIR=$(PXA_SRC_PVK_LNX_DIR)
	make -C linux_audio/alsa_plugin/phone -f Makefile.phone clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
ifeq "$(ENABLE_MRD_CMNDS)" "y"
	make -C MRD clean MAKERULES=$(PXA_SRC_DIR)/Rules.make
endif


	-rm $(ATCMDSRVOBJS)
	-rm linux_telephony/atcmdsrv/src/main.o
	-rm atcmdsrv
	-rm sm
	-rm $(DIAG_LINUX_OBJ_TEL)
	-rm audioserver
	-rm telephony_monitor/main.o
	-rm telmon
ifeq "$(CONFIG_BIONIC)" "y"
	-rm atcmdsrv_symbol
	-rm sm_symbol
	-rm audioserver_symbol
	-rm telmon_symbol
endif
	-rm serial_client
	-rm serial_cfun0
	-rm vt_test
	$(SUDO) rm $(PXA_TARGET_FS_DIR)/tel -rf
	-rm -f $(PXA_TARGET_FS_DIR)/usr/lib/libasound_module_ctl_phone.so
	-rm -f $(PXA_TARGET_FS_DIR)/usr/lib/libasound_module_pcm_phone.so
	-rm $(PXA_TARGET_FS_DIR)/usr/sbin -rf
ifeq "$(CONFIG_BIONIC)" "y"
	$(SUDO) rm $(PXA_TARGET_DIR) -rf
else
	-rm -f $(PXA_TARGET_FS_DIR)/root/.audiorc
	-rm -f $(PXA_TARGET_FS_DIR)/etc/asound.conf
endif
	-rm -f $(DIAG_LINUX_OBJ_TEL)
	-rm Boerne_DIAG.mdb.txt
	make -C prepass clean
	-find -name '*.i' -exec rm {} \;
	-find -name '*.pp' -exec rm {} \;
	-find -name '*.cmd' -exec rm {} \;
	-find -name '*.o' -exec rm {} \;
	-find -name '*.order' -exec rm {} \;
