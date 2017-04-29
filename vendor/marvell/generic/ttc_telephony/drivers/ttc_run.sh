insmod cploaddev.ko
#echo 0 > /sys/devices/system/cpu/cpu0/cp
#cat Arbel.bin > /dev/arbel_bin
#cat TTC1_M05_A0_AI_Flash.bin > /dev/m05_flash
#cat TTC1_M05_A0_AI_DDR.bin > /dev/m05_ddr
echo 1 > /sys/devices/system/cpu/cpu0/cp

export NVM_ROOT_DIR="/Linux/Marvell/NVM"
if [ ! -e $NVM_ROOT_DIR ]; then
        mkdir -p $NVM_ROOT_DIR
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

# delay 1 second to get udevd notified about the devices creation
# in the modules above, before atcmdsrv and audioserver start
sleep 1
./sm -D yes &
sleep 2
./atcmdsrv -l 6 -t yes -M yes &
sleep 2
./audioserver -t yes &
