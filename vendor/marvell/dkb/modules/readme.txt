This Mdroid.mk enables to build kernel modules in 2 ways:

Use prebuilt kernel modules:
===========================

1. Create a folder "modules" in the top folder of android source code.

2. Copy those modules there.

3. ANDROID_PREBUILT_MODULES=module make

The prebuilt modules should come with kernel zImage. If you want to run android on NFS, you should download modules_android_nfs.tgz and use files in that package as the prebuilt modules.

Ignore building kernel modules:
==============================

You can ask the build system to ignore building those kernel modules. Kernel modules are required to make android run successfully on the board. But if you don't build the whole package it is acceptable to ignore them.

1. ANDROID_PREBUILT_MODULES=no_kernel_modules make




