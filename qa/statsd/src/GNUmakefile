TOPDIR = ../../..
include $(TOPDIR)/src/include/builddefs

TESTDIR = $(PCP_VAR_DIR)/testsuite/statsd/src

SUBDIRS = cases utils

ifeq "$(PMDA_STATSD)" "true"
default setup default_pcp: $(SUBDIRS)
	$(SUBDIRS_MAKERULE)

install install_pcp: $(SUBDIRS)
	$(INSTALL) -m 755 -d $(TESTDIR)
	$(INSTALL) -m 644 GNUmakefile.install $(TESTDIR)/GNUmakefile
	$(SUBDIRS_MAKERULE)
else
default setup default_pcp:
install install_pcp:
endif

include $(BUILDRULES)
