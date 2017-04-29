# plugindir is set in configure

##############################################################################
# change libgsth263enc.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgsth263enc.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgsth263enc_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgsth263enc_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgsth263enc_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgsth263enc_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgsth263enc_la_SOURCES = gsth263enc.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgsth263enc_la_CFLAGS = $(GST_CFLAGS)
libgsth263enc_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodech263enc
libgsth263enc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgsth263enc_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gsth263enc.h
