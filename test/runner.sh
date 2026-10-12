#!/bin/bash
# Test runner behavior without requiring optional interpreters or a C compiler.
set -eu
runner=$(cd "$(dirname "$0")" && pwd)/ttest.sh
tmpd=$(mktemp -d)
trap 'rm -rf "$tmpd"' EXIT
unset SHC_TEST_SHELLS
mkdir "$tmpd/bin" "$tmpd/work"
for tool in mktemp rm; do
    ln -s "$(command -v "$tool")" "$tmpd/bin/$tool"
done
ln -s /bin/sh "$tmpd/bin/sh"
ln -s /bin/sh "$tmpd/bin/tcsh"
export SHC_RUNNER_CHMOD=$(command -v chmod)
export SHC_RUNNER_EXEC_LOG="$tmpd/executed"
cat >"$tmpd/compiler" <<'COMPILER'
#!/bin/bash
while (($#)); do
    case "$1" in
        -o) output=$2; shift ;;
    esac
    shift
done
case ${SHC_RUNNER_MODE:-success} in
    no_binary) exit 0 ;;
    execution_failure) status=7 ;;
    *) status=0 ;;
esac
if [[ ${SHC_RUNNER_MODE:-success} = wrong_output ]]; then
    message=wrong
else
    message='Hello World fp:first sp:second'
fi
printf '#!/bin/sh\necho executed >>"$SHC_RUNNER_EXEC_LOG"\necho "%s"\nexit %d\n' "$message" "$status" >"$output"
"$SHC_RUNNER_CHMOD" +x "$output"
[[ ${SHC_RUNNER_MODE:-success} != compilation_failure ]]
COMPILER
chmod +x "$tmpd/compiler"
checks=0

fail() {
    echo "FAIL: $*" >&2
    cat "$tmpd/log" >&2
    exit 1
}

run() {
    local expected=$1 status=0
    shift
    rm -f "$SHC_RUNNER_EXEC_LOG"
    env PATH="$tmpd/bin" TMPDIR="$tmpd/work" "$@" /bin/bash "$runner" "$tmpd/compiler" >"$tmpd/log" 2>&1 || status=$?
    [ "$status" -eq "$expected" ] || fail "status $status, expected $expected"
    # The runner must clean up test scripts and generated executables.
    [ -z "$(ls -A "$tmpd/work")" ] || fail 'temporary files left behind'
}

contains() {
    grep -F -- "$1" "$tmpd/log" >/dev/null || fail "missing output: $1"
}

pass() {
    checks=$((checks + 1))
    echo "PASS: runner $*"
}

run 0
contains "PASSED: $tmpd/bin/tcsh"
contains 'PASSED: 10'
contains 'SKIPPED: 35'
pass 'PATH discovery and optional skips'

run 0 SHC_TEST_SHELLS='sh tcsh'
contains 'PASSED: 10'
contains 'SKIPPED: 0'
pass 'required shell names'

run 0 SHC_TEST_SHELLS="$tmpd/bin/tcsh"
contains 'PASSED: 5'
pass 'explicit interpreter path'

(cd "$tmpd"; run 0 SHC_TEST_SHELLS='bin/tcsh')
contains "PASSED: $tmpd/bin/tcsh"
pass 'relative interpreter path'

run 1 SHC_TEST_SHELLS='sh missing-shell'
contains 'FAILED: required shell unavailable: missing-shell'
contains 'PASSED: 5'
contains 'FAILED: 5'
pass 'missing required shell fails'

run 1 SHC_TEST_SHELLS=''
contains 'SHC_TEST_SHELLS must contain at least one shell'
pass 'empty required list fails'

for mode in compilation_failure execution_failure wrong_output no_binary; do
    run 1 SHC_TEST_SHELLS=sh SHC_RUNNER_MODE="$mode"
    contains 'PASSED: 0'
    contains 'FAILED: 5'
    if [[ $mode = compilation_failure ]]; then
        [ ! -e "$SHC_RUNNER_EXEC_LOG" ] || fail 'executed output after failed compilation'
    fi
    pass "$mode fails"
done

rm "$tmpd/bin/sh" "$tmpd/bin/tcsh"
run 1
contains 'no shells available for testing'
contains 'SKIPPED: 45'
[ ! -e "$SHC_RUNNER_EXEC_LOG" ] || fail 'ran a test without an interpreter'
pass 'all-skipped run fails'

echo "Test runner checks: $checks passed"
