# plugindir is set in configure

##############################################################################
# change libgsth264enc.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgsth264enc.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgsth264enc_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgsth264enc_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgsth264enc_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgsth264enc_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgsth264enc_la_SOURCES = gsth264enc.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgsth264enc_la_CFLAGS = $(GST_CFLAGS)
libgsth264enc_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodech264enc
libgsth264enc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgsth264enc_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gsth264enc.h
