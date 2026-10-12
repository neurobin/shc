#!/bin/bash
# Exercise bash-static recognition even on hosts without a static Bash package.
set -eu

shc=${1:-shc}
case "$shc" in
    /*) ;;
    */*) shc="$PWD/$shc" ;;
    *) shc=$(command -v "$shc") ;;
esac
static_bash=$(command -v bash-static || true)
bash_path=$(command -v bash)
tmpd=$(mktemp -d)
trap 'rm -rf "$tmpd"' EXIT
cd "$tmpd"
ln -s "$bash_path" bash-static

check_shell() {
    local shell=$1 label=$2 opt output expected
    printf '#!%s\n' "$shell" >script
    cat >>script <<'EOF'
# Bash arrays and printf %q exercise Bash syntax and argument boundaries.
args=("$@")
printf 'argc=%s\n' "${#args[@]}"
printf '<%q>\n' "${args[@]}"
EOF
    expected=$("$shell" script 'two words' '' '*' "single'quote" '-n')
    for opt in '' '-r'; do
        "$shc" ${opt:+"$opt"} -f script -o compiled
        output=$(./compiled 'two words' '' '*' "single'quote" '-n')
        if [ "$output" != "$expected" ]; then
            echo "FAIL: $label ${opt:-default}: arguments changed" >&2
            exit 1
        fi
        # A compiled script must still depend on the original interpreter path.
        if [ "$label" = alias ]; then
            mv bash-static saved-shell
            if ./compiled >output 2>error; then
                echo 'FAIL: compiled script used a different interpreter' >&2
                exit 1
            fi
            grep -F -- "$shell" error >/dev/null
            mv saved-shell bash-static
            [ "$(./compiled 'two words' '' '*' "single'quote" '-n')" = "$expected" ]
        fi
        echo "PASS: bash-static $label ${opt:-default}"
    done
}

check_shell "$tmpd/bash-static" alias
if [ -n "$static_bash" ]; then
    check_shell "$static_bash" installed
else
    echo 'SKIP: installed bash-static (not available)'
fi
