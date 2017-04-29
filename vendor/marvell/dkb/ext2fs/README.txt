GPT partition information

partition			start block address		end block address	size in block	size in byte	device name
-------------------------------------------------------------------------------------------------------------
UBOOT               0x0008 0000             0x0010 0000         0x0008 0000     512K            mmcblk0p1
Android Ramdisk     0x0010 0000             0x0014 0000         0x0004 0000     256K            mmcblk0p2
Security Signature  0x0014 0000             0x0018 0000         0x0004 0000     256K            mmcblk0p3
Arbel Image         0x0018 0000             0x0088 0000         0x0070 0000     7M              mmcblk0p4
MSA Image           0x0088 0000             0x0098 0000         0x0010 0000     1M              mmcblk0p5
Kernel              0x0098 0000             0x00d8 0000         0x0040 0000     4M              mmcblk0p6
Maintenance Kernel  0x00d8 0000             0x0118 0000         0x0040 0000     4M              mmcblk0p7
Recovery Ramdisk    0x0118 0000             0x0168 0000         0x0050 0000     5M              mmcblk0p8
Misc                0x0168 0000             0x016c 0000         0x0004 0000     256K            mmcblk0p9
Cache               0x016c 0000             0x0680 0000         0x0514 0000     81M256K         mmcblk0p10
Android System      0x0680 0000             0x1080 0000         0x00a0 0000     160M            mmcblk0p11
Android Data        0x1080 0000             0x1e40 0000         0x0dc0 0000     220M            mmcblk0p12
NVM                 0x1e40 0000             0x1ec0 0000         0x0080 0000     8M              mmcblk0p13
Telephony           0x1ec0 0000             0x1f40 0000         0x0080 0000     8M              mmcblk0p14
Masss Storage       0x1f40 0000             0xebff be00         0xccbf be00     3275M1007.5K    mmcblk0p15
