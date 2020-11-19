# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4-p6 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ../..
prefix = /usr
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/share/info
mandir = ${prefix}/share/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/motor
pkglibdir = $(libdir)/motor
pkgincludedir = $(includedir)/motor

top_builddir = ../..

ACLOCAL = /home/witherking25/motor/motor/missing aclocal-1.4
AUTOCONF = autoconf
AUTOMAKE = /home/witherking25/motor/motor/missing automake-1.4
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = x86_64-pc-linux-gnu
host_triplet = x86_64-pc-linux-gnu
BUILD_INCLUDED_LIBINTL = no
CATOBJEXT = .gmo
CC = gcc
CXX = c++
DATADIRNAME = share
GENCAT = gencat
GLIBC21 = yes
GMSGFMT = /usr/bin/msgfmt
HAVE_LIB = @HAVE_LIB@
INSTOBJEXT = .mo
INTLBISON = bison
INTLLIBS = 
INTLOBJS = 
INTL_LIBTOOL_SUFFIX_PREFIX = 
LIB = @LIB@
LIBICONV = 
LIBINTL = 
LTLIB = @LTLIB@
LTLIBICONV = 
LTLIBINTL = 
MAKEINFO = makeinfo
MKINSTALLDIRS = $(top_builddir)/./mkinstalldirs
PACKAGE = motor
POSUB = po
RANLIB = ranlib
USE_INCLUDED_LIBINTL = no
USE_NLS = yes
VERSION = 3.4.0

noinst_LIBRARIES = libdebugger.a

INCLUDES = -I$(top_srcdir)/kkstrtext -I$(top_srcdir)/kkconsui/include     -I$(top_srcdir)/kksystr/include -I$(top_srcdir)/intl     -I$(top_srcdir)/src/ui/ncurses -I$(top_srcdir)/src/debugger     -I$(top_srcdir)/src/executor -I$(top_srcdir)/src/project     -I$(top_srcdir)/src -I$(top_srcdir)/parser/include     -I$(top_srcdir)/src/ui -I$(top_srcdir)/src/configuration     -I$(top_srcdir)/src/tagbrowser


libdebugger_a_SOURCES = breakpoint.cc debuggercommand.cc debuggermessage.cc     watch.cc motordebugger.cc stackitem.cc


EXTRA_DIST = breakpoint.h debuggercommand.h debuggermessage.h watch.h     motordebugger.h stackitem.h

mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../config.h
CONFIG_CLEAN_FILES = 
LIBRARIES =  $(noinst_LIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I../..
CPPFLAGS = 
LDFLAGS = 
LIBS = -lncurses  
libdebugger_a_LIBADD = 
libdebugger_a_OBJECTS =  breakpoint.o debuggercommand.o \
debuggermessage.o watch.o motordebugger.o stackitem.o
AR = ar
CXXFLAGS = -g -O2
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@
DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP_ENV = --best
SOURCES = $(libdebugger_a_SOURCES)
OBJECTS = $(libdebugger_a_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .cc .o .s
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps src/debugger/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-noinstLIBRARIES:

clean-noinstLIBRARIES:
	-test -z "$(noinst_LIBRARIES)" || rm -f $(noinst_LIBRARIES)

distclean-noinstLIBRARIES:

maintainer-clean-noinstLIBRARIES:

.c.o:
	$(COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

libdebugger.a: $(libdebugger_a_OBJECTS) $(libdebugger_a_DEPENDENCIES)
	-rm -f libdebugger.a
	$(AR) cru libdebugger.a $(libdebugger_a_OBJECTS) $(libdebugger_a_LIBADD)
	$(RANLIB) libdebugger.a
.cc.o:
	$(CXXCOMPILE) -c $<

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = src/debugger

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
breakpoint.o: breakpoint.cc breakpoint.h ../../src/motorcommon.h \
	../../parser/include/parser.h ../../parser/include/parlist.h \
	../../config.h ../../kkstrtext/kkstrtext.h \
	../../kkstrtext/conf.h
debuggercommand.o: debuggercommand.cc debuggercommand.h \
	../../src/motorcommon.h ../../parser/include/parser.h \
	../../parser/include/parlist.h ../../config.h \
	../../kkstrtext/kkstrtext.h ../../kkstrtext/conf.h
debuggermessage.o: debuggermessage.cc debuggermessage.h \
	../../src/motorcommon.h ../../parser/include/parser.h \
	../../parser/include/parlist.h ../../config.h \
	../../kkstrtext/kkstrtext.h ../../kkstrtext/conf.h
motordebugger.o: motordebugger.cc motordebugger.h \
	../../src/motorcommon.h ../../parser/include/parser.h \
	../../parser/include/parlist.h ../../config.h \
	../../kkstrtext/kkstrtext.h ../../kkstrtext/conf.h breakpoint.h \
	debuggercommand.h debuggermessage.h stackitem.h \
	../../src/ui/motorui.h ../../src/executor/motorexecutor.h \
	../../src/executor/parserule.h ../../src/project/motorproject.h \
	../../src/project/projectname.h \
	../../src/project/projectfiles.h \
	../../src/project/motorfolder.h ../../src/project/motorfile.h \
	../../src/project/projecttake.h \
	../../src/project/projectpaths.h \
	../../src/project/projecttempl.h \
	../../src/project/sourcetemplate.h \
	../../src/project/motordist.h ../../src/project/projectdesk.h \
	../../src/project/projectdebug.h watch.h \
	../../src/tagbrowser/motortagbrowser.h ../../src/motormisc.h \
	../../src/configuration/motorconfiguration.h \
	../../kksystr/include/kkiproc.h ../../kksystr/include/conf.h \
	../../kksystr/include/kkfsys.h
stackitem.o: stackitem.cc motordebugger.h ../../src/motorcommon.h \
	../../parser/include/parser.h ../../parser/include/parlist.h \
	../../config.h ../../kkstrtext/kkstrtext.h \
	../../kkstrtext/conf.h breakpoint.h debuggercommand.h \
	debuggermessage.h stackitem.h ../../src/ui/motorui.h
watch.o: watch.cc watch.h ../../src/motorcommon.h \
	../../parser/include/parser.h ../../parser/include/parlist.h \
	../../config.h ../../kkstrtext/kkstrtext.h \
	../../kkstrtext/conf.h

info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am:
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am:
uninstall: uninstall-am
all-am: Makefile $(LIBRARIES)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-noinstLIBRARIES mostlyclean-compile \
		mostlyclean-tags mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-noinstLIBRARIES clean-compile clean-tags clean-generic \
		mostlyclean-am

clean: clean-am

distclean-am:  distclean-noinstLIBRARIES distclean-compile \
		distclean-tags distclean-generic clean-am

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-noinstLIBRARIES \
		maintainer-clean-compile maintainer-clean-tags \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-noinstLIBRARIES distclean-noinstLIBRARIES \
clean-noinstLIBRARIES maintainer-clean-noinstLIBRARIES \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile tags mostlyclean-tags distclean-tags \
clean-tags maintainer-clean-tags distdir info-am info dvi-am dvi check \
check-am installcheck-am installcheck install-exec-am install-exec \
install-data-am install-data install-am install uninstall-am uninstall \
all-redirect all-am all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
