
include vendor/marvell/$(TARGET_PRODUCT)/gst_modules.mk

# graphics modules
GRANDFATHERED_USER_MODULES += \
	libGAL \
	libGLESv1_CM_MRVL \
	libGLESv2_MRVL \
	libEGL_MRVL \
	libgcu \
	librs_jni \
	libRS

# devices modules
GRANDFATHERED_USER_MODULES += \
	libsensor \
	sensors.default \
	lights.default \
	gps.default  \
	libfmstub \
	com.marvell.fmmanager \
	FMRadioServer \
	libif228 \
	libsim \
	libcmmbstack \
	libinno \
	libsiano \
	libsmscontrol \
	libsmsmbbms \
	libmbbmsengine \
	MBBMSService \
	com.marvell.cmmb.aidl \
	com.marvell.cmmb.aidl.xml \
	IF228FW.bin \
	cmmb_ming_app.inp \
	libgstcmmbsplitter \
	libcmmb_playengine \
	cmmbplayengine.conf \
	libMarvellWireless \
	MarvellWirelessDaemon \
	libwpa_client \
	libecc \
	libsms4 \
	chat \
	overlay.default \
	pxa27x-keypad.kcm \
	libNetworkInfo \
	libire \
	libusbsetting \
	BatteryCalibration \
	libasound_module_pcm_phone \
	labtool \
	liblabtool \
	mrvl \
	libmarvell-ril \
	i2cdetect \
	i2cdump \
	i2cset \
	i2cget \
	libasound_module_pcm_ippvp \
	libasound_module_pcm_vtrec \
	libPowerSetting \
	powerpolicy \
	bmmtest

# audio modules
GRANDFATHERED_USER_MODULES += \
	libaudiopath \
	libasound \
	libacm \
	libasound_module_ctl_phone \
	libOrgan \
	libFileMonitorService \
	FileMonitorService

# multimedia modules except gst, include IPP, bmm/pmem, opencore...
GRANDFATHERED_USER_MODULES += \
	libippvp \
	libicrctrlsvr \
	libippsp \
	libcodecwmadec \
	libcodecaacdec \
	libcodecaacenc \
	libcodecmp3dec \
	libcodech263dec \
	libcodech264dec \
	libcodecmpeg4dec \
	libcodecmpeg2dec \
	libcodecwmvdec \
	libcodecamrnbdec \
	libcodecamrnbenc \
	libcodecamrwbdec \
	libcodecamrwbenc \
	libcodecg711 \
	libcodecjpegdec \
	libcodecjpegenc \
	libcodecmpeg4enc \
	libcodech264enc \
	libcodech263enc \
	libippexif \
	libcameraengine \
	libcamera \
	libsensorhal_extisp \
	libavutil \
	libavcodec \
	libavformat \
	libmiscgen \
	libMrvlOmx \
	libstagefright_hardware_renderer \
	libphycontmem \
	libpmemhelper \
	libbmm \
	libcnm \
	libcnm \
	libcnmhal \
	libcnmhal \
	libcodeccnmdec \
	libcodeccnmdec \
	libcodeccnmenc \
	libcodeccnmenc \
	libippcam \
	libvpx_ipp

# opencore related modules
GRANDFATHERED_USER_MODULES += \
	libopencorehw \
	libMrvlOmxWrapper \
	pvplayer \
	libopencore_common \
	libopencore_author \
	libopencore_player \
	libomx_sharedlibrary \
	libomx_avcdec_sharedlibrary \
	libomx_m4vdec_sharedlibrary \
	libomx_aacdec_sharedlibrary \
	libomx_amrdec_sharedlibrary \
	libomx_mp3dec_sharedlibrary \
	libomx_amrenc_sharedlibrary \
	libopencore_net_support \
	libopencore_downloadreg \
	libopencore_download \
	libopencore_rtspreg \
	libopencore_rtsp \
	libopencore_mp4localreg \
	libopencore_mp4local \
	lib_il_aacdec_wmmx2lnx \
	lib_il_mp3dec_wmmx2lnx \
	lib_il_basecore_wmmx2lnx \
	lib_il_ippomxmem_wmmx2lnx \
	lib_il_mpeg4aspdec_wmmx2lnx \
	lib_il_h264dec_wmmx2lnx \
	lib_il_mpeg4enc_wmmx2lnx \
	lib_il_h264enc_wmmx2lnx \
	lib_il_aacenc_wmmx2lnx \
	lib_il_amrnbenc_wmmx2lnx \
	lib_il_amrnbdec_wmmx2lnx \
	lib_il_h263enc_wmmx2lnx \
	lib_il_h263dec_wmmx2lnx \
	lib_il_amrwbdec_wmmx2lnx \
	lib_il_codadx8dec_wmmx2lnx \
	lib_il_codadx8enc_wmmx2lnx \
	libMrvlOmxConfig \
	libomx_mrvl_sharedlibrary

# MVT related modules
GRANDFATHERED_USER_MODULES += \
	libopencore_2way \
	libopencore_vtengine \
	libvtengine_client \
	libvtengine_jni \
	libvtengineservice \
	libvtengine \
	vt_console_app \
	vtengineserver \
	CallSetting \
	pv2way_console_app \
	pvlog
# Wi-Fi Direct related modules
GRANDFATHERED_USER_MODULES+= \
	dhcp_announce \
	mwu \
	libmwu_client \
	mwu_cli \
	wifidir_init \
	uap \
	wifidirect_defaults \
	WifiDirectHttpDemoApp \
	libmime4j \
	libhttpclient \
	libhttpcore \
	libhtpmime
