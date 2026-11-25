#!/bin/sh
set -eu

cd "$(dirname "$0")"
test_dir=$(mktemp -d ./.test-programa.XXXXXX)
trap 'rm -rf "$test_dir"' 0

check_output() {
    if ! make --no-print-directory run INPUT="$1" > "$test_dir/obtido" 2> "$test_dir/erro"; then
        cat "$test_dir/erro" >&2
        return 1
    fi
    diff -u "$2" "$test_dir/obtido"
    test ! -s "$test_dir/erro"
}

check_error() {
    if ./programa "$1" > "$test_dir/obtido" 2> "$test_dir/erro"; then
        printf 'A entrada %s deveria falhar\n' "$1" >&2
        return 1
    fi
    test ! -s "$test_dir/obtido"
    grep -F "$2" "$test_dir/erro" > /dev/null
}

printf 'IMP\r\nIMP\r\n' > "$test_dir/entrada vazia.in"
printf 'IMP\n\nIMP\n\n' > "$test_dir/esperado"
check_output "$test_dir/entrada vazia.in" "$test_dir/esperado"

printf '  IMP \t\n' > "$test_dir/eco.in"
printf '  IMP \t\n\n' > "$test_dir/esperado"
check_output "$test_dir/eco.in" "$test_dir/esperado"

: > "$test_dir/vazio.in"
check_output "$test_dir/vazio.in" "$test_dir/vazio.in"
check_error "$test_dir/ausente.in" 'abrir o arquivo de entrada'

for command in 'INC -1' 'INC 4294967296' 'INC 20x' 'INC 1 2' 'INC' 'IMP 1' 'XYZ 0' ''; do
    printf '%s\n' "$command" > "$test_dir/invalido.in"
    check_error "$test_dir/invalido.in" 'comando inválido'
done

for command in 'REM 0' 'SUC 4294967295' 'PRE 0'; do
    printf '%s\n' "$command" > "$test_dir/indisponivel.in"
    check_error "$test_dir/indisponivel.in" 'operação ainda indisponível'
done

for case_name in teste01 teste02; do
    check_output "../../testes/$case_name.in" "../../testes/$case_name.out"
done

printf 'INC 4294967295\nINC 2147483648\nINC 65536\nINC 65535\nINC 3\nINC 2\nINC 1\nINC 0\nIMP' > "$test_dir/decrescente.in"
printf 'IMP\n0, 1, 2, 3, 65535, 65536, 2147483648, 4294967295\n' > "$test_dir/esperado"
check_output "$PWD/$test_dir/decrescente.in" "$test_dir/esperado"

printf 'INC 9\nINC 9\nIMP\nINC 1\nINC 9\nIMP\n' > "$test_dir/multiplicidades.in"
printf 'IMP\n9, 9\nIMP\n1, 9, 9, 9\n' > "$test_dir/esperado"
check_output "$test_dir/multiplicidades.in" "$test_dir/esperado"

printf 'INC 4294967295\nINC 0\nINC 4294967295\nINC 65536\nINC 65535\nINC 0\nIMP\n' > "$test_dir/extremos-repetidos.in"
printf 'IMP\n0, 0, 65535, 65536, 4294967295, 4294967295\n' > "$test_dir/esperado"
check_output "$test_dir/extremos-repetidos.in" "$test_dir/esperado"

printf 'Interface de linha de comando verificada\n'
