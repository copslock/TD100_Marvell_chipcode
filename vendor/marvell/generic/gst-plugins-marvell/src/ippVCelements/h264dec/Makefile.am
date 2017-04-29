# plugindir is set in configure

##############################################################################
# change libgsth264dec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgsth264dec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgsth264dec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgsth264dec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgsth264dec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgsth264dec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgsth264dec_la_SOURCES = gsth264dec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgsth264dec_la_CFLAGS = $(GST_CFLAGS)
libgsth264dec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodech264dec
libgsth264dec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgsth264dec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gsth264dec.h
