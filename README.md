# Shell Script Compiler

	A generic shell script compiler. Shc takes a script, which is
	specified on the command line and produces C source code. The
	generated source code is then compiled and linked to produce a
	stripped binary executable. Use with care.

Install:
--------
	cd to directory "main": cd main
	do a "make"
	and then "sudo make install"

<<<<<<< HEAD
	or simply run the "install" file provided, in terminal
=======
	or simply run the "install" script/file provided, in terminal
>>>>>>> master


Testing:
--------
	cd to test "directory": cd test
	Try: shc -f test.bash -o test

<<<<<<< HEAD
	output binary file will be test
	the sequence of -o -f options doesn't matter
=======
	output binary file will be test. If no output file is specified
	by the -o option, then it will create an executable with .x extension
	by default
>>>>>>> master


Known bugs:
-----------

	The one (and I hope the only) limitation using shc is the
	_SC_ARG_MAX system configuration parameter.

	It limits the maximum length of the arguments to the exec function,
	limiting the maximum length of the runnable script of shc.

	!! - CHECK YOUR RESULTS CAREFULLY BEFORE USING - !!


Archived at: http://github.com/neurobin/shc/archive/release.zip.

Authors:  
--------

Francisco Rosales Garcia

<frosal@fi.upm.es>


Jahidul Hamid

http://github.com/neurobin



