#!/bin/bash

# Default shells are optional; an explicit list makes every entry required.
# Disable pathname expansion when splitting the space-separated override.
set -f
shells=(sh dash bash ash ksh zsh tcsh csh rc)
required=0
if [[ ${SHC_TEST_SHELLS+x} ]]; then
    shells=($SHC_TEST_SHELLS)
    required=1
    if ((${#shells[@]} == 0)); then
        echo 'FAILED: SHC_TEST_SHELLS must contain at least one shell' >&2
        exit 1
    fi
fi
check_opts=('' '-r' '-v' '-D' '-S')
shc=${1-shc}

stat=0
pc=0
fc=0
sc=0
available=0
case_id=0
tmpd=$(mktemp -d) || exit 1
trap 'rm -rf "$tmpd"' EXIT
trap 'exit 1' HUP INT TERM

printf '\n== Running tests ...\n'
for name in "${shells[@]}"; do
    case "$name" in
        */*) shell=$name ;;
        *) shell=$(type -P -- "$name") ;;
    esac
    if [[ ! -f "$shell" || ! -x "$shell" ]]; then
        if ((required)); then
            printf 'FAILED: required shell unavailable: %s\n' "$name"
            fc=$((fc + ${#check_opts[@]}))
            stat=1
        else
            printf 'SKIPPED: shell unavailable: %s (%d cases)\n' "$name" "${#check_opts[@]}"
            sc=$((sc + ${#check_opts[@]}))
        fi
        continue
    fi
    # A shebang needs a path that remains valid when the binary reexecutes.
    case "$shell" in
        /*) ;;
        *) shell="$PWD/$shell" ;;
    esac
    available=$((available + 1))
    for opt in "${check_opts[@]}"; do
        case_id=$((case_id + 1))
        tmpf="$tmpd/test.$case_id"
        binary="$tmpf.x"
        if ! printf '#!%s\n%s\n' "$shell" "echo 'Hello World fp:'\$1 sp:\$2" >"$tmpf"; then
            echo 'FAILED: could not create test script' >&2
            exit 1
        fi
        reason=''
        if ! "$shc" ${opt:+"$opt"} -f "$tmpf" -o "$binary"; then
            reason='compilation failed'
        elif ! out=$("$binary" first second); then
            reason='execution failed'
        elif [[ "$out" != 'Hello World fp:first sp:second' ]]; then
            reason='unexpected output'
        fi
        if [[ -z "$reason" ]]; then
            printf 'PASSED: %s [shc %s]\n' "$shell" "$opt"
            pc=$((pc + 1))
        else
            printf 'FAILED: %s [shc %s]: %s\n' "$shell" "$opt" "$reason"
            stat=1
            fc=$((fc + 1))
        fi
    done
done

if ((available == 0)); then
    echo 'FAILED: no shells available for testing' >&2
    stat=1
fi
printf '\nTest Summary\n------------\nPASSED: %d\nFAILED: %d\nSKIPPED: %d\n' "$pc" "$fc" "$sc"
exit "$stat"
