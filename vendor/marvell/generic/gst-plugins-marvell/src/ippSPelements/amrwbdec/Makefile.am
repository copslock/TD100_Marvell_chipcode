# plugindir is set in configure

##############################################################################
# change libgstamrwbdec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstamrwbdec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstamrwbdec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstamrwbdec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstamrwbdec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstamrwbdec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstamrwbdec_la_SOURCES = gstamrwbdec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstamrwbdec_la_CFLAGS = $(GST_CFLAGS)
libgstamrwbdec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecamrwbdec
libgstamrwbdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstamrwbdec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstamrwbdec.h
