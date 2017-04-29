# plugindir is set in configure

##############################################################################
# change libgstippmpeg2dec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstippmpeg2dec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstippmpeg2dec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstippmpeg2dec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstippmpeg2dec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstippmpeg2dec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstippmpeg2dec_la_SOURCES = gstmpeg2dec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstippmpeg2dec_la_CFLAGS = $(GST_CFLAGS)
libgstippmpeg2dec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecmpeg2dec
libgstippmpeg2dec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstippmpeg2dec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstmpeg2dec.h
