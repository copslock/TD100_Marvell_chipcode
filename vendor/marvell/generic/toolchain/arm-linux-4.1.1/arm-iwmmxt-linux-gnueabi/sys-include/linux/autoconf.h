/*
 * Automatically generated C config: don't edit
 * Linux kernel version: 2.6.14.6
 * Tue Nov  4 11:23:58 2008
 */
#define AUTOCONF_INCLUDED
#define CONFIG_ARM 1
#define CONFIG_MMU 1
#define CONFIG_UID16 1
#define CONFIG_RWSEM_GENERIC_SPINLOCK 1
#define CONFIG_GENERIC_CALIBRATE_DELAY 1

/*
 * Code maturity level options
 */
#define CONFIG_EXPERIMENTAL 1
#define CONFIG_CLEAN_COMPILE 1
#define CONFIG_BROKEN_ON_SMP 1
#define CONFIG_INIT_ENV_ARG_LIMIT 32

/*
 * General setup
 */
#define CONFIG_LOCALVERSION ""
#define CONFIG_LOCALVERSION_AUTO 1
#define CONFIG_SWAP 1
#define CONFIG_SYSVIPC 1
#undef CONFIG_POSIX_MQUEUE
#undef CONFIG_BSD_PROCESS_ACCT
#define CONFIG_SYSCTL 1
#undef CONFIG_AUDIT
#define CONFIG_HOTPLUG 1
#define CONFIG_KOBJECT_UEVENT 1
#undef CONFIG_IKCONFIG
#define CONFIG_INITRAMFS_SOURCE ""
#undef CONFIG_EMBEDDED
#define CONFIG_KALLSYMS 1
#undef CONFIG_KALLSYMS_ALL
#undef CONFIG_KALLSYMS_EXTRA_PASS
#define CONFIG_PRINTK 1
#define CONFIG_BUG 1
#define CONFIG_BASE_FULL 1
#define CONFIG_FUTEX 1
#define CONFIG_EPOLL 1
#define CONFIG_CC_OPTIMIZE_FOR_SIZE 1
#define CONFIG_SHMEM 1
#define CONFIG_CC_ALIGN_FUNCTIONS 0
#define CONFIG_CC_ALIGN_LABELS 0
#define CONFIG_CC_ALIGN_LOOPS 0
#define CONFIG_CC_ALIGN_JUMPS 0
#undef CONFIG_TINY_SHMEM
#define CONFIG_BASE_SMALL 0

/*
 * Loadable module support
 */
#define CONFIG_MODULES 1
#undef CONFIG_MODULE_UNLOAD
#define CONFIG_OBSOLETE_MODPARM 1
#undef CONFIG_MODVERSIONS
#undef CONFIG_MODULE_SRCVERSION_ALL
#undef CONFIG_KMOD

/*
 * System Type
 */
#undef CONFIG_ARCH_CLPS7500
#undef CONFIG_ARCH_CLPS711X
#undef CONFIG_ARCH_CO285
#undef CONFIG_ARCH_EBSA110
#undef CONFIG_ARCH_CAMELOT
#undef CONFIG_ARCH_FOOTBRIDGE
#undef CONFIG_ARCH_INTEGRATOR
#undef CONFIG_ARCH_IOP3XX
#undef CONFIG_ARCH_IXP4XX
#undef CONFIG_ARCH_IXP2000
#undef CONFIG_ARCH_L7200
#define CONFIG_ARCH_PXA 1
#undef CONFIG_ARCH_RPC
#undef CONFIG_ARCH_SA1100
#undef CONFIG_ARCH_S3C2410
#undef CONFIG_ARCH_SHARK
#undef CONFIG_ARCH_LH7A40X
#undef CONFIG_ARCH_OMAP
#undef CONFIG_ARCH_VERSATILE
#undef CONFIG_ARCH_IMX
#undef CONFIG_ARCH_H720X
#undef CONFIG_ARCH_AAEC2000

/*
 * Intel PXA2xx Implementations
 */
