# Shell Script Compiler

	A generic shell script compiler. Shc takes a script, which is
	specified on the command line and produces C source code. The
	generated source code is then compiled and linked to produce a
	stripped binary executable. Use with care.

Install:
--------

	do a "make"
	and then "sudo make install"

	or simply run the "install" file provided, in terminal


Testing:
--------

   Try:	 shc -f test.bash -o test

	output binary file will be test


Bugs:
-----

	The one (and I hope the only) limitation using shc is the
	_SC_ARG_MAX system configuration parameter.

	It limits the maximum length of the arguments to the exec function,
	limiting the maximum length of the runnable script of shc.

	!! - CHECK YOUR RESULTS CAREFULLY BEFORE USING - !!


Archived at: http://github.com/neurobin/shc/archive/release.zip.
Archived by: http://github.com/neurobin

Authors:  
--------

Francisco Rosales Garcia

<frosal@fi.upm.es>


Jahidul Hamid

http://github.com/neurobin



