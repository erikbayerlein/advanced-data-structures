#!/bin/sh
set -eu

cd "$(dirname "$0")"
make build

test_dir=$(mktemp -d ./.issue01.XXXXXX)
trap 'rm -rf "$test_dir"' 0

printf 'IMP 0\nIMP 99\n' > "$test_dir/entrada.in"
printf 'IMP 0\n\nIMP 99\n\n' > "$test_dir/esperado.out"
make run INPUT="$test_dir/entrada.in" > "$test_dir/obtido.out" 2> "$test_dir/erro.out"
cmp -s "$test_dir/esperado.out" "$test_dir/obtido.out" || {
    diff -u "$test_dir/esperado.out" "$test_dir/obtido.out"
    exit 1
}
test ! -s "$test_dir/erro.out"

cp "$test_dir/entrada.in" "$test_dir/entrada.txt"
make run INPUT="$test_dir/entrada.txt" > "$test_dir/obtido.out" 2> "$test_dir/erro.out"
cmp -s "$test_dir/esperado.out" "$test_dir/obtido.out"
test ! -s "$test_dir/erro.out"

if make run INPUT="$test_dir/inexistente.in" > "$test_dir/obtido.out" 2> "$test_dir/erro.out"; then
    echo 'arquivo ausente deveria falhar' >&2
    exit 1
fi
test ! -s "$test_dir/obtido.out"
test -s "$test_dir/erro.out"

printf 'IMP abc\n' > "$test_dir/invalido.in"
if make run INPUT="$test_dir/invalido.in" > "$test_dir/obtido.out" 2> "$test_dir/erro.out"; then
    echo 'comando malformado deveria falhar' >&2
    exit 1
fi
test ! -s "$test_dir/obtido.out"
test -s "$test_dir/erro.out"

echo 'Issue #1: verificação concluída'
