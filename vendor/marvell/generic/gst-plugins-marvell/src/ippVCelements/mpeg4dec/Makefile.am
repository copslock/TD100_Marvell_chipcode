# plugindir is set in configure

##############################################################################
# change libgstmpeg4dec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstmpeg4dec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstmpeg4dec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstmpeg4dec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstmpeg4dec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstmpeg4dec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstmpeg4dec_la_SOURCES = gstmpeg4dec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstmpeg4dec_la_CFLAGS = $(GST_CFLAGS)
libgstmpeg4dec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecmpeg4dec
libgstmpeg4dec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstmpeg4dec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstmpeg4dec.h
