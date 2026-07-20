#!/bin/bash
# shellcheck disable=2016,2028
shells=('/bin/sh' '/bin/dash' '/bin/bash' '/bin/ksh' '/bin/zsh' '/usr/bin/tcsh' '/bin/csh' '/usr/bin/rc' '/usr/bin/python' '/usr/bin/python2' '/usr/bin/python3' '/usr/bin/perl')
## Install: sudo apt install dash bash ksh zsh tcsh csh rc

check_opts=('' '-r' '-v' '-D' '-S' '-P' '-p' '-H' '-2')

shc=${1-shc}

txtred='\e[0;31m' # Red
txtgrn='\e[0;32m' # Green
txtrst='\e[0m'    # Text Reset

stat=0
pc=0
fc=0
# Comma separated list of shells that are skipped
SKIP=",${SKIP},ash,"
echo
echo "== Running tests ... (Skip expression: $SKIP)"
for shell in "${shells[@]}"; do
    BASESHELL=${shell##*/}
    if [ "${SKIP#*,"${BASESHELL}",}" != "$SKIP" ] ; then
        echo    "===================================================="
        echo -e "=== $shell                :SKIPPED"
        echo    "===================================================="
        continue
    fi
    if [ ! -x "$shell" ] ; then
        echo    "===================================================="
        echo -e "=== $shell                :${txtred}MISSING${txtrst}"
        echo    "===================================================="
        ((fc++))
        stat=1
        continue
    fi
    for opt in "${check_opts[@]}"; do
        if [ "${opt}" = "-H" ] ; then
            if [ "${shell#*sh}" = "$shell" ] ; then
                # Only supported for "bourne shell"
                continue
            fi
        fi
        tmpd=$(mktemp -d "/tmp/shc.${BASESHELL}${opt}.XXX.tst")
        tmpf="$tmpd/test.$(basename "$shell")"
        tmpa="$tmpd/a.out.${BASESHELL}$opt"
        tmpl="$tmpd/a.log"
        out=""
        firstarg='first quote" and space'
        secondarg="secondWithSingleQuote'"

        # Default values for echo/print and expectations
        args_echo=' fp:(1) sp:(2)'
        args_expected=" fp:${firstarg} sp:${secondarg}"
        sn_echo=' sn:(0)'
        sn_expected=" sn:${tmpa}"

        # Modify defaults according to test/skip test
        if [ "${opt}" = "-H" ] ; then
            # -H does not support arguments
            args_echo=
            args_expected=
            sn_echo=
            sn_expected=
        elif [ "${opt}" = "-p" ] ; then
            # -p does not suppose $0 support
            sn_echo=
            sn_expected=
        fi

        default_echo="${shell}: Hello World${sn_echo}${args_echo}"
        expected="${shell}: Hello World${sn_expected}${args_expected}"

        arg_only_echo="${shell}: Hello World${args_echo}"
        arg_only_expected="${shell}: Hello World${args_expected}"

        {
            echo '#!'"$shell"
            if [ "${shell#*/pyth}" != "$shell" ] ; then
                # Python
                default_echo="${default_echo//\(/\{}"  # Use % to indicate arg
                default_echo="${default_echo//)/\}}"    # No end parentheses
                echo 'import sys; sys.stdout.write("'"${default_echo}"'".format(*sys.argv)+"\n")'
            elif [ "$BASESHELL" = "rc" ] ; then
                default_echo="${default_echo//\(/\$}"  # Use $ to indicate arg
                default_echo="${default_echo//)/}"    # No end parentheses
                arg_only_echo="${arg_only_echo//\(/\$}"  # Use $ to indicate arg
                arg_only_echo="${arg_only_echo//)/}"    # No end parentheses
                # rc
                if [ "$opt" != "-P" ] ; then
                    echo "echo ${default_echo}"
                else
                    echo "echo ${arg_only_echo}"
                    expected="${arg_only_expected}"
                fi
            elif [ "${shell#*/perl}" != "$shell" ] ; then
                # perl
                default_echo="${default_echo//\(/\$ARGV\[}"  # Use $ARGV[ to indicate arg
                default_echo="${default_echo//)/\]}"         # Place ] after arg index
                default_echo="${default_echo//\$ARGV\[0\]/\$0}"  # $0 for first argument
                default_echo="${default_echo//1/0}"  # Argument shift
                default_echo="${default_echo//2/1}"  # Argument shift
                echo 'print "'"${default_echo}"'";'
            elif [ "${shell#*/csh}" != "$shell" ] ; then
                # csh - can not forge $0
                arg_only_echo="${arg_only_echo//\(/\$}"  # Use $ to indicate arg
                arg_only_echo="${arg_only_echo//)/}"    # No end parentheses
                echo 'echo "'"${arg_only_echo}"'"'
                expected="${arg_only_expected}"
            else
                default_echo=${default_echo//\(/\$}  # Use $ to indicate arg
                default_echo="${default_echo//)/}"    # No end parentheses
                echo 'echo "'"${default_echo}"'"'
            fi
        } > "$tmpf"
        # shellcheck disable=SC2086
        "$shc" $opt -f "$tmpf" -o "$tmpa"
        # ls -la "$tmpa"

        if [ "$opt" = "-D" ] ; then
            # Hide debug output
            out=$("$tmpa" "$firstarg" "$secondarg" 2>/dev/null)
            # TODO: compare dbg output
            # outdbg=$("$tmpa" first second 2>1)
        else
            out=$("$tmpa" "$firstarg" "$secondarg" 2>&1)
        fi
        if [[ "$out" = "$expected" ]]; then
            echo    "===================================================="
            echo -e "=== $shell [with shc $opt]: ${txtgrn}PASSED${txtrst}"
            echo    "===================================================="
            ((pc++))
            rm -rf "$tmpd"
        else
            echo    "===================================================="
            echo -e "=== $shell [with shc $opt]: ${txtred}FAILED${txtrst}"
            echo    "===================================================="
            echo "  Files kept in '$tmpd'"
            printf "*** Expected Output:\n%s\n" "$expected"
            printf "*** Output:\n%s\n*** End of output\n" "$out"
            echo "$out" > "$tmpl"
            stat=1
            ((fc++))
        fi
    done
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

if ((stat>0)); then
    echo "EXIT with code $stat"
fi
exit $stat