#undef CONFIG_ARCH_LUBBOCK
#define CONFIG_MACH_MAINSTONE 1
#undef CONFIG_ARCH_PXA_IDP
#undef CONFIG_PXA_SHARPSL
#define CONFIG_PXA27x 1
#define CONFIG_IWMMXT 1

/*
 * Processor Type
 */
#define CONFIG_CPU_32 1
#define CONFIG_CPU_XSCALE 1
#define CONFIG_CPU_32v5 1
#define CONFIG_CPU_ABRT_EV5T 1
#define CONFIG_CPU_CACHE_VIVT 1
#define CONFIG_CPU_TLB_V4WBI 1

/*
 * Processor Features
 */
#undef CONFIG_ARM_THUMB
#define CONFIG_XSCALE_PMU 1

/*
 * Bus support
 */
#define CONFIG_ISA_DMA_API 1

/*
 * PCCARD (PCMCIA/CardBus) support
 */
#undef CONFIG_PCCARD

/*
 * Kernel Features
 */
#undef CONFIG_PREEMPT
#undef CONFIG_NO_IDLE_HZ
#undef CONFIG_ARCH_DISCONTIGMEM_ENABLE
#define CONFIG_SELECT_MEMORY_MODEL 1
#define CONFIG_FLATMEM_MANUAL 1
#undef CONFIG_DISCONTIGMEM_MANUAL
#undef CONFIG_SPARSEMEM_MANUAL
#define CONFIG_FLATMEM 1
#define CONFIG_FLAT_NODE_MEM_MAP 1
#undef CONFIG_SPARSEMEM_STATIC
#define CONFIG_LEDS 1
#define CONFIG_LEDS_TIMER 1
#define CONFIG_LEDS_CPU 1
#define CONFIG_ALIGNMENT_TRAP 1

/*
 * Boot options
 */
#define CONFIG_ZBOOT_ROM_TEXT 0x0
#define CONFIG_ZBOOT_ROM_BSS 0x0
#define CONFIG_CMDLINE "root=/dev/nfs ip=bootp console=ttyS0,115200 mem=64M"
#undef CONFIG_XIP_KERNEL

/*
 * Floating point emulation
 */

/*
 * At least one emulation must be selected
 */
#define CONFIG_FPE_NWFPE 1
#undef CONFIG_FPE_NWFPE_XP
#undef CONFIG_FPE_FASTFPE

/*
 * Userspace binary formats
 */
#define CONFIG_BINFMT_ELF 1
#undef CONFIG_BINFMT_AOUT
#undef CONFIG_BINFMT_MISC
#undef CONFIG_ARTHUR

/*
 * Power management options
 */
#undef CONFIG_PM

/*
 * Networking
 */
#define CONFIG_NET 1

/*
 * Networking options
 */
#undef CONFIG_PACKET
#define CONFIG_UNIX 1
#undef CONFIG_NET_KEY
#define CONFIG_INET 1
#undef CONFIG_IP_MULTICAST
#undef CONFIG_IP_ADVANCED_ROUTER
#define CONFIG_IP_FIB_HASH 1
#define CONFIG_IP_PNP 1
#undef CONFIG_IP_PNP_DHCP
#define CONFIG_IP_PNP_BOOTP 1
#undef CONFIG_IP_PNP_RARP
#undef CONFIG_NET_IPIP
#undef CONFIG_NET_IPGRE
#undef CONFIG_ARPD
#undef CONFIG_SYN_COOKIES
#undef CONFIG_INET_AH
#undef CONFIG_INET_ESP
#undef CONFIG_INET_IPCOMP
#undef CONFIG_INET_TUNNEL
#define CONFIG_INET_DIAG 1
#define CONFIG_INET_TCP_DIAG 1
#undef CONFIG_TCP_CONG_ADVANCED
#define CONFIG_TCP_CONG_BIC 1
#undef CONFIG_IPV6
#undef CONFIG_NETFILTER

/*
 * DCCP Configuration (EXPERIMENTAL)
 */
#undef CONFIG_IP_DCCP

/*
 * SCTP Configuration (EXPERIMENTAL)
 */
