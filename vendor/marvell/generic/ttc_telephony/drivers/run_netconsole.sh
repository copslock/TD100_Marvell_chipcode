#!/system/bin/sh
#insmod cploaddev.ko
#echo 0 > /sys/devices/system/cpu/cpu0/cp 
#cat Arbel.bin > /dev/arbel_bin
#cat TTC1_M05_A0_AI_Flash.bin > /dev/m05_flash
#cat TTC1_M05_A0_AI_DDR.bin > /dev/m05_ddr
#echo 1 > /sys/devices/system/cpu/cpu0/cp 

export NVM_ROOT_DIR="/data/Linux/Marvell/NVM"
if [ ! -e $NVM_ROOT_DIR ]; then
	mkdir -p $NVM_ROOT_DIR
fi

#insmod citty.ko
#insmod msocketk.ko
#insmod cci_datastub.ko
#insmod ccinetdev.ko
#insmod diag.ko

#insmod gs_diag.ko
#insmod g_android.ko
insmod g_ether.ko
ifconfig usb0 192.168.1.101 netmask 255.255.255.0 up
insmod netconsole.ko netconsole=4444@192.168.1.101/usb0,6666@192.168.1.100/a2:35:82:1f:0b:01
insmod g_serial.ko
ln -s /dev/ttyGS0 /dev/ttygs
getty 115200 /dev/ttygs &

#insmod seh.ko
#insmod cidatattydev.ko

# delay 1 second to get udevd notified about the devices creation
# in the modules above, before atcmdsrv and audioserver start
#sleep 1
#./sm -f sm.log&
#sleep 2
#./atcmdsrv -f atcmd.log&
#./cp_levante.sh
