# $Id$

MAINTAINERCLEANFILES = \
	Makefile.in

EXTRA_DIST = \
	tool.nsh

INCLUDES = \
	-I$(top_builddir) \
	-I$(top_srcdir)/src


bin_PROGRAMS = \
	synfig

synfig_SOURCES = \
	definitions.h \
	progress.h \
	renderprogress.h \
	job.h \
	main.cpp

synfig_LDADD = \
	../synfig/libsynfig.la \
	@SYNFIG_LIBS@ \
	@OPENEXR_HALF_LIBS@

synfig_CXXFLAGS = \
	@SYNFIG_CFLAGS@
