# plugindir is set in configure

##############################################################################
# change libgstamrnbenc.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstamrnbenc.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstamrnbenc_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstamrnbenc_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstamrnbenc_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstamrnbenc_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstamrnbenc_la_SOURCES = gstamrnbenc.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstamrnbenc_la_CFLAGS = $(GST_CFLAGS)
libgstamrnbenc_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecamrnbenc
libgstamrnbenc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstamrnbenc_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstamrnbenc.h
