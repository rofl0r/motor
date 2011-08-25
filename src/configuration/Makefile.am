noinst_LIBRARIES = libconfiguration.a

INCLUDES = -I$(top_srcdir)/kkstrtext -I$(top_srcdir)/kkconsui/include \
    -I$(top_srcdir)/kksystr/include -I$(top_srcdir)/intl \
    -I$(top_srcdir)/src/ui/ncurses -I$(top_srcdir)/src/debugger \
    -I$(top_srcdir)/src/executor -I$(top_srcdir)/src/project \
    -I$(top_srcdir)/src/ui -I$(top_srcdir)/src/tagbrowser -I$(top_srcdir)/src \
    -I$(top_srcdir)/parser/include

AM_CPPFLAGS = -DSHARE_DIR=\"$(pkgdatadir)\" -DLOCALE_DIR=\"$(datadir)/locale\"

libconfiguration_a_SOURCES = motorconfiguration.cc extkey.cc

EXTRA_DIST = motorconfiguration.h
