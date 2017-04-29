# plugindir is set in configure

##############################################################################
# change libgstamrwbenc.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstamrwbenc.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstamrwbenc_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstamrwbenc_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstamrwbenc_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstamrwbenc_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstamrwbenc_la_SOURCES = gstamrwbenc.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstamrwbenc_la_CFLAGS = $(GST_CFLAGS)
libgstamrwbenc_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecamrwbenc
libgstamrwbenc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstamrwbenc_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstamrwbenc.h
