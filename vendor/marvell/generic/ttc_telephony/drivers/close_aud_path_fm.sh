#!/system/bin/sh

# disable fm headset, 0x8030064
amixer -Dphone cset numid=36, 134414436

# disable fm speaker, 0x8030064
amixer -Dphone cset numid=12, 134414436

