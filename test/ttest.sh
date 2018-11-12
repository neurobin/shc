#!/bin/bash

shells=('/bin/sh' '/bin/dash' '/bin/bash' '/bin/ash' '/bin/ksh' '/bin/zsh' '/usr/bin/tcsh' '/bin/csh' '/usr/bin/rc')
## Install: sudo apt install dash bash ash ksh zsh tcsh csh rc

shc=${1-shc}

txtred='\e[0;31m' # Red
txtgrn='\e[0;32m' # Green
txtrst='\e[0m'    # Text Reset

stat=0
pc=0
fc=0
echo
echo "== Running tests ..."
for shell in ${shells[@]}; do
    tmpd=$(mktemp -d)
    tmpf="$tmpd/test.$(basename $shell)"
    echo '#!'"$shell
    echo 'Hello World'
    " > "$tmpf"
    "$shc" -f "$tmpf" -o "$tmpd/a.out"
    out=$("$tmpd/a.out")
    if [[ "$out" = *'Hello World'* ]]; then
        echo -e "$shell: ${txtgrn}PASSED${txtrst}"
        ((pc++))
    else
        echo -e "$shell: ${txtred}FAILED${txtrst}"
        stat=1
        ((fc++))
    fi
    rm -r "$tmpd"
done

echo
echo "Test Summary"
echo "------------"

if ((pc>0)); then
    pt="${txtgrn}PASSED${txtrst}"
else
    pt="PASSED"
fi

if ((fc>0)); then
    ft="${txtred}FAILED${txtrst}"
else
    ft="FAILED"
fi

echo -e "$pt: $pc"
echo -e "$ft: $fc"
echo "------------"
echo

exit $stat
