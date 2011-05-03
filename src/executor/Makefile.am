noinst_LIBRARIES = libexecutor.a

INCLUDES = -I$(top_srcdir)/kkstrtext -I$(top_srcdir)/kkconsui/include \
    -I$(top_srcdir)/kksystr/include -I$(top_srcdir)/intl \
    -I$(top_srcdir)/src/ui -I$(top_srcdir)/src/debugger \
    -I$(top_srcdir)/src/executor -I$(top_srcdir)/src/project \
    -I$(top_srcdir)/src/tagbrowser -I$(top_srcdir)/src/configuration \
    -I$(top_srcdir)/src -I$(top_srcdir)/parser/include

libexecutor_a_SOURCES = parserule.cc motorexecutor.cc

EXTRA_DIST = parserule.h motorexecutor.h
