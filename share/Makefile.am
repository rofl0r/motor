
mshare_DATA = external syntax themes CVS.vcsrc
msharedir = $(datadir)/motor

tmpl.tar.gz: dist-hook
dist-hook:
	-if test -d templates; \
	    then tar zcvf tmpl.tar.gz --exclude "*CVS*" templates/*; \
	fi

clean-local:
	-if test -d templates; then rm -f tmpl.tar.gz; fi

install-data-local: tmpl.tar.gz
	tar zxvf tmpl.tar.gz -C $(datadir)/motor/
	-find $(datadir)/motor/ -name CVS -type d -exec rm -rf {} \;
	if test -d templates; then rm -f tmpl.tar.gz; fi

uninstall-local:
	rm -rf $(datadir)/motor/templates

EXTRA_DIST = tmpl.tar.gz $(mshare_DATA)
