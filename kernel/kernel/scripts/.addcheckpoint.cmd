cmd_scripts/addcheckpoint := gcc -Wp,-MD,scripts/.addcheckpoint.d -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer     -o scripts/addcheckpoint scripts/addcheckpoint.c  

deps_scripts/addcheckpoint := \
  scripts/addcheckpoint.c \
    $(wildcard include/config/yaffs/util.h) \
    $(wildcard include/config/yaffs/yaffs2.h) \
  /usr/include/stdlib.h \
  /usr/include/features.h \
  /usr/include/bits/predefs.h \
  /usr/include/sys/cdefs.h \
  /usr/include/bits/wordsize.h \
  /usr/include/gnu/stubs.h \
  /usr/include/gnu/stubs-64.h \
  /usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stddef.h \
  /usr/include/bits/waitflags.h \
  /usr/include/bits/waitstatus.h \
  /usr/include/endian.h \
  /usr/include/bits/endian.h \
  /usr/include/bits/byteswap.h \
  /usr/include/sys/types.h \
  /usr/include/bits/types.h \
  /usr/include/bits/typesizes.h \
  /usr/include/time.h \
  /usr/include/sys/select.h \
  /usr/include/bits/select.h \
  /usr/include/bits/sigset.h \
  /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h \
  /usr/include/alloca.h \
  /usr/include/bits/stdlib.h \
  /usr/include/stdio.h \
  /usr/include/libio.h \
  /usr/include/_G_config.h \
  /usr/include/wchar.h \
  /usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h \
  /usr/include/bits/sys_errlist.h \
  /usr/include/bits/stdio.h \
  /usr/include/bits/stdio2.h \
  /usr/include/fcntl.h \
  /usr/include/bits/fcntl.h \
  /usr/include/bits/stat.h \
  /usr/include/bits/fcntl2.h \
  /usr/include/sys/stat.h \
  /usr/include/dirent.h \
  /usr/include/bits/dirent.h \
  /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h \
  /usr/include/linux/limits.h \
  /usr/include/string.h \
  /usr/include/xlocale.h \
  /usr/include/bits/string.h \
  /usr/include/bits/string2.h \
  /usr/include/bits/string3.h \
  /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h \
  /usr/include/bits/environments.h \
  /usr/include/bits/confname.h \
  /usr/include/getopt.h \
  /usr/include/bits/unistd.h \
  /usr/include/assert.h \
  scripts/../fs/yaffs2/yaffs_ecc.h \
  scripts/../fs/yaffs2/yaffs_guts.h \
    $(wildcard include/config/yaffs/no/yaffs1.h) \
    $(wildcard include/config/yaffs/unicode.h) \
    $(wildcard include/config/yaffs/wince.h) \
    $(wildcard include/config/yaffs/short/names/in/ram.h) \
    $(wildcard include/config/yaffs/auto/unicode.h) \
  scripts/../fs/yaffs2/yportenv.h \
    $(wildcard include/config/yaffs/direct.h) \
    $(wildcard include/config/yaffsfs/provide/values.h) \
  scripts/../fs/yaffs2/devextras.h \
    $(wildcard include/config/yaffs/provide/defs.h) \
  scripts/../fs/yaffs2/yaffs_list.h \
  scripts/../fs/yaffs2/yaffs_tagsvalidity.h \
  scripts/../fs/yaffs2/yaffs_guts.h \
  scripts/../fs/yaffs2/yaffs_packedtags2.h \
  scripts/../fs/yaffs2/yaffs_ecc.h \

scripts/addcheckpoint: $(deps_scripts/addcheckpoint)

$(deps_scripts/addcheckpoint):
