
Purpose:

	A generic shell script compiler. Shc takes a script, which is
	specified on the command line and produces C source code. The
	generated source code is then compiled and linked to produce a
	stripped binary executable. Use with care.

Building:

	Just do a "make"


Testing:

   Try:	 "make test" 
    or:  "shc -v -f match"  then  "match.x sh"

	Caveat emptor: see Copyright

	The results look fine to me, but I havn't used this in anger, but
	the author has used shc for his work widely over SunOS, Solaris and
	Linux, and done some testing on Irix and HPUX. 

	We tested it on a few SMALL ksh scripts - big tasks should probably
	be written in C in the first place (see _SC_ARG_MAX below)!


Bugs:

	The one (and I hope the only) limitation using shc is the
	_SC_ARG_MAX system configuration parameter.

	It limits the maximum length of the arguments to the exec function,
	limiting the maximum length of the runnable script of shc.

	!! - CHECK YOUR RESULTS CAREFULLY BEFORE USING - !!


Archived at: ftp://hpux.csc.liv.ac.uk/hpux/Languages/shc-2.4a
Archived by: steff@csc.liv.ac.uk

Author:  Francisco Rosales Garcia
--------------------------------------------------------------------
 Francisco Rosales García <frosal@fi.upm.es>    TEL: +341 336 73 80
 http://www.datsi.fi.upm.es/~frosal            FAX: +34 1 336 73 73
 Departamento de Arquitectura y Tecnología de Sistemas Informáticos
 Facultad de Informática. Universidad Politécnica de Madrid. España
--------------------------------------------------------------------

