[![build status image](https://travis-ci.org/neurobin/shc.svg?branch=release)](https://travis-ci.org/neurobin/shc)
[![GitHub stars](https://img.shields.io/github/stars/neurobin/shc.svg)](https://github.com/neurobin/shc/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/neurobin/shc.svg)](https://github.com/neurobin/shc/network)
[![GitHub issues](https://img.shields.io/github/issues/neurobin/shc.svg)](https://github.com/neurobin/shc/issues)

# Shell Script Compiler

A generic shell script compiler. Shc takes a script, which is specified on the command line and produces C source code. The generated source code is then compiled and linked to produce a stripped binary executable.

The compiled binary will still be dependent on the shell specified in the first line of the shell code (i.e shebang) (i.e. `#!/bin/sh`), thus shc does not create completely independent binaries.

shc itself is not a compiler such as cc, it rather encodes and encrypts a shell script and generates C source code with the added expiration capability. It then uses the system compiler to compile a stripped binary which behaves exactly like the original script. Upon execution, the compiled binary will decrypt and execute the code with the shell -c option.

## Install

### Building & installing locally

```bash
./configure
make
sudo make install
```

**Note** If `make` fails due to *automake*'s version, run `./autogen.sh` before running the above commands.

### Debian GNU/Linux and Ubuntu systems

```bash
sudo apt-get install shc
```

### Ubuntu systems (via PPA repository)

```bash
sudo add-apt-repository ppa:neurobin/ppa
sudo apt-get update
sudo apt-get install shc
```

If the above installation method seems like too much work, then just download a compiled binary package from [release page](https://github.com/neurobin/shc/releases/latest) and copy the `shc` binary to `/usr/bin` and `shc.1` file to `/usr/share/man/man1`.

## Usage

```bash
shc [options]
shc -f script.sh -o binary
shc -U -f script.sh -o binary  # Untraceable binary (prevent strace, ptrace etc..)
shc -H -f script.sh -o binary  # Untraceable binary, does not require root (only bourne shell (sh) scripts with no parameter)
```

## The hardening flag `-H`

This flag is currently in an experimental state and may not work in all systems. This flag only works for **default** shell. For example, if you compile a **bash** script with `-H` flag then the resulting executable will only work in systems where the default shell is **bash**. You may change the default shell which generally is `/bin/sh` which further is just a link to another shell like bash or dash etc.

**Also `-H` does not work with positional parameters (yet)**

## Testing

```bash
./configure
make
make test
```

Temporary directories are generally created in `/tmp/shc.SHELL.OPT.XXX.tst` (caps are replaced according to test).
When a test succeeds, the directory is removed, if not, it is kept to help with debugging.

Clean up test outputs manually with:

```bash
rm /tmp/shc.*.tst
```


## Known limitations

- SCRIPT length: The `_SC_ARG_MAX` system configuration parameter limits the .
length of the arguments to the exec function.
  With standard options this limits the maximum length of the runnable script of shc.
  However, you can now use the `-P` options which uses a pipe which circumvents this limitation.


!! - CHECK YOUR RESULTS CAREFULLY BEFORE USING - !!

## Links

1. [Man Page](https://neurobin.github.io/shc/man.html)
2. [Web Page](https://neurobin.github.io/shc)

# Contributing

If you want to make pull requests, please do so against the **master** branch. The default branch is **release** which should contain clean package files ready to be used.

If you want to edit the manual, please edit the **man.md** file.
The ci flow  will generate the other manual files from it.
You can also generate these locally with the following commands (requires `pandoc`):

```bash
pandoc -s man.md -t man -o shc.1
#also run this command to generate the html manual
pandoc -s man.md -t html -o man.html
```

If you change anything related to `autotools`, `./autogen.sh` should be run to regenerate the derived files.
Again, the ci flow will generate these automatically.

You may need to pull after a push because of the changes committed by ci.
