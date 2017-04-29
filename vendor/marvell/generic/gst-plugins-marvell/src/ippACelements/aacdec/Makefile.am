# plugindir is set in configure

##############################################################################
# change libgstaacdec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstaacdec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstaacdec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstaacdec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstaacdec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstaacdec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstaacdec_la_SOURCES = gstaacdec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstaacdec_la_CFLAGS = $(GST_CFLAGS)
libgstaacdec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecaacdec
libgstaacdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstaacdec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstaacdec.h
