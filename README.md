[![build status image](https://travis-ci.org/neurobin/shc.svg?branch=release)](https://travis-ci.org/neurobin/shc)
[![GitHub stars](https://img.shields.io/github/stars/neurobin/shc.svg)](https://github.com/neurobin/shc/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/neurobin/shc.svg)](https://github.com/neurobin/shc/network)
[![GitHub issues](https://img.shields.io/github/issues/neurobin/shc.svg)](https://github.com/neurobin/shc/issues)

# Shell Script Compiler

A generic shell script compiler. Shc takes a script, which is specified on the command line and produces C source code. The generated source code is then compiled and linked to produce a stripped binary executable.

The compiled binary will still be dependent on the shell specified in the first line of the shell code (i.e shebang) (i.e. #!/bin/sh), thus shc does not create completely independent binaries.

shc itself is not a compiler such as cc, it rather encodes and encrypts a shell script and generates C source code with the added expiration capability. It then uses the system compiler to compile a stripped binary which behaves exactly like the original script. Upon execution, the compiled binary will decrypt and execute the code with the shell -c option.

## Install

1. ./configure
2. make
3. sudo make install

**Note** If `make` fails due to *automake* version, run `./autogen.sh` before running the above commands.

### Ubuntu-specific

```
sudo add-apt-repository ppa:neurobin/ppa
sudo apt-get update
sudo apt-get install shc
```

If the above installation method seems like too much work, then just download a compiled binary package from [release page](https://github.com/neurobin/shc/releases/latest) and copy the `shc` binary to `/usr/bin` and `shc.1` file to `/usr/share/man/man1`.

## Usage

```
shc [options]
shc -f script.sh -o binary
shc -U -f script.sh -o binary # Untraceable binary (prevent strace, ptrace etc..)
```

## Testing

1. `cd` to `test` directory: `cd test`
1. Try: `shc -f test.bash -o test`, where <span class="light-quote">test.bash</span> is the bash source.
2. output binary file will be test. If no output file is specified
by the `-o` option, then it will create an executable with `.x` extension by default.

## Known bugs

The one (and I hope the only) limitation using shc is the _SC_ARG_MAX system configuration parameter.
It limits the maximum length of the arguments to the exec function, limiting the maximum length of the runnable script of shc.

!! - CHECK YOUR RESULTS CAREFULLY BEFORE USING - !!
<h3><a href="http://neurobin.github.io/shc/man.html">Man Page</a></h3>
<h3><a href="http://neurobin.github.io/shc">WebPage</a></h3>
