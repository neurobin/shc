[![build status image](https://travis-ci.org/neurobin/shc.svg?branch=release)](https://travis-ci.org/neurobin/shc)
[![GitHub stars](https://img.shields.io/github/stars/neurobin/shc.svg)](https://github.com/neurobin/shc/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/neurobin/shc.svg)](https://github.com/neurobin/shc/network)
[![GitHub issues](https://img.shields.io/github/issues/neurobin/shc.svg)](https://github.com/neurobin/shc/issues)

# Shell Script Compiler

A generic shell script compiler. Shc takes a script, which is specified on the command line and produces C source code. The generated source code is then compiled and linked to produce a stripped binary executable.

The compiled binary will still be dependent on the shell specified in the first line of the shell code (i.e shebang) (i.e. #!/bin/sh), thus shc does not create completely independent binaries.

shc itself is not a compiler such as cc, it rather encodes and encrypts a shell script and generates C source code with the added expiration capability. It then uses the system compiler to compile a stripped binary which behaves exactly like the original script. Upon execution, the compiled binary will decrypt and execute the code with the shell -c option.

## Install

```bash
./configure
make
sudo make install
```

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
shc -H -f script.sh -o binary # Untraceable binary, does not require root (only bourne shell (sh) scripts with no parameter)
```

## Redistributing binaries with -r

By default, shc binds the compiled script to metadata of the shell executable
specified by its shebang. This metadata includes the shell file's inode, device,
owner, group, size, and timestamps. If it changes, the compiled script can fail
to decrypt correctly and print unreadable characters. This can happen after a
shell update or when moving the binary to another machine or container, and on
some systems after a reboot.

Use `-r` when the binary needs to run independently of this shell-file metadata:

```bash
shc -r -f script.sh -o binary
```

The description "Relax security" means that `-r` skips using this metadata in
the encryption key. The script remains encrypted in the generated binary, but
it is no longer bound to that particular shell file. This option does not change
the expiration date set with `-e` or the restrictions imposed by `-U` and `-H`.

The target system still needs a compatible operating system, CPU architecture,
and any shared libraries required by the binary. It must also provide the shell
at the path specified by the shebang and the commands and files used by the
script. `-r` does not bundle these dependencies or make a binary work across
different operating systems. Test the binary on each intended target system.

## The hardening flag -H

This flag is currently in an experimental state and may not work in all systems. This flag only works for **default** shell. For example, if you compile a **bash** script with `-H` flag then the resultant executable will only work in systems where the default shell is **bash**. You may change the default shell which generally is `/bin/sh` which further is just a link to another shell like bash or dash etc.

**Also `-H` does not work with positional parameters (yet)**

## Testing

```bash
./configure
make
make check
```

## Known limitations

The one (and I hope the only) limitation using shc is the _SC_ARG_MAX system configuration parameter.
It limits the maximum length of the arguments to the exec function, limiting the maximum length of the runnable script of shc.

!! - CHECK YOUR RESULTS CAREFULLY BEFORE USING - !!

## Links

1. [Man Page](http://neurobin.github.io/shc/man.html)
2. [Web Page](http://neurobin.github.io/shc)

# Contributing

If you want to make pull requests, please do so against the **master** branch. The default branch is **release** which should contain clean package files ready to be used.

If you want to edit the manual, please edit the **man.md** file (available in the master branch) instead and then generate the manual file from it with the command (requires `pandoc` to be installed):

```bash
pandoc -s man.md -t man -o shc.1
#also run this command to generate the html manual
pandoc -s man.md -t html -o man.html
```

If you change anything related to autotools, please run `./autogen.sh` afterwards.
