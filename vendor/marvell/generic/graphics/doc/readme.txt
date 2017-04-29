    GC500 Unified Graphics Driver for Android  
    Ver0.8.0.4375-TD-Gingerbread-Beta6

1.  Introduction
    This package contains GC500 unified graphics driver for android and some documents. 
    This document is used to describe the package contents, how to build kernel mode driver and how to install the driver binaries to device.
 
2.  Updates
    -- Add rgba8888 support for eglCopyBuffers.            [Internal bug partial fix CQ00190078 SVN4309, Edmurn]
    -- Add non-aligned size yuv buffer support in 2d path. [Internal bug fix CQ00180543 SVN4222, Carrot]

3.  Package Contents
    gc500/                -- Package root directory
       doc                -- Documentation
       galcroe_src        -- GC source code needed to build kernel mode driver
       hgl                -- Dir placed user mode static link libraries to build
       prebuilt           -- Prebuilt driver binaries, just for reference, you need to build them by yourself.
       samples            -- Some sample codes

4.  Build and Installation
4.1 System Requirement 
    Host PC :
       Operating system   -- Ubuntu 8.04 or newer
       Toolchain          -- arm-marvell-linux-gnueabi
    Target Device:
        Platform          -- PXA920/PXA910 processor DKB platform
        Operation system  -- Linux kernel 2.6.29 or newer
   
4.2 Build Kernel Mode Driver
    Enter ~/galcore_src, modify the "Makefile" according to your system:
       1)  Modify "CROSS_COMPILE" to your toolchain directory with prefix
           (path_to_your_marvelltoolchain_dir/bin/arm-marvell-linux-gnueabi-);
       2)  Modify "KERNEL_DIR" to point to your linux kernel tree.
    Build kernel mode driver by commands:
       #make clean
       #make
    If no error, the build result galcore.ko will be placed on current directory.
	
4.3 Build User Mode Driver
    1) Copy galcore.ko built by you to ~/galcore
    2) #cd android_home
       #. ./build/envsetup.sh
       #choosecombo
       #mmm ~/hgl/
    3) Output:
       android_output_dir/system/lib/modules/galcore.ko
       android_output_dir/system/lib/libGAL.so
       android_output_dir/system/lib/libgcu.so
       android_output_dir/system/lib/egl/egl.cfg
       android_output_dir/system/lib/egl/libEGL.so
       android_output_dir/system/lib/egl/libGLESv1_CM_MRVL.so
       android_output_dir/system/lib/egl/libGLESv2_MRVL.so

4.4 Installation
    For PXA920/PXA910(TD/TTC)
       mknod  /dev/galcore c 199 0
       insmod /system/lib/modules/galcore.ko registerMemBase=0xc0400000 irqLine=8 gpu_frequency=312 contiguousSize=0x2000000 pmemSize=0
    Reboot the device and run sample applications on device.
    Note:
        gpu_frequency may can't be set when insmod as kernel may hard coding as following:
	For pxa918, fix gc rate to 156MHZ (the same as 312<=gpu_frequency<403)
	For pxa921, fix gc rate to 250MHZ (the same as gpu_frequency>=501).
	For pxa920, gc clk can be selected freely (250MHZ if gpu_frequency>=501, 201.5M if 403<=gpu_frequency<501, 156M if 312<=gpu_frequency<403, 78M if gpu_frequency<312).
5.  FAQ

6.  Known Issues

