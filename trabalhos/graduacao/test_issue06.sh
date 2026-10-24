#!/bin/sh
# Issue #6: invariantes, 1000 atualizações, espaço linear e consultas sem
# alocação; também confere `make run` com caminhos variados e via make -C.
set -eu

cd "$(dirname "$0")"
make build

test_dir=$(mktemp -d ./.issue06.XXXXXX)
trap 'rm -rf "$test_dir"' 0

"${CXX:-c++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic \
    persistent_list.cpp test_issue06.cpp -o "$test_dir/structural_test"
"$test_dir/structural_test"

# A regressão da #5 precisa continuar passando.
sh test_regression.sh > /dev/null

expected=../../testes/teste05.out
check_run() {
    "$@" > "$test_dir/obtido.out" 2> "$test_dir/erro.out"
    if ! cmp -s "$expected" "$test_dir/obtido.out"; then
        echo "falha: $*" >&2
        diff -u "$expected" "$test_dir/obtido.out" >&2
        exit 1
    fi
    test ! -s "$test_dir/erro.out"
}

cp ../../testes/teste05.in "$test_dir/entrada_sem_extensao"
check_run make run INPUT=../../testes/teste05.in
check_run make run INPUT="$(pwd)/../../testes/teste05.in"
check_run make run INPUT="$test_dir/entrada_sem_extensao"
# Chamado da raiz do repositório: nenhum "Entering directory" no stdout.
check_run sh -c 'cd ../.. &&
    make -C trabalhos/graduacao run INPUT=../../testes/teste05.in'
