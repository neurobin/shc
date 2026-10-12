#!/bin/bash
# Verify warnings without relying on the host's ARG_MAX or running a compiler
# on an oversized script. The driver calls the production read_script().
set -eu
testdir=$(cd "$(dirname "$0")" && pwd)
tmpd=$(mktemp -d)
trap 'rm -rf "$tmpd"' EXIT

# Compiler commands and flags intentionally use shell word splitting, as make
# does, so CC="ccache cc" and multiple flags work without eval.
${CC:-cc} ${CPPFLAGS:-} ${CFLAGS:-} "$testdir/arg-max.c" \
    ${LDFLAGS:-} -o "$tmpd/arg-max"
printf '%0100d' 0 >"$tmpd/script"
checks=0
for mode in large reported unbounded error zero below boundary above; do
    "$tmpd/arg-max" "$mode" "$tmpd/script" >"$tmpd/stdout" 2>"$tmpd/stderr"
    cmp "$tmpd/script" "$tmpd/stdout"
    if [ "$mode" = above ]; then
        grep -F 'WARNING!!' "$tmpd/stderr" >/dev/null
        grep -F 'returns 124 bytes' "$tmpd/stderr" >/dev/null
        grep -F 'is 100 bytes length' "$tmpd/stderr" >/dev/null
    elif [ -s "$tmpd/stderr" ]; then
        echo "FAIL: unexpected warning for $mode limit" >&2
        cat "$tmpd/stderr" >&2
        exit 1
    fi
    checks=$((checks + 1))
    echo "PASS: $mode limit"
done
echo "ARG_MAX regressions: $checks passed"
