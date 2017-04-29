#!/system/bin/sh

# disable fm speaker, 0x8030064
amixer -Dphone cset numid=12, 134414436

# enable fm headset, with volume 100, 0x8030064
amixer -Dphone cset numid=35, 134414436