#undef CONFIG_IP_SCTP
#undef CONFIG_ATM
#undef CONFIG_BRIDGE
#undef CONFIG_VLAN_8021Q
#undef CONFIG_DECNET
#undef CONFIG_LLC2
#undef CONFIG_IPX
#undef CONFIG_ATALK
#undef CONFIG_X25
#undef CONFIG_LAPB
#undef CONFIG_NET_DIVERT
#undef CONFIG_ECONET
#undef CONFIG_WAN_ROUTER
#undef CONFIG_NET_SCHED
#undef CONFIG_NET_CLS_ROUTE

/*
 * Network testing
 */
#undef CONFIG_NET_PKTGEN
#undef CONFIG_HAMRADIO
#undef CONFIG_IRDA
#undef CONFIG_BT
#undef CONFIG_IEEE80211

/*
 * Device Drivers
 */

/*
 * Generic Driver Options
 */
#define CONFIG_STANDALONE 1
#define CONFIG_PREVENT_FIRMWARE_BUILD 1
#undef CONFIG_FW_LOADER
#undef CONFIG_DEBUG_DRIVER

/*
 * Memory Technology Devices (MTD)
 */
#define CONFIG_MTD 1
#undef CONFIG_MTD_DEBUG
#undef CONFIG_MTD_CONCAT
#define CONFIG_MTD_PARTITIONS 1
#define CONFIG_MTD_REDBOOT_PARTS 1
#define CONFIG_MTD_REDBOOT_DIRECTORY_BLOCK -1
#undef CONFIG_MTD_REDBOOT_PARTS_UNALLOCATED
#undef CONFIG_MTD_REDBOOT_PARTS_READONLY
#undef CONFIG_MTD_CMDLINE_PARTS
#undef CONFIG_MTD_AFS_PARTS

/*
 * User Modules And Translation Layers
 */
#define CONFIG_MTD_CHAR 1
#define CONFIG_MTD_BLOCK 1
#undef CONFIG_FTL
#undef CONFIG_NFTL
#undef CONFIG_INFTL

/*
 * RAM/ROM/Flash chip drivers
 */
#define CONFIG_MTD_CFI 1
#undef CONFIG_MTD_JEDECPROBE
#define CONFIG_MTD_GEN_PROBE 1
#define CONFIG_MTD_CFI_ADV_OPTIONS 1
#define CONFIG_MTD_CFI_NOSWAP 1
#undef CONFIG_MTD_CFI_BE_BYTE_SWAP
#undef CONFIG_MTD_CFI_LE_BYTE_SWAP
#define CONFIG_MTD_CFI_GEOMETRY 1
#define CONFIG_MTD_MAP_BANK_WIDTH_1 1
#define CONFIG_MTD_MAP_BANK_WIDTH_2 1
#define CONFIG_MTD_MAP_BANK_WIDTH_4 1
#undef CONFIG_MTD_MAP_BANK_WIDTH_8
#undef CONFIG_MTD_MAP_BANK_WIDTH_16
#undef CONFIG_MTD_MAP_BANK_WIDTH_32
#undef CONFIG_MTD_CFI_I1
#define CONFIG_MTD_CFI_I2 1
#undef CONFIG_MTD_CFI_I4
#undef CONFIG_MTD_CFI_I8
#undef CONFIG_MTD_OTP
#define CONFIG_MTD_CFI_INTELEXT 1
#undef CONFIG_MTD_CFI_AMDSTD
#undef CONFIG_MTD_CFI_STAA
#define CONFIG_MTD_CFI_UTIL 1
#undef CONFIG_MTD_RAM
#undef CONFIG_MTD_ROM
#undef CONFIG_MTD_ABSENT
#undef CONFIG_MTD_XIP

/*
 * Mapping drivers for chip access
 */
#undef CONFIG_MTD_COMPLEX_MAPPINGS
#undef CONFIG_MTD_PHYSMAP
#undef CONFIG_MTD_MAINSTONE
#undef CONFIG_MTD_ARM_INTEGRATOR
#undef CONFIG_MTD_EDB7312
#undef CONFIG_MTD_SHARP_SL
#undef CONFIG_MTD_PLATRAM

/*
 * Self-contained MTD device drivers
 */
