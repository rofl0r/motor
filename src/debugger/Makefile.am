noinst_LIBRARIES = libdebugger.a

INCLUDES = -I$(top_srcdir)/kkstrtext -I$(top_srcdir)/kkconsui/include \
    -I$(top_srcdir)/kksystr/include -I$(top_srcdir)/intl \
    -I$(top_srcdir)/src/ui/ncurses -I$(top_srcdir)/src/debugger \
    -I$(top_srcdir)/src/executor -I$(top_srcdir)/src/project \
    -I$(top_srcdir)/src -I$(top_srcdir)/parser/include \
    -I$(top_srcdir)/src/ui -I$(top_srcdir)/src/configuration \
    -I$(top_srcdir)/src/tagbrowser

libdebugger_a_SOURCES = breakpoint.cc debuggercommand.cc debuggermessage.cc \
    watch.cc motordebugger.cc stackitem.cc

EXTRA_DIST = breakpoint.h debuggercommand.h debuggermessage.h watch.h \
    motordebugger.h stackitem.h
