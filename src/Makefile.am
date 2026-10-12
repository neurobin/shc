bin_PROGRAMS = shc
shc_SOURCES = shc.c


check: shc
	/bin/bash "$(srcdir)/../test/runner.sh"
	$(srcdir)/../test/ttest.sh ./shc

