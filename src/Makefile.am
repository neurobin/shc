bin_PROGRAMS = shc
shc_SOURCES = shc.c


check: shc
	CC="$(CC)" CPPFLAGS="$(CPPFLAGS)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" /bin/bash "$(srcdir)/../test/arg-max.sh"
	$(srcdir)/../test/ttest.sh ./shc

