#!/system/bin/sh
cd /marvell/tel
./serial_cfun0
# echo 1 > /sys/class/graphics/fb0/blank
echo mem > /sys/power/state