#undef CONFIG_MTD_SLRAM
#undef CONFIG_MTD_PHRAM
#undef CONFIG_MTD_MTDRAM
#undef CONFIG_MTD_BLKMTD
#undef CONFIG_MTD_BLOCK2MTD

/*
 * Disk-On-Chip Device Drivers
 */
#undef CONFIG_MTD_DOC2000
#undef CONFIG_MTD_DOC2001
#undef CONFIG_MTD_DOC2001PLUS

/*
 * NAND Flash Device Drivers
 */
#undef CONFIG_MTD_NAND

/*
 * Parallel port support
 */
#undef CONFIG_PARPORT

/*
 * Plug and Play support
 */

/*
 * Block devices
 */
#undef CONFIG_BLK_DEV_COW_COMMON
#undef CONFIG_BLK_DEV_LOOP
#undef CONFIG_BLK_DEV_NBD
#undef CONFIG_BLK_DEV_RAM
#define CONFIG_BLK_DEV_RAM_COUNT 16
#undef CONFIG_CDROM_PKTCDVD

/*
 * IO Schedulers
 */
#define CONFIG_IOSCHED_NOOP 1
#define CONFIG_IOSCHED_AS 1
#define CONFIG_IOSCHED_DEADLINE 1
#define CONFIG_IOSCHED_CFQ 1
#undef CONFIG_ATA_OVER_ETH

/*
 * ATA/ATAPI/MFM/RLL support
 */
#define CONFIG_IDE 1
#define CONFIG_BLK_DEV_IDE 1

/*
 * Please see Documentation/ide.txt for help/info on IDE drives
 */
#undef CONFIG_BLK_DEV_IDE_SATA
#define CONFIG_BLK_DEV_IDEDISK 1
#undef CONFIG_IDEDISK_MULTI_MODE
#undef CONFIG_BLK_DEV_IDECD
#undef CONFIG_BLK_DEV_IDETAPE
#undef CONFIG_BLK_DEV_IDEFLOPPY
#undef CONFIG_IDE_TASK_IOCTL

/*
 * IDE chipset support/bugfixes
 */
#undef CONFIG_IDE_GENERIC
#undef CONFIG_IDE_ARM
#undef CONFIG_BLK_DEV_IDEDMA
#undef CONFIG_IDEDMA_AUTO
#undef CONFIG_BLK_DEV_HD

/*
 * SCSI device support
 */
#undef CONFIG_RAID_ATTRS
#undef CONFIG_SCSI

/*
 * Multi-device support (RAID and LVM)
 */
#undef CONFIG_MD

/*
 * Fusion MPT device support
 */
#undef CONFIG_FUSION

/*
 * IEEE 1394 (FireWire) support
 */

/*
 * I2O device support
 */

/*
 * Network device support
 */
#define CONFIG_NETDEVICES 1
#undef CONFIG_DUMMY
#undef CONFIG_BONDING
#undef CONFIG_EQUALIZER
#undef CONFIG_TUN

/*
 * PHY device support
 */
#undef CONFIG_PHYLIB

/*
 * Ethernet (10 or 100Mbit)
 */
#define CONFIG_NET_ETHERNET 1
#define CONFIG_MII 1
#define CONFIG_SMC91X 1
#undef CONFIG_DM9000

/*
 * Ethernet (1000 Mbit)
 */

/*
 * Ethernet (10000 Mbit)
 */

/*
 * Token Ring devices
 */

/*
 * Wireless LAN (non-hamradio)
 */
#undef CONFIG_NET_RADIO

/*
 * Wan interfaces
 */
#undef CONFIG_WAN
#undef CONFIG_PPP
#undef CONFIG_SLIP
#undef CONFIG_SHAPER
#undef CONFIG_NETCONSOLE
#undef CONFIG_NETPOLL
#undef CONFIG_NET_POLL_CONTROLLER

/*
 * ISDN subsystem
 */
#undef CONFIG_ISDN

/*
 * Input device support
 */
#define CONFIG_INPUT 1

/*
 * Userland interfaces
 */
