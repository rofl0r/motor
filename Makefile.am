
SUBDIRS = intl po parser kkstrtext kksystr kkconsui src share misc

man_MANS = motor.1

EXTRA_DIST = config.rpath  config.rpath  AUTHORS ChangeLog COPYING FAQ INSTALL README TODO \
    tutorial/*.html tutorial/images/*.png tutorial/images/*.gif \
    tutorial/images/*.jpg motor.spec motor.1

AUTOMAKE_OPTIONS = foreign

CLEANFILES = *.o motor

rpm: dist
	mv $(PACKAGE)-$(VERSION).tar.gz ..
	cd ..; rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz
	find ~/rpm -name "$(PACKAGE)-$(VERSION)*rpm" -exec mv {} ~/devel/ \;
