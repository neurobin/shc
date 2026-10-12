#!/bin/bash
# Keep generated C beside the selected executable without modifying the input.
set -eu
shc=${1:-shc}
case "$shc" in
    /*) ;;
    */*) shc="$PWD/$shc" ;;
    *) shc=$(command -v "$shc") ;;
esac
tmpd=$(mktemp -d)
trap 'chmod u+w "$tmpd/source" 2>/dev/null || :; rm -rf "$tmpd"' EXIT
cd "$tmpd"
mkdir source build
printf '#!/bin/sh\nprintf "output-path-ok\\n"\n' >original
cp original source/script.sh
checks=0

pass() {
    checks=$((checks + 1))
    echo "PASS: $*"
}

check_binary() {
    [ -s "$1.c" ]
    [ "$("$1")" = output-path-ok ]
}

reject() {
    if "$shc" "$@" >stdout 2>stderr; then
        echo "FAIL: expected rejection: $*" >&2
        exit 1
    fi
}

cp original default.sh
"$shc" -f default.sh
check_binary ./default.sh.x
pass 'default naming unchanged'

"$shc" -f source/script.sh -o build/tool
check_binary ./build/tool
[ ! -e source/script.sh.x.c ]
pass 'explicit output puts C in the build directory'

printf 'existing generated source\n' >source/script.sh.x.c
cp source/script.sh.x.c sentinel
"$shc" -f source/script.sh -o build/rebuild
check_binary ./build/rebuild
cmp sentinel source/script.sh.x.c
pass 'explicit output leaves the old C path untouched'

"$shc" -f "$tmpd/source/script.sh" -o "$tmpd/build/absolute"
check_binary "$tmpd/build/absolute"
pass 'absolute output path'

"$shc" -f source/script.sh -o build/explicit.x
check_binary ./build/explicit.x
pass 'explicit .x output suffix'

"$shc" -f source/script.sh -o build/explicit.c
check_binary ./build/explicit.c
pass 'explicit .c output suffix'

chmod a-w source
"$shc" -f source/script.sh -o build/read-only-source
check_binary ./build/read-only-source
cmp original source/script.sh
chmod u+w source
pass 'source directory need not be writable'

reject -f source/script.sh -o missing/tool
grep -F 'creating output file: missing/tool.c' stderr >/dev/null
cmp sentinel source/script.sh.x.c
pass 'missing output directory fails at the selected C path'

cp original collision.c
reject -f collision.c -o collision
grep -F 'refusing to overwrite source script' stderr >/dev/null
cmp original collision.c
[ ! -e collision ]
pass 'C output cannot overwrite the source'

ln -s "$tmpd/collision.c" build/symlink.c
reject -f collision.c -o build/symlink
grep -F 'refusing to overwrite source script' stderr >/dev/null
cmp original collision.c
[ ! -e build/symlink ]
pass 'C output symlink cannot overwrite the source'

ln collision.c build/hardlink.c
reject -f collision.c -o build/hardlink
grep -F 'refusing to overwrite source script' stderr >/dev/null
cmp original collision.c
[ ! -e build/hardlink ]
pass 'C output hard link cannot overwrite the source'

# Repeating the output directory in the compiler command can exceed 4096 bytes
# even when each path and the old input-based compiler command fit that limit.
longdir=build
for i in 1 2 3 4 5 6 7 8; do
    component="$(printf '%080d' "$i")"
    longdir="$longdir/$component"
    mkdir "$longdir"
done
CFLAGS="-DSHC_OUTPUT_PADDING=$(printf '%03000d' 0 | tr 0 x)" \
    "$shc" -f source/script.sh -o "$longdir/tool"
[ -s "$longdir/tool.c" ]
# A short hard link avoids the separate pre-existing runtime path-length limit.
ln "$longdir/tool" short-output
[ "$(./short-output)" = output-path-ok ]
pass 'compiler command accommodates both long output paths'

cmp original source/script.sh
cmp sentinel source/script.sh.x.c
echo "Output-path regressions: $checks passed"