#define CONFIG_INPUT_MOUSEDEV 1
#define CONFIG_INPUT_MOUSEDEV_PSAUX 1
#define CONFIG_INPUT_MOUSEDEV_SCREEN_X 1024
#define CONFIG_INPUT_MOUSEDEV_SCREEN_Y 768
#undef CONFIG_INPUT_JOYDEV
#undef CONFIG_INPUT_TSDEV
#define CONFIG_INPUT_EVDEV 1
#undef CONFIG_INPUT_EVBUG

/*
 * Input Device Drivers
 */
#define CONFIG_INPUT_KEYBOARD 1
#define CONFIG_KEYBOARD_ATKBD 1
#undef CONFIG_KEYBOARD_SUNKBD
#undef CONFIG_KEYBOARD_LKKBD
#undef CONFIG_KEYBOARD_XTKBD
#undef CONFIG_KEYBOARD_NEWTON
#undef CONFIG_INPUT_MOUSE
#undef CONFIG_INPUT_JOYSTICK
#undef CONFIG_INPUT_TOUCHSCREEN
#undef CONFIG_INPUT_MISC

/*
 * Hardware I/O ports
 */
#define CONFIG_SERIO 1
#undef CONFIG_SERIO_SERPORT
#define CONFIG_SERIO_LIBPS2 1
#undef CONFIG_SERIO_RAW
#undef CONFIG_GAMEPORT

/*
 * Character devices
 */
#define CONFIG_VT 1
#define CONFIG_VT_CONSOLE 1
#define CONFIG_HW_CONSOLE 1
#undef CONFIG_SERIAL_NONSTANDARD

/*
 * Serial drivers
 */
#undef CONFIG_SERIAL_8250

/*
 * Non-8250 serial port support
 */
#define CONFIG_SERIAL_PXA 1
#define CONFIG_SERIAL_PXA_CONSOLE 1
#define CONFIG_SERIAL_CORE 1
#define CONFIG_SERIAL_CORE_CONSOLE 1
#define CONFIG_UNIX98_PTYS 1
#define CONFIG_LEGACY_PTYS 1
#define CONFIG_LEGACY_PTY_COUNT 256

/*
 * IPMI
 */
#undef CONFIG_IPMI_HANDLER

/*
 * Watchdog Cards
 */
#undef CONFIG_WATCHDOG
#undef CONFIG_NVRAM
#undef CONFIG_RTC
#undef CONFIG_DTLK
#undef CONFIG_R3964

/*
 * Ftape, the floppy tape device driver
 */
#undef CONFIG_RAW_DRIVER

/*
 * TPM devices
 */

/*
 * I2C support
 */
#undef CONFIG_I2C

/*
 * Hardware Monitoring support
 */
#define CONFIG_HWMON 1
#undef CONFIG_HWMON_VID
#undef CONFIG_HWMON_DEBUG_CHIP

/*
 * Misc devices
 */

/*
 * Multimedia Capabilities Port drivers
 */

/*
 * Multimedia devices
 */
#undef CONFIG_VIDEO_DEV

/*
 * Digital Video Broadcasting Devices
 */
#undef CONFIG_DVB

/*
 * Graphics support
 */
#define CONFIG_FB 1
#define CONFIG_FB_CFB_FILLRECT 1
#define CONFIG_FB_CFB_COPYAREA 1
#define CONFIG_FB_CFB_IMAGEBLIT 1
#define CONFIG_FB_SOFT_CURSOR 1
#undef CONFIG_FB_MACMODES
#undef CONFIG_FB_MODE_HELPERS
#undef CONFIG_FB_TILEBLITTING
#define CONFIG_FB_PXA 1
#undef CONFIG_FB_PXA_PARAMETERS
#undef CONFIG_FB_S1D13XXX
#undef CONFIG_FB_VIRTUAL

/*
 * Console display driver support
 */
#undef CONFIG_VGA_CONSOLE
#define CONFIG_DUMMY_CONSOLE 1
#define CONFIG_FRAMEBUFFER_CONSOLE 1
#undef CONFIG_FONTS
#define CONFIG_FONT_8x8 1
#define CONFIG_FONT_8x16 1

/*
 * Logo configuration
 */
