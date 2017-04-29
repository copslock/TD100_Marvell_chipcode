# plugindir is set in configure

##############################################################################
# change libgstwmvdec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstwmvdec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstwmvdec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstwmvdec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstwmvdec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstwmvdec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstwmvdec_la_SOURCES = gstwmvdec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstwmvdec_la_CFLAGS = $(GST_CFLAGS)
libgstwmvdec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecwmvdec
libgstwmvdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstwmvdec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstwmvdec.h
