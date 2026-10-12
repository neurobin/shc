#!/bin/bash
# Error diagnostics must describe the failure without a trailing strerror(0).
set -eu
export LC_ALL=C
unset CC CFLAGS LDFLAGS STRIP

shc=${1:-shc}
case "$shc" in
    /*) ;;
    */*) shc="$PWD/$shc" ;;
    *) shc=$(command -v "$shc") ;;
esac
tmpd=$(mktemp -d)
trap 'rm -rf "$tmpd"' EXIT
cd "$tmpd"
checks=0

fail() {
    echo "FAIL: $*" >&2
    cat stderr >&2
    exit 1
}

run() {
    local expected=$1
    shift
    local status=0
    "$@" >stdout 2>stderr || status=$?
    [ "$status" -eq "$expected" ] || fail "exit status $status, expected $expected: $*"
    if grep -E 'Success|Undefined error: 0' stderr >/dev/null; then
        fail 'misleading system error'
    fi
}

contains() {
    grep -F -- "$1" stderr >/dev/null || fail "missing diagnostic: $1"
}

one_line() {
    [ "$(wc -l <stderr)" -eq 1 ] || fail 'expected exactly one diagnostic'
}

pass() {
    checks=$((checks + 1))
    echo "PASS: $*"
}

printf 'echo missing-shebang\n' >invalid
run 1 "$shc" -f invalid
contains 'invalid first line in script: echo missing-shebang'
one_line
pass 'invalid shebang'

printf '#!sh\necho invalid-path\n' >invalid
run 1 "$shc" -f invalid
contains 'invalid shell path: sh'
one_line
pass 'invalid shell path'

printf '#!/no/such/unsupported-shell\necho unsupported\n' >invalid
run 1 "$shc" -f invalid
contains 'Unknown shell (unsupported-shell)'
one_line
pass 'unsupported shell'

run 1 "$shc" -f missing
contains 'opening script: missing: No such file or directory'
one_line
pass 'missing input'

mkdir directory
run 1 "$shc" -f directory
contains 'reading script: directory: Is a directory'
one_line
pass 'input read failure'

printf '#!%s/missing/sh\necho missing-shell\n' "$tmpd" >invalid
run 1 "$shc" -f invalid
contains "examining shell: $tmpd/missing/sh: No such file or directory"
one_line
pass 'missing shell'

printf '#!/bin/sh\nprintf "ok\\n"\n' >valid
mkdir valid.x.c
run 1 "$shc" -f valid
contains 'creating output file: valid.x.c: Is a directory'
one_line
rmdir valid.x.c
pass 'output open failure'

# Ignoring SIGXFSZ makes a file-size limit produce an I/O error, not a signal.
# Verify that compilation is never attempted after an output write failure.
run 1 bash -c 'trap "" XFSZ; ulimit -f 1; CC="touch compiler-ran; false" exec "$1" -f valid' _ "$shc"
contains 'writing output file: valid.x.c: File too large'
one_line
[ ! -e compiler-ran ] || fail 'compiler ran after failed output write'
pass 'output write failure'

run 1 env CC='echo compiler-diagnostic >&2; exit 42;' "$shc" -f valid
contains 'compiler-diagnostic'
contains 'command exited with status 42:'
contains 'exit 42;'
[ "$(wc -l <stderr)" -eq 2 ] || fail 'unexpected compiler diagnostics'
pass 'compiler failure preserves child stderr and exit status'

run 1 env CC="$tmpd/missing-compiler" "$shc" -f valid
contains 'command exited with status 127:'
contains "$tmpd/missing-compiler"
pass 'compiler not found'

run 1 env CC='kill -TERM $$;' "$shc" -f valid
contains 'command terminated by signal 15:'
one_line
pass 'compiler command killed by signal'

run 0 env STRIP='exit 23;' "$shc" -f valid
contains 'command exited with status 23:'
contains 'warning: could not strip output: valid.x'
[ "$(./valid.x)" = ok ] || fail 'strip warning broke executable'
pass 'strip failure remains a warning'

run 0 env CC=true STRIP=true "$shc" -f valid -o missing/output
contains 'command exited with status 1: chmod'
contains 'warning: could not set output permissions: missing/output'
pass 'chmod failure remains a warning'

run 0 "$shc" -f valid
[ ! -s stderr ] || fail 'unexpected successful-build diagnostic'
[ "$(./valid.x)" = ok ] || fail 'successful build did not run'
pass 'successful compilation and execution'

echo "Error-reporting tests: $checks passed"
