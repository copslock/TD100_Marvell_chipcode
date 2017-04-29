#!/system/bin/sh

# disable fm headset, 0x8030064
amixer -Dphone cset numid=36, 134414436

# enable fm speaker, with volume 100, 0x8030064
amixer -Dphone cset numid=11, 134414436

