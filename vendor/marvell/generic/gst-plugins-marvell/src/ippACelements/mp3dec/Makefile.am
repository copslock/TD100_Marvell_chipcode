# plugindir is set in configure

##############################################################################
# change libgstmp3dec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstmp3dec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstmp3dec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstmp3dec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstmp3dec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstmp3dec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstmp3dec_la_SOURCES = gstmp3dec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstmp3dec_la_CFLAGS = $(GST_CFLAGS)
libgstmp3dec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecmp3dec
libgstmp3dec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstmp3dec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstmp3dec.h
