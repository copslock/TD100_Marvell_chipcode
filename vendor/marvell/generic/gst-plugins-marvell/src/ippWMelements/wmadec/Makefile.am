# plugindir is set in configure

##############################################################################
# change libgstwmadec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstwmadec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstwmadec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstwmadec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstwmadec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstwmadec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstwmadec_la_SOURCES = gstwmadec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstwmadec_la_CFLAGS = $(GST_CFLAGS)
libgstwmadec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecwmadec
libgstwmadec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstwmadec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstwmadec.h