#define CONFIG_LOGO 1
#define CONFIG_LOGO_LINUX_MONO 1
#define CONFIG_LOGO_LINUX_VGA16 1
#define CONFIG_LOGO_LINUX_CLUT224 1
#undef CONFIG_BACKLIGHT_LCD_SUPPORT

/*
 * Sound
 */
#undef CONFIG_SOUND

/*
 * USB support
 */
#define CONFIG_USB_ARCH_HAS_HCD 1
#define CONFIG_USB_ARCH_HAS_OHCI 1
#undef CONFIG_USB

/*
 * USB Gadget Support
 */
#undef CONFIG_USB_GADGET

/*
 * MMC/SD Card support
 */
#undef CONFIG_MMC

/*
 * File systems
 */
#define CONFIG_EXT2_FS 1
#undef CONFIG_EXT2_FS_XATTR
#undef CONFIG_EXT2_FS_XIP
#undef CONFIG_EXT3_FS
#undef CONFIG_JBD
#undef CONFIG_REISERFS_FS
#undef CONFIG_JFS_FS
#undef CONFIG_FS_POSIX_ACL
#undef CONFIG_XFS_FS
#undef CONFIG_MINIX_FS
#undef CONFIG_ROMFS_FS
#define CONFIG_INOTIFY 1
#undef CONFIG_QUOTA
#define CONFIG_DNOTIFY 1
#undef CONFIG_AUTOFS_FS
#undef CONFIG_AUTOFS4_FS
#undef CONFIG_FUSE_FS

/*
 * CD-ROM/DVD Filesystems
 */
#undef CONFIG_ISO9660_FS
#undef CONFIG_UDF_FS

/*
 * DOS/FAT/NT Filesystems
 */
#define CONFIG_FAT_FS 1
#define CONFIG_MSDOS_FS 1
#undef CONFIG_VFAT_FS
#define CONFIG_FAT_DEFAULT_CODEPAGE 437
#undef CONFIG_NTFS_FS

/*
 * Pseudo filesystems
 */
#define CONFIG_PROC_FS 1
#define CONFIG_SYSFS 1
#undef CONFIG_TMPFS
#undef CONFIG_HUGETLB_PAGE
#define CONFIG_RAMFS 1
#undef CONFIG_RELAYFS_FS

/*
 * Miscellaneous filesystems
 */
#undef CONFIG_ADFS_FS
#undef CONFIG_AFFS_FS
#undef CONFIG_HFS_FS
#undef CONFIG_HFSPLUS_FS
#undef CONFIG_BEFS_FS
#undef CONFIG_BFS_FS
#undef CONFIG_EFS_FS
#undef CONFIG_JFFS_FS
#define CONFIG_JFFS2_FS 1
#define CONFIG_JFFS2_FS_DEBUG 0
#define CONFIG_JFFS2_FS_WRITEBUFFER 1
#undef CONFIG_JFFS2_COMPRESSION_OPTIONS
#define CONFIG_JFFS2_ZLIB 1
#define CONFIG_JFFS2_RTIME 1
#undef CONFIG_JFFS2_RUBIN
#undef CONFIG_CRAMFS
#undef CONFIG_VXFS_FS
#undef CONFIG_HPFS_FS
#undef CONFIG_QNX4FS_FS
#undef CONFIG_SYSV_FS
#undef CONFIG_UFS_FS

/*
 * Network File Systems
 */
#define CONFIG_NFS_FS 1
#undef CONFIG_NFS_V3
#undef CONFIG_NFS_V4
#undef CONFIG_NFS_DIRECTIO
#undef CONFIG_NFSD
#define CONFIG_ROOT_NFS 1
#define CONFIG_LOCKD 1
#define CONFIG_NFS_COMMON 1
#define CONFIG_SUNRPC 1
#undef CONFIG_RPCSEC_GSS_KRB5
#undef CONFIG_RPCSEC_GSS_SPKM3
#undef CONFIG_SMB_FS
#undef CONFIG_CIFS
#undef CONFIG_NCP_FS
#undef CONFIG_CODA_FS
#undef CONFIG_AFS_FS
#undef CONFIG_9P_FS

