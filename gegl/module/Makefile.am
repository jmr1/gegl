AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)/gegl \
	-I$(top_srcdir)/gegl \
	-I$(top_builddir)/gegl/buffer \
	-I$(top_srcdir)/gegl/buffer \
	-I$(top_builddir)/gegl/graph \
	-I$(top_srcdir)/gegl/graph \
	-I$(top_builddir)/gegl/operation \
	-I$(top_srcdir)/gegl/operation \
	-I$(top_builddir)/gegl/opencl \
	-I$(top_srcdir)/gegl/opencl \
	-I$(top_builddir)/gegl/process \
	-I$(top_srcdir)/gegl/process \
	-I$(top_builddir)/gegl/property-types \
	-I$(top_srcdir)/gegl/property-types	\
	-DG_LOG_DOMAIN=\"GEGL\"

AM_CFLAGS = $(DEP_CFLAGS) $(BABL_CFLAGS)

noinst_LTLIBRARIES = libmodule.la

#libmodule_publicdir = $(includedir)/gegl-$(GEGL_API_VERSION)/gegl/module
#libmodule_public_HEADERS = #$(MODULE_headers)

libmodule_la_SOURCES = \
	gegldatafiles.c	\
	geglmodule.c	\
	geglmoduledb.c	\
	gegldatafiles.h	\
	geglmoduledb.h	\
	geglmodule.h

#libmodule_la_SOURCES = $(libmodule_sources) $(libmodule_public_HEADERS)
