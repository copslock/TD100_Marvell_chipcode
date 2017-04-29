#!/bin/sh
# This script is for power off charge,when system is power down,plug in usb will do power
# off charge with green led blinking to alert
# 1 Check the condition include  bat_present ,chg_online and on-key info;
# 2 If above condition is statified, will do charging without boot up anroid,and do led
#   blinking for user alert ,so called power off charge;
# 3 When  plug out USB charger ,shut down system;
#   When  plut out battery, shut down system;
# TODO:
# 1 When there is better user space App, can show some image or animation to show power-off
#   charge user alert;
# 2 When system is boot up with battyer only, it will check battery voltage in uboot, if it
#   is too low, should abandon booting process

do_charge_led_blinking() {
	cd /sys/class/leds/led0-green/
	echo timer > trigger
	echo 1200 > delay_on
	echo 1200 > delay_off
}
undo_led_blinking() {
	cd /sys/class/leds/led0-green/
	echo none > trigger
	[ -f delay_on ] && echo 0 > delay_on
	[ -f delay_off ] && echo 0 > delay_off
	echo 0 > brightness
}

LED_ON=0
CHG_DONE_THRESHOLD=100
CURRENT_STATUS="Unknown"
FLAG=0
#mount system for reboot effective
mkdir /system
#board 920 and 920H will mount /system to different block
if [ $IS_MTD -eq 1 ] ; then
 	mount -t yaffs /dev/mtdblock11 /system
elif [ $IS_MTD -eq 0 ] ; then
 	mount -t ext4 /dev/mmcblk0p11 /system
else
	echo "IS_MTD = $IS_MTD"
fi

#get chg and bat information
#BAT_CAP=`cat /sys/class/power_supply/battery/capacity`
ON_KEY_INT=`cat /proc/interrupts`
BAT_PRESENT=`cat /sys/class/power_supply/battery/present`
USB_ONLINE=`cat /sys/class/power_supply/usb/online`
AC_ONLINE=`cat /sys/class/power_supply/ac/online`
CHG_STATUS=`cat /sys/class/power_supply/battery/status`
if [ $USB_ONLINE -eq 1 -o $AC_ONLINE -eq 1 ]; then
	CHG_ONLINE=1
else
	CHG_ONLINE=0
fi

#polling bat and charger status, and do diffrent activity
# clear led setting
undo_led_blinking
while [ $BAT_PRESENT -eq 1 ]
do
	echo "chg online:$CHG_ONLINE"
	echo "bat online:$BAT_PRESENT"
	#echo "on key interrupt:$ON_KEY_INT"
	cat /sys/class/power_supply/battery/uevent
	for arg in $ON_KEY_INT ; do
		ARG1=`echo $ARG2`
		ARG2=`echo $ARG3`
		ARG3=`echo $arg`
		# set flag for check on-key irqnum,note
		if [ $arg == "onkey" ]; then
			# on key interrupt num>= 1,so on-key pressed
			if [ $ARG1 -ge 1 ]; then
				# on key interrupt
				echo " onkey pressed detcet---"
				echo -0x3e > /proc/driver/pm860x_charger
				echo 0x1 > /proc/driver/pm860x_charger
				undo_led_blinking
				/system/bin/reboot
				exit 0
			fi
		fi
	done
	if [ $CHG_ONLINE -eq 1 ]; then
		echo "status:current=$CURRENT_STATUS ,new_read=$CHG_STATUS"
		if [ $CURRENT_STATUS != $CHG_STATUS ]; then
			CURRENT_STATUS=$CHG_STATUS;
			undo_led_blinking
			LED_ON=0
		fi
		if [ $CHG_STATUS == "Charging" ]; then
			echo "battery charging ..."
			if [ $LED_ON -ne 1 ];then
				echo "start green led blinking..."
				echo 0 > /sys/class/backlight/portofino-bl/brightness
				do_charge_led_blinking
				LED_ON=1
				/system/bin/charge
			fi
		elif [ $CHG_STATUS == "Discharging" ]; then
			echo "battery discharging ..."
		fi
	else
		echo "usb plug out,power off system..."
		undo_led_blinking
		/system/bin/reboot -p
		exit 0
	fi
	sleep 3 
	ON_KEY_INT=`cat /proc/interrupts`
	USB_ONLINE=`cat /sys/class/power_supply/usb/online`
	AC_ONLINE=`cat /sys/class/power_supply/ac/online`
	BAT_PRESENT=`cat /sys/class/power_supply/battery/present`
	CHG_STATUS=`cat /sys/class/power_supply/battery/status`
	if [ $USB_ONLINE -eq 1 -o $AC_ONLINE -eq 1 ]; then
		CHG_ONLINE=1
	else
		CHG_ONLINE=0
	fi
done
# battery plug out
if [ $BAT_PRESENT -ne 1 ]; then
	undo_led_blinking
	/system/bin/reboot -p
	exit 0
fi

