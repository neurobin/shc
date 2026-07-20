% shc(1) shc user manual
%
% August 19, 2024
<hr>

# NAME
shc - Generic shell script compiler

# SYNOPSIS
**shc** \[ -e *DATE* \] \[ -m *MESSAGE* \] \[ -i *IOPT* \] \[ -x *CMD* \] \[ -l *LOPT* \] \[ -o *OUTFILE* \] \[ -2ABCDHpPSUhrv \] -f *SCRIPT* 

# DESCRIPTION
**shc** creates a stripped binary executable version of the script specified with `-f` on the command line.

The binary version will get a `.x` extension appended by default if *OUTFILE* is not defined with [-o *OUTFILE*] option
and will usually be a bit larger in size than the original ascii code.
Generated C source code is saved in a file with the extension `.x.c` or in a file specified with appropriate option.

If you provide an expiration DATE with the `-e` option, the compiled binary will refuse to run after the date specified. 
The message **Please contact your provider** will be displayed instead. This message can be changed with the `-m` option.

You can compile any kind of shell script, but you need to supply valid `-i`, `-x` and `-l` options.

The compiled binary will still require the shell specified in the first line of the shell code (i.e. `#!/bin/sh`) to be available on the system,
therefore **shc** does not create completely independent binaries, it mainly obfuscates the source script.

**shc** itself is not a compiler such as cc, it rather encodes and encrypts a shell script and generates C source code with the added expiration capability. 
It then uses the system compiler to compile a stripped binary which behaves exactly like the original script.
Upon execution, the compiled binary will decrypt and execute the code with the shell `-c` option.
It will not give you any speed improvement as a real C program would.

**shc**'s main purpose is to protect your shell scripts from modification or inspection.
You can use it if you wish to distribute your scripts but don't want them to be easily readable by other people.   

# OPTIONS

-e *DATE*
: Expiration date in *dd/mm/yyyy* format `[none]`

-m *MESSAGE*
: message to display upon expiration `["Please contact your provider"]`

-f *SCRIPT*
: File path of the script to compile 

-P
: Use a pipe to feed the script, with ARGV fixes.  Enabled automatically
  for `python`, `perl` and `csh`.

-p
: Use a pipe to feed the script, without ARGV fixing.

-i *IOPT*
: Inline option for the shell interpreter i.e: `-e`

-x *CMD*
: eXec command, as a printf format i.e: `exec(\\'%s\\',@ARGV);` 

-l *LOPT*
: Last shell option i.e: `--` 

-o *OUTFILE*
: output to the file specified by OUTFILE

-r
: Relax security. Make a redistributable binary which executes on different systems running the same operating system. You can release your binary with this option for others to use 

-v
: Verbose compilation 

-S
: Enable setuid for root callable programs

-D
: Enable debug (show exec calls, etc.)

-U
: Make binary execution untraceable (using *strace*, *ptrace*, *truss*, etc.) 

-H
: Hardening. Extra security flag without root access requirement that protects against dumping, code injection, `cat /proc/pid/cmdline`, `ptrace`, etc...  This feature is **experimental** and may not work on all systems. it requires bourne shell (sh) scripts

-C
: Display license and exit 

-A
: Display abstract and exit 

-2
: Use `mmap2` system call.

-B
: Compile for BusyBox 

-h
: Display help and exit 


# ENVIRONMENT VARIABLES

These can be used to provide options to the GCC Compiler.
Examples: static compilation, machine architecture, sanitize options.

CC
: C compiler command `[cc]`

CFLAGS
: C compiler flags `[none]`

LDFLAGS
: Linker flags `[none]`
 
# EXAMPLES

Compile a script which can be run on other systems with the trace option enabled (without `-U` flag):

```bash
shc -f myscript -o mybinary
```

Compile an untraceable binary:

```bash
shc -Uf myscript -o mybinary
```

Compile an untraceable binary that doesn't require root access (experimental):

```bash
shc -Hf myscript -o mybinary
```
 
# LIMITATIONS
The maximum size of the script that could be executed once compiled is limited by the operating system configuration parameter `_SC_ARG_MAX` (see sysconf(2))

# MAIN AUTHORS

Francisco Rosales <frosal@fi.upm.es>
Md Jahidul Hamid <jahidulhamid@yahoo.com>

Note: Do not contact them, they are no longer actively involved

# REPORT BUGS TO
https://github.com/NextSHC/shc/issues 
