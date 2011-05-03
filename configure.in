AC_INIT(aclocal.m4)
AM_INIT_AUTOMAKE(motor, "3.4.0")

AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

###	use option --enable-locales-fix to fix problems with double-byte
###	encodings on systems with broken locale, iscntrl()

AC_ARG_ENABLE(locales-fix,
	[  --enable-locales-fix    enables workaround for broken locales. use this
			  option if some of characters in your texts in Chinese
			  or Japanese diappear])

###     use option --with-fribidi to compile with fribidi support
AC_ARG_WITH(fribidi,
	[  --with-fribidi=[DIR]    enable Hebrew and Arabic support using fribidi
			  library in DIR],
	[with_fribidi=$withval],
	[with_fribidi=no])

if test "$with_fribidi" = "yes"; then
    for ac_dir in /usr/local /usr; do
	if test -f "$ac_dir/include/fribidi/fribidi.h"; then
	    with_fribidi=$ac_dir
	    break;
	fi
    done
fi

if test -n "$with_fribidi" -a "$with_fribidi" != "no"; then
    if test "$with_fribidi" = "yes"; then with_fribidi="/usr"; fi

    if test -r $with_fribidi/include/fribidi/fribidi.h; then
	LIBS="$LIBS `$with_fribidi/bin/fribidi-config --libs`"
    fi
fi

###	check for gnu make which is needed to compile programs from motor

AC_MSG_CHECKING(for GNU make)
gnu_make="gmake"

if test -z "`which gmake 2>/dev/null`"; then
    if test -z "`which make 2>/dev/null`"; then
	AC_MSG_ERROR([hey, what you gonna use to build the program?! you must have either gmake or make binary to do it])
    else
	if test -z "`make --version | grep 'GNU Make'`"; then
	    AC_MSG_ERROR([the make you have is not GNU])
	else
	    gnu_make="make"
	fi
    fi
fi

AC_MSG_RESULT($gnu_make)
AC_DEFINE_UNQUOTED(GNUMAKE, "$gnu_make", [gnu make program name])

AC_CHECK_LIB(ncurses, main,,
    [AC_CHECK_LIB(curses, main,, AC_MSG_ERROR(The ncurses terminal library is required in order to build the program))])

ALL_LINGUAS="ru pl de zh_CN"
AM_GNU_GETTEXT

AC_OUTPUT([Makefile
    src/Makefile
    src/debugger/Makefile
    src/executor/Makefile
    src/project/Makefile
    src/configuration/Makefile
    src/manager/Makefile
    src/grepper/Makefile
    src/vcs/Makefile
    src/tagbrowser/Makefile
    src/ui/Makefile
    src/ui/ncurses/Makefile
    kkconsui/Makefile
    kkconsui/src/Makefile
    kkconsui/include/Makefile
    kkstrtext/Makefile
    kksystr/Makefile
    kksystr/src/Makefile
    kksystr/include/Makefile
    parser/Makefile
    parser/src/Makefile
    parser/include/Makefile
    share/Makefile
    misc/Makefile
    po/Makefile.in
    intl/Makefile
    motor.spec
])
