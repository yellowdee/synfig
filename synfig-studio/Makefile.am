# $Id$

MAINTAINERCLEANFILES = \
	COPYING \
	INSTALL \
	config/ltmain.sh \
	doxygen.cfg \
	config/config.guess \
	config/config.sub \
	config/ltmain.sh \
	config/install-sh \
	config/mkinstalldirs \
	config/aclocal.m4 \
	config/missing \
	config/texinfo.tex \
	config/depcomp \
	aclocal.m4 \
	config.h.in \
	configure \
	stamp-h.in \
	Makefile.in \
	config.log \
	config.status \
	.doc_stamp \
	.DS_Store

SUBDIRS = \
	build_tools \
	src \
	images \
	po

EXTRA_DIST = \
	COPYING \
	TODO \
	m4/subs.m4 \
	doxygen.cfg.in \
	doxygen.cfg \
	macosxbuild.sh \
	win32build.sh \
	win32inst.nsi.in \
	config/package \
	config/depcomp \
	m4/cxx_macros.m4 \
	m4/ETL.m4 \
	ChangeLog.old \
	synfigstudio.xml.in \
	synfigstudio-thumbnailer.schemas.in


DISTCHECK_CONFIGURE_FLAGS = --disable-update-mimedb

# Desktop entry
desktopdir = $(prefix)/share/applications
desktop_DATA = synfigstudio.desktop

# @INTLTOOL_DESKTOP_RULE@

mimeinfodir              = $(prefix)/share/mime-info
mimeinfo_DATA            = synfigstudio.keys synfigstudio.mime

# Icon
icondir                 = $(datadir)/pixmaps
icon_DATA               = images/synfig_icon.png images/sif_icon.png

mimedir              = $(prefix)/share/mime/packages
mime_DATA            = synfigstudio.xml

ACLOCAL_AMFLAGS=-I m4

GREP=grep
PRINTF=printf
SH=sh
DOXYGEN=doxygen
#SVN_REPOSITORY=@SVN_REPOSITORY@

SVN=svn
TAG=@PACKAGE_TARNAME@_@VERSION_MAJ@_@VERSION_MIN@_@VERSION_REV@

ChangeLog:
	../autobuild/git2cl > ChangeLog

stats:
	-@echo
	-@echo  -- Stats
	-@echo
	-@$(PRINTF) "Total lines: "
	-@wc -l $(shell find $(top_srcdir)/src -name '*.[ch]*' | $(GREP) -v libavcodec) | $(GREP) total
	-@$(PRINTF) "Total size: "
	-@du -hcs $(shell find $(top_srcdir)/src -name '*.[ch]*' | $(GREP) -v libavcodec) | $(GREP) total
	-@echo 

listfixmes:
	-@echo
	-@echo  -- List of pending FIXMEs
	-@echo
	-@$(GREP) FIXME -n $(shell find $(top_srcdir) -name '*.[ch]*' | grep -v svn)
	-@echo

listhacks:
	-@echo
	-@echo  -- List of pending HACKs
	-@echo
	-@$(GREP) HACK -n $(shell find $(top_srcdir) -name '*.[ch]*' | grep -v svn)
	-@echo

run: check

.doc_stamp: doxygen.cfg
	$(DOXYGEN) doxygen.cfg
	touch .doc_stamp

package-win32: all win32inst.nsi
	convert images/installer_logo.png bmp3:images/installer_logo.bmp
	make -C images sif_icon.ico synfig_icon.ico
	grep -v -e 'installer_logo' -e 'sif_icon' images/images.nsh >images/images.nsh.tmp
	grep -v -e 'installer_logo' -e 'sif_icon' images/unimages.nsh >images/unimages.nsh.tmp
	mv -f images/images.nsh.tmp images/images.nsh
	mv -f images/unimages.nsh.tmp images/unimages.nsh
	makensis win32inst.nsi

package-osx: all pkg-info/macosx/synfig-studio.info
	[ -d pkg_root ] && $(RMDIR) pkg_root || true
	convert images/installer_logo_osx.png $(srcdir)/pkg-info/macosx/studio-resources/background.tif
	make install prefix="`pwd`/pkg_root"
	$(srcdir)/config/package pkg_root pkg-info/macosx/synfig-studio.info -r $(srcdir)/pkg-info/macosx/studio-resources
if WIN32_PKG	
package: package-win32
else
if MACOSX_PKG
package: package-osx
endif
endif

install-data-hook:
if ENABLE_UPDATE_MIMEDB
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(datadir)/mime"
endif

uninstall-hook:
if ENABLE_UPDATE_MIMEDB
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(datadir)/mime"
endif

html: .doc_stamp

rtf: .doc_stamp

docs: pdf html

.PHONY: stats listfixmes listhacks check docs pdf html rtf
