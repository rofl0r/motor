INCLUDES = -I$(top_srcdir)/kkstrtext -I$(top_srcdir)/kksystr/include \
    -I$(top_srcdir)/intl -I$(top_srcdir)/src/debugger \
    -I$(top_srcdir)/src/executor -I$(top_srcdir)/src/configuration \
    -I$(top_srcdir)/src/ui -I$(top_srcdir)/src -I$(top_srcdir)/src/manager \
    -I$(top_srcdir)/src/vcs -I$(top_srcdir)/src/tagbrowser \
    -I$(top_srcdir)/parser/include

noinst_LIBRARIES = libproject.a

libproject_a_SOURCES = motorfile.cc motorfolder.cc motorproject.cc \
    projectdebug.cc projectdesk.cc projectfiles.cc projectname.cc \
    projectpaths.cc projecttempl.cc sourcetemplate.cc projecttake.cc \
    motordist.cc

EXTRA_DIST = motorfile.h motorfolder.h motorproject.h projectdebug.h \
    projectdesk.h projectfiles.h projectname.h projectpaths.h \
    projecttempl.h sourcetemplate.h projecttake.h motordist.h
