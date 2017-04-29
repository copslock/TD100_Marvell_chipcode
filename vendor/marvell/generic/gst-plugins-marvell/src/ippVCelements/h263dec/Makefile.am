# plugindir is set in configure

##############################################################################
# change libgsth263dec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgsth263dec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgsth263dec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgsth263dec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgsth263dec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgsth263dec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgsth263dec_la_SOURCES = gsth263dec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgsth263dec_la_CFLAGS = $(GST_CFLAGS)
libgsth263dec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodech263dec
libgsth263dec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgsth263dec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gsth263dec.h
