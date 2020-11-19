SUBDIRS = ui debugger executor project configuration manager grepper \
    vcs tagbrowser

INCLUDES = -I$(top_srcdir)/kkstrtext -I$(top_srcdir)/kksystr/include \
    -I$(top_srcdir)/kkconsui/include -I$(top_srcdir)/intl \
    -I$(top_srcdir)/src/ui -I$(top_srcdir)/src/ui/ncurses \
    -I$(top_srcdir)/src/debugger -I$(top_srcdir)/src/executor \
    -I$(top_srcdir)/src/project -I$(top_srcdir)/src/configuration \
    -I$(top_srcdir)/src/manager -I$(top_srcdir)/src/grepper \
    -I$(top_srcdir)/src/tagbrowser -I$(top_srcdir)/src/vcs \
    -I$(top_srcdir)/parser/include

CPPFLAGS = -DSHARE_DIR=\"$(pkgdatadir)\" -DLOCALE_DIR=\"$(datadir)/locale\"

bin_PROGRAMS = motor
motor_SOURCES = main.cc motormisc.cc

EXTRA_DIST = motorcommon.h motormisc.h

motor_LDADD = project/libproject.a debugger/libdebugger.a \
    executor/libexecutor.a configuration/libconfiguration.a \
    manager/libmanager.a ui/ncurses/libncursesui.a ui/libui.a \
    grepper/libgrepper.a vcs/libvcs.a tagbrowser/libtagbrowser.a \
    ../parser/libparser.a ../kkstrtext/libkkstrtext.a \
    ../kkconsui/src/libkkconsui.a ../kksystr/src/libkksystr.a \
    @INTLLIBS@ @LIBS@