/*
 * Partition Types
 */
#undef CONFIG_PARTITION_ADVANCED
#define CONFIG_MSDOS_PARTITION 1

/*
 * Native Language Support
 */
#define CONFIG_NLS 1
#define CONFIG_NLS_DEFAULT "iso8859-1"
#undef CONFIG_NLS_CODEPAGE_437
#undef CONFIG_NLS_CODEPAGE_737
#undef CONFIG_NLS_CODEPAGE_775
#undef CONFIG_NLS_CODEPAGE_850
#undef CONFIG_NLS_CODEPAGE_852
#undef CONFIG_NLS_CODEPAGE_855
#undef CONFIG_NLS_CODEPAGE_857
#undef CONFIG_NLS_CODEPAGE_860
#undef CONFIG_NLS_CODEPAGE_861
#undef CONFIG_NLS_CODEPAGE_862
#undef CONFIG_NLS_CODEPAGE_863
#undef CONFIG_NLS_CODEPAGE_864
#undef CONFIG_NLS_CODEPAGE_865
#undef CONFIG_NLS_CODEPAGE_866
#undef CONFIG_NLS_CODEPAGE_869
#undef CONFIG_NLS_CODEPAGE_936
#undef CONFIG_NLS_CODEPAGE_950
#undef CONFIG_NLS_CODEPAGE_932
#undef CONFIG_NLS_CODEPAGE_949
#undef CONFIG_NLS_CODEPAGE_874
#undef CONFIG_NLS_ISO8859_8
#undef CONFIG_NLS_CODEPAGE_1250
#undef CONFIG_NLS_CODEPAGE_1251
#undef CONFIG_NLS_ASCII
#define CONFIG_NLS_ISO8859_1 1
#undef CONFIG_NLS_ISO8859_2
#undef CONFIG_NLS_ISO8859_3
#undef CONFIG_NLS_ISO8859_4
#undef CONFIG_NLS_ISO8859_5
#undef CONFIG_NLS_ISO8859_6
#undef CONFIG_NLS_ISO8859_7
#undef CONFIG_NLS_ISO8859_9
#undef CONFIG_NLS_ISO8859_13
#undef CONFIG_NLS_ISO8859_14
#undef CONFIG_NLS_ISO8859_15
#undef CONFIG_NLS_KOI8_R
#undef CONFIG_NLS_KOI8_U
#undef CONFIG_NLS_UTF8

/*
 * Profiling support
 */
#undef CONFIG_PROFILING

/*
 * Kernel hacking
 */
#undef CONFIG_PRINTK_TIME
#define CONFIG_DEBUG_KERNEL 1
#define CONFIG_MAGIC_SYSRQ 1
#define CONFIG_LOG_BUF_SHIFT 14
#define CONFIG_DETECT_SOFTLOCKUP 1
#undef CONFIG_SCHEDSTATS
#undef CONFIG_DEBUG_SLAB
#undef CONFIG_DEBUG_SPINLOCK
#undef CONFIG_DEBUG_SPINLOCK_SLEEP
#undef CONFIG_DEBUG_KOBJECT
#define CONFIG_DEBUG_BUGVERBOSE 1
#define CONFIG_DEBUG_INFO 1
#undef CONFIG_DEBUG_FS
#define CONFIG_FRAME_POINTER 1
#define CONFIG_DEBUG_USER 1
#undef CONFIG_DEBUG_WAITQ
#define CONFIG_DEBUG_ERRORS 1
#define CONFIG_DEBUG_LL 1
#undef CONFIG_DEBUG_ICEDCC

/*
 * Security options
 */
#undef CONFIG_KEYS
#undef CONFIG_SECURITY

/*
 * Cryptographic options
 */
#undef CONFIG_CRYPTO

/*
 * Hardware crypto devices
 */

/*
 * Library routines
 */
#undef CONFIG_CRC_CCITT
#undef CONFIG_CRC16
#define CONFIG_CRC32 1
#undef CONFIG_LIBCRC32C
#define CONFIG_ZLIB_INFLATE 1
#define CONFIG_ZLIB_DEFLATE 1
