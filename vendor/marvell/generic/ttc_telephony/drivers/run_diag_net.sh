#!/system/bin/sh
insmod cploaddev.ko
#echo 0 > /sys/devices/system/cpu/cpu0/cp 
#cat Arbel.bin > /dev/arbel_bin
#cat TTC1_M05_A0_AI_Flash.bin > /dev/m05_flash
#cat TTC1_M05_A0_AI_DDR.bin > /dev/m05_ddr
echo 1 > /sys/devices/system/cpu/cpu0/cp 

export NVM_ROOT_DIR="/data/Linux/Marvell/NVM"
if [ ! -e $NVM_ROOT_DIR ]; then
	mkdir -p $NVM_ROOT_DIR
fi

insmod citty.ko
insmod msocketk.ko
insmod cci_datastub.ko
insmod ccinetdev.ko
insmod diag.ko
#insmod gs_diag.ko
insmod g_ether.ko
insmod seh.ko
insmod cidatattydev.ko
ifconfig usb0 192.168.1.101 netmask 255.255.255.0 up
./sm -M yes -D yes &
./atcmdsrv &
./audioserver &
