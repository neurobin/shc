# Makefile
#

INSTALL_PATH = /usr/local

# For SCO
CFLAGS = -b elf -O -D_SVID

# For IRIX
CFLAGS = -xansi -fullwarn -O3 -g0

# For Solaris
CFLAGS = -fast -xO4 -s -v -Xa

# For HPUX
CFLAGS = -Wall -O -Ae

# For OSF1
CFLAGS = -w -verbose -fast -std1 -g0

# For GNU C compiler
CFLAGS = -Wall # -O6 -pedantic

#SHELL = /bin/sh

all: shc ask_for_test

shc: shc.c
	$(CC) $(CFLAGS) $@.c -o $@

ask_for_test:
	@echo '***	Do you want to probe shc with a test script?: '
	@echo '***	Please try...	make test'

test: make_the_test ask_for_strings

make_the_test: match.x
	@echo '***	Running a compiled test script!'
	@echo '***	It must show files with substring "sh" in your PATH...'
	./match.x sh

match.x: shc match
	@echo '***	Compiling script "match"'
	CFLAGS="$(CFLAGS)" ./shc -v -f match

ask_for_strings:
	@echo '***	Do you want to see strings in the generated binary?: '
	@echo '***	Please try...	make strings'

strings: make_the_strings ask_for_expiration

make_the_strings: match.x
	@echo '***	Running: "strings -n 5 'match.x'"'
	@echo '***	It must show no sensible information...'
	strings -n 5 match.x

ask_for_expiration:
	@echo '***	Do you want to probe expiration date?: '
	@echo '***	Please try...	make expiration'

expiration: til_yesterday ask_for_install

til_yesterday: shc match
	@echo '***	Compiling "match" to expired date'
	CFLAGS="$(CFLAGS)" ./shc -vvv -e `date "+%d/%m/%Y"` -f match
	@echo '***	Running a compiled test script!'
	@echo '***	It must fail showing "./match.x: has expired!"'
	./match.x

ask_for_install:
	@echo '***	Do you want to install shc?: '
	@echo '***	Please try...	make install'

install: shc
	@echo '***	Installing shc and shc.1 on '$(INSTALL_PATH)
	install -c -s shc $(INSTALL_PATH)/bin/
	mkdir -p $(INSTALL_PATH)/man/man1/
	install -c -m 644 shc.1 $(INSTALL_PATH)/man/man1/

clean:
	rm -f *.o *~ *.x.c

cleanall: clean
	rm -f shc *.x

