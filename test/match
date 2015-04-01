#!/bin/sh
#
# File search script by <workexp@csc.liv.ac.uk>
# Usage: match [Filename]
#
IFS=":$IFS"

for substring in $@
do
  for path in $PATH
  do
    for command in $path/*$substring*
    do
      if [ -f $command ]
      then
        echo $command
      fi
    done
  done
done

# Added
echo "[$$] PAUSED... Hit return!"
read DUMMY
