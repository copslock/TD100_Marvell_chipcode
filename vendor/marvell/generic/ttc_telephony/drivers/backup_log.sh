#!/system/bin/sh

file_name="/data/atcmd.log"
file_exist=`ls $file_name`
case "$file_exist" in
    $file_name)
	mv /data/atcmd.log /data/atcmd.log.bak;
	;;
	*)
	;;
esac

file_name="/data/sm.log"
file_exist=`ls $file_name`
case "$file_exist" in
    $file_name)
	mv /data/sm.log  /data/sm.log.bak;
	;;
	*)
	;;
esac

file_name="/data/ad.log"
file_exist=`ls $file_name`
case "$file_exist" in
    $file_name)
	mv /data/ad.log  /data/ad.log.bak;
	;;
	*)
	;;
esac

file_name="/data/radio.log"
file_exist=`ls $file_name`
case "$file_exist" in
    $file_name)
	mv /data/radio.log  /data/radio.log.bak;
	;;
	*)
	;;
esac

file_name=""$NVM_ROOT_DIR"/AppErrorStatistic.log"
file_exist=`ls $file_name`
case "$file_exist" in
    $file_name)
	mv $NVM_ROOT_DIR/AppErrorStatistic.log  $NVM_ROOT_DIR/AppErrorStatistic.log.bak;
	;;
	*)
	;;
esac

file_name=""$NVM_ROOT_DIR"/CpErrorStatistic.log"
file_exist=`ls $file_name`
case "$file_exist" in
    $file_name)
	mv $NVM_ROOT_DIR/CpErrorStatistic.log  $NVM_ROOT_DIR/CpErrorStatistic.log.bak;
	;;
	*)
	;;
esac

