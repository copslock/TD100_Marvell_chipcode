#!/system/bin/sh

busybox killall -9 pppd
stop ril-daemon
busybox killall -9 sm atcmdsrv audioserver

rmmod cidatattydev
rmmod seh
rmmod gs_diag
rmmod diag
rmmod gs_modem
rmmod ccinetdev
rmmod cci_datastub
rmmod msocketk
rmmod citty
echo 0 > /sys/devices/system/cpu/cpu0/cp
rmmod cploaddev