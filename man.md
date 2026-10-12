% shc(1) shc user manual
%
% January 14, 2019
<hr>

# NAME
shc - Generic shell script compiler

# SYNOPSIS
**shc** [ -e *date* ] [ -m *addr* ] [ -i *iopt* ] [ -x *cmnd* ] [ -l *lopt* ] [ -o *outfile* ] [ -ABCDhUHvSr ] -f *script* 

# DESCRIPTION
**shc** creates a stripped binary executable version of the script specified with `-f` on the command line.

The binary version will get a `.x` extension appended by default if *outfile* is not defined with [-o *outfile*] option
and will usually be a bit larger in size than the original ascii code.
Generated C source code is saved in a file with the extension `.x.c` or in a file specified with appropriate option.

If you supply an expiration date with the `-e` option, the compiled binary will refuse to run after the date specified. 
The message **Please contact your provider** will be displayed instead. This message can be changed with the `-m` option.

Interpreter selection and built-in defaults are described under SCRIPT INTERPRETERS.

The compiled binary will still be dependent on the shell specified in the first line of the shell code (i.e. `#!/bin/sh`),
thus **shc** does not create completely independent binaries.

**shc** itself is not a compiler such as cc, it rather encodes and encrypts a shell script and generates C source code with the added expiration capability. 
It then uses the system compiler to compile a stripped binary which behaves exactly like the original script.
Upon execution, the compiled binary will decrypt and execute the code with the shell `-c` option.
Unfortunately, it will not give you any speed improvement as a real C program would.

**shc**'s main purpose is to protect your shell scripts from modification or inspection.
You can use it if you wish to distribute your scripts but don't want them to be easily readable by other people.   

# SCRIPT INTERPRETERS

Use a shebang that names the interpreter directly, such as `#!/bin/bash` or
`#!/bin/sh`. To find Bash on your system, run `command -v bash` and use the
absolute path it prints after `#!` on the first line of the script. For example,
if it prints `/bin/bash`:

```bash
#!/bin/bash
printf 'Hello, world!\n'
```

`#!/usr/bin/env bash` is not supported by automatic interpreter detection: **shc**
recognizes `env` as the interpreter and reports `Unknown shell (env)` rather
than looking up Bash through `PATH`. The compiled program still needs the
interpreter at the path specified in the shebang when it runs.

Built-in defaults are available for the interpreter names `sh`, `dash`, `bash`,
`zsh`, `bsh`, `Rsh`, `ksh`, `tsh`, `ash`, `csh`, `tcsh`, `rc`, and `perl`.
These names are matched against the last component of the interpreter path;
they are not a guarantee that every script or option works with every shell.
Other interpreter names require suitable `-i`, `-x`, and `-l` options. The
experimental `-H` mode has additional restrictions described under OPTIONS.

# OPTIONS

-e *date*
: Expiration date in *dd/mm/yyyy* format `[none]`

-m *message*
: message to display upon expiration `["Please contact your provider"]`

-f *script_name*
: File path of the script to compile 

-i *inline_option*
: Inline option for the shell interpreter i.e: `-e`

-x *command*
: eXec command, as a printf format i.e: `exec(\\'%s\\',@ARGV);` 

-l *last_option*
: Last shell option i.e: `--` 

-o *outfile*
: output to the file specified by outfile 

-r
: Relax security. Make a redistributable binary which executes on different systems running the same operating system. You can release your binary with this option for others to use 

-v
: Verbose compilation 

-S
: Switch ON setuid for root callable programs [OFF]

-D
: Switch on debug exec calls 

-U
: Make binary to be untraceable (using *strace*, *ptrace*, *truss*, etc.) 

-H
: Hardening. Extra security flag without root access requirement that protects against dumping, code injection, `cat /proc/pid/cmdline`, ptrace, etc.. This feature is **experimental** and may not work on all systems. it require bourne shell (sh) scripts

-C
: Display license and exit 

-A
: Display abstract and exit 

-B
: Compile for BusyBox 

-h
: Display help and exit 


# ENVIRONMENT VARIABLES

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

# AUTHORS
Francisco Rosales <frosal@fi.upm.es>

Md Jahidul Hamid <jahidulhamid@yahoo.com>

# REPORT BUGS TO
https://github.com/neurobin/shc/issues 

