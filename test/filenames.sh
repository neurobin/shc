#!/bin/bash
# Filename regressions. Run directly with a built shc, or via make check.
set -eu

shc=${1:-shc}
case "$shc" in
    */*) shc="$(cd "$(dirname "$shc")" && pwd)/$(basename "$shc")" ;;
    *) shc=$(command -v "$shc") ;;
esac
tmpd=$(mktemp -d)
trap 'rm -rf "$tmpd"' EXIT
trap 'exit 1' HUP INT TERM
cd "$tmpd"

names=('plain' 'with space' "single'quote" 'double"quote' 'dollar$HOME'
       '$(touch command-ran)' '`touch backtick-ran`' 'semi;touch semicolon-ran;#'
       'glob*[ab]?' 'back\slash' '-leading' 'bang!name' $'tab\tname'
       $'line\n#endif\n#error filename-was-code\n#if 0\nname')
printf '<%s>\n' 'first with space' "second'quote" > expected
count=0

check_binary() {
    local binary=$1
    if ! "$binary" 'first with space' "second'quote" > actual 2> runtime.err; then
        printf 'Execution failed: %s\n' "$binary" >&2
        cat actual runtime.err >&2
        exit 1
    fi
    if ! cmp -s expected actual || [ -s runtime.err ]; then
        printf 'Filename regression: %s\n' "$binary" >&2
        cat actual runtime.err >&2
        exit 1
    fi
    count=$((count + 1))
}

# Exercise lsto="--" re-execution even where ash and tsh are not installed.
mkdir aliases
ln -s /bin/sh aliases/ash
ln -s /bin/sh aliases/tsh
for shell in /bin/sh /bin/dash /bin/bash /bin/ash /bin/ksh /bin/zsh \
             /bin/csh /bin/tcsh /usr/bin/rc /usr/bin/perl \
             "$tmpd/aliases/ash" "$tmpd/aliases/tsh"; do
    [ -x "$shell" ] || continue
    printf '#!%s\n' "$shell" > script
    case "$shell" in
        *csh) printf '%s\n' "printf '<%s>\\n' \$argv:q" >> script ;;
        */rc) printf '%s\n' "printf '<%s>\\n' \$*" >> script ;;
        */perl) printf '%s\n' 'print "<$_>\n" for @ARGV;' >> script ;;
        *) printf '%s\n' 'printf "<%s>\n" "$@"' >> script ;;
    esac
    shell_names=("${names[@]}")
    if [ "$shell" != /bin/sh ]; then
        # Compilation quoting is shared; exercise interpreter-specific exec
        # quoting with combined metacharacters and a multiline filename.
        shell_names=("with space and 'quote' \$literal!name" "${names[13]}")
    fi
    for name in "${shell_names[@]}"; do
        # Default output exercises the input path through compilation and exec.
        cp script "./$name.sh"
        "$shc" -f "$name.sh" > compile.log 2>&1 || { cat compile.log; exit 1; }
        check_binary "./$name.sh.x"
        # A plain input isolates handling of the explicit output path.
        "$shc" -f script -o "$name.out" > compile.log 2>&1 || { cat compile.log; exit 1; }
        check_binary "./$name.out"
        # chmod must grant group write permission, as before this fix.
        [ "$(LC_ALL=C ls -ld "./$name.out" | head -n 1 | cut -c1-10)" = '-rwxrwxr-x' ]
    done
    printf 'Filename checks passed for %s\n' "$shell"
done
[ ! -e command-ran ] && [ ! -e backtick-ran ] && [ ! -e semicolon-ran ]

# Keep tool commands and flags shell-parsed, while passing filenames literally.
real_cc=$(command -v cc)
real_strip=$(command -v strip)
export real_cc real_strip
cat > 'cc wrapper' <<'EOF'
#!/bin/sh
[ "$1" = '--cc-check' ] && [ "$2" = '-DNAME=two words' ] &&
    [ "$3" = '-Ldirectory with space' ] || exit 91
shift
printf 'cc\n' >> tools-called
exec "$real_cc" "$@"
EOF
cat > 'strip wrapper' <<'EOF'
#!/bin/sh
[ "$1" = '--strip-check' ] && [ "$2" = "./-output ' with spaces" ] &&
    [ "$#" = 2 ] || exit 92
shift
printf 'strip\n' >> tools-called
exec "$real_strip" "$@"
EOF
chmod +x 'cc wrapper' 'strip wrapper'
mkdir 'directory with space'
printf '%s\n' '#!/bin/sh' 'printf "<%s>\n" "$@"' > script
CC="'./cc wrapper' --cc-check" CFLAGS='-DNAME="two words"' \
LDFLAGS='-L"directory with space"' STRIP="'./strip wrapper' --strip-check" \
    "$shc" -f script -o "-output ' with spaces"
check_binary "./-output ' with spaces"
printf 'cc\nstrip\n' > expected-tools
cmp expected-tools tools-called

# Escaping can expand a valid path beyond the old 4096-byte command buffer.
longdir=.
for i in 1 2 3 4 5 6 7 8; do
    component="$(printf '%010d' "$i")$(printf '%090d' 0 | tr 0 "'")"
    longdir="$longdir/$component"
    mkdir "$longdir"
done
cp script "$longdir/input"
"$shc" -f "$longdir/input" -o "$longdir/output"
check_binary "$longdir/output"

printf 'Filename regressions: %d passed\n' "$count"
