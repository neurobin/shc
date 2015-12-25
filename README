# Shell Script Compiler

A generic shell script compiler. Shc takes a script, which is
specified on the command line and produces C source code. The
generated source code is then compiled and linked to produce a
stripped binary executable. 

The  compiled  binary  will  still  be dependent on the shell specified in the
first line of the shell code (i.e shebang) (i.e. #!/bin/sh), thus shc does not  create  com‐
pletely independent binaries.

shc  itself  is  not  a  compiler such as cc, it rather encodes and encrypts a
shell script and generates C source code with the added expiration capability.
It  then  uses  the system compiler to compile a stripped binary which behaves
exactly like the original script. Upon execution,  the  compiled  binary  will
decrypt  and  execute  the  code with the shell -c option.

Install:
--------

1. cd to directory "main": `cd main`
2. do a `make`
3. and then `sudo make install`
4. or simply run the `install` script/file provided, in terminal.

Usage:
------
```
shc [options]
shc -f script.sh -o binary
```



Testing:
--------

1. cd to `test` directory: `cd test`
2. Try: `shc -f test.bash -o test`
3. output binary file will be test. If no output file is specified
by the `-o` option, then it will create an executable with `.x` extension by default.


Known bugs:
-----------

The one (and I hope the only) limitation using shc is the
_SC_ARG_MAX system configuration parameter.

It limits the maximum length of the arguments to the exec function,
limiting the maximum length of the runnable script of shc.

!! - CHECK YOUR RESULTS CAREFULLY BEFORE USING - !!
<h2><a href="http://neurobin.github.io/shc">WebPage</a></h2>
