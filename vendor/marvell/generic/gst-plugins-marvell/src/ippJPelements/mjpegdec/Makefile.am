# plugindir is set in configure

##############################################################################
# change libgstmjpegdec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstmjpegdec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstmjpegdec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstmjpegdec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstmjpegdec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstmjpegdec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstmjpegdec_la_SOURCES = gstmjpegdec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstmjpegdec_la_CFLAGS = $(GST_CFLAGS)
libgstmjpegdec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecjpegdec
libgstmjpegdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstmjpegdec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstmjpegdec.h
