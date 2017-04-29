#!/system/bin/sh
insmod cploaddev.ko
#echo 0 > /sys/devices/system/cpu/cpu0/cp 
#cat Arbel.bin > /dev/arbel_bin
#cat TTC1_M05_A0_AI_Flash.bin > /dev/m05_flash
#cat TTC1_M05_A0_AI_DDR.bin > /dev/m05_ddr
echo 1 > /sys/devices/system/cpu/cpu0/cp 

export NVM_ROOT_DIR="/data/Linux/Marvell/NVM"
export MARVELL_RW_DIR="/data/acat"
if [ ! -e $NVM_ROOT_DIR ]; then
	mkdir -p $NVM_ROOT_DIR
fi

if [ ! -e $MARVELL_RW_DIR ]; then
	mkdir -p $MARVELL_RW_DIR
fi
insmod citty.ko
insmod msocketk.ko
insmod cci_datastub.ko
insmod ccinetdev.ko
insmod gs_modem.ko
insmod diag.ko
insmod gs_diag.ko
insmod seh.ko
insmod cidatattydev.ko

# change devices owner to system
chown system.system /dev/cctdev*
chown system.system /dev/msocket
chown system.system /dev/ccidatastub
chown system.system /dev/seh
chown system.system /dev/diagdatastub
chown system.system /dev/ttymodem0
chown system.system /dev/ttydiag0
chown system.system /dev/block/mtdblock3
chown system.system /dev/block/mmcblk0p3
chown radio.bluetooth /dev/cctdatadev1

setprop sys.telephonymoduleloglevel 8
/marvell/tel/sm -M yes -D yes &
/marvell/tel/atcmdsrv -M yes &
/marvell/tel/audioserver &
start ril-daemon
