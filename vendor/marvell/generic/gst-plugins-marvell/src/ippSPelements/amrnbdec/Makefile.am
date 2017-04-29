# plugindir is set in configure

##############################################################################
# change libgstamrnbdec.la to something more suitable, e.g. libmysomething.la  #
##############################################################################
plugin_LTLIBRARIES = libgstamrnbdec.la

##############################################################################
# for the next set of variables, rename the prefix if you renamed the .la,   #
#  e.g. libgstamrnbdec_la_SOURCES => libmysomething_la_SOURCES                 #
#       libgstamrnbdec_la_CFLAGS  => libmysomething_la_CFLAGS                  #
#       libgstamrnbdec_la_LIBADD  => libmysomething_la_LIBADD                  #
#       libgstamrnbdec_la_LDFLAGS => libmysomething_la_LDFLAGS                 #
##############################################################################

# sources used to compile this plug-in
libgstamrnbdec_la_SOURCES = gstamrnbdec.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstamrnbdec_la_CFLAGS = $(GST_CFLAGS)
libgstamrnbdec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(GSTCTRL_LIBS) -lcodecamrnbdec
libgstamrnbdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstamrnbdec_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstamrnbdec.h
