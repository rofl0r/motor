prefix=/usr/local
bindir=$(prefix)/bin
datadir=$(prefix)/share

SRCS=$(sort $(wildcard src/*.cc src/*/*.cc src/*/*/*.cc kkconsui/src/*.cc kkstrtext/*.cc kksystr/src/*.cc))
CSRCS=$(sort $(wildcard parser/src/*.c))
COBJS=$(CSRCS:.c=.o)
OBJS=$(SRCS:.cc=.o) $(COBJS)
INCLUDES=-Isrc -Iparser/include -Ikkstrtext -Isrc/ui -Isrc/project -Isrc/executor \
         -Isrc/debugger -Isrc/tagbrowser -Ikksystr/include -Isrc/configuration \
         -Ikkconsui/include -Isrc/manager -Isrc/vcs -Isrc/grepper -Isrc/ui/ncurses
PROGS=motor
TEMPLATES=$(sort $(wildcard share/templates/* share/templates/*/* share/templates/*/*/*))
LIBS=-lncurses
LDFLAGS=
CFLAGS=-O0 -g3
CPPFLAGS=-DINT=intptr_t -DUINT=uintptr_t
CXXFLAGS=$(CFLAGS)

-include config.mak

all: $(PROGS)

install: install-progs install-templates

install-progs: $(PROGS:%=$(DESTDIR)$(bindir)/%)
install-templates: #$(TEMPLATES:share/%=$(DESTDIR)$(datadir)/motor/%)
	mkdir -p $(DESTDIR)$(datadir)/motor
	cp -r share/templates $(DESTDIR)$(datadir)/motor/


$(DESTDIR)$(bindir)/%: %
	install -D -m 755  $< $@

#$(DESTDIR)$(datadir)/%: share/%
#	install -D -m 644  $< $@

clean:
	rm -f $(PROGS)
	rm -f $(OBJS)

motor: $(OBJS)
	$(CXX) -o motor $^ $(LDFLAGS) $(LIBS)

%.o: %.cc
	$(CXX) -DSHARE_DIR=\"$(datadir)/motor\" $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS) -c -o  $@ $<

%.o: %.c
	$(CC) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) -c -o  $@ $<

