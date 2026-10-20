#!/bin/sh
set -eu

cd "$(dirname "$0")"
make build

test_dir=$(mktemp -d ./.issue02.XXXXXX)
trap 'rm -rf "$test_dir"' 0

"${CXX:-c++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic \
    persistent_list.cpp test_issue02.cpp -o "$test_dir/structural_test"
"$test_dir/structural_test"

for number in 01 02; do
    make run INPUT="../../testes/teste${number}.in" \
        > "$test_dir/obtido.out" 2> "$test_dir/erro.out"
    if ! cmp -s "../../testes/teste${number}.out" "$test_dir/obtido.out"; then
        diff -u "../../testes/teste${number}.out" "$test_dir/obtido.out"
        exit 1
    fi
    test ! -s "$test_dir/erro.out"
done

cat > "$test_dir/entrada.in" <<'EOF'
IMP 0
INC 0
INC 65535
INC 32767
INC 0
IMP 0
IMP 1
IMP 2
IMP 3
IMP 4
IMP 99
IMP -1
INC 9223372036854775807
IMP 5
EOF
cat > "$test_dir/esperado.out" <<'EOF'
IMP 0

IMP 0

IMP 1
0
IMP 2
0, 65535
IMP 3
0, 32767, 65535
IMP 4
0, 0, 32767, 65535
IMP 99
0, 0, 32767, 65535
IMP -1
0, 0, 32767, 65535
IMP 5
0, 0, 32767, 65535, 9223372036854775807
EOF
make run INPUT="$test_dir/entrada.in" \
    > "$test_dir/obtido.out" 2> "$test_dir/erro.out"
if ! cmp -s "$test_dir/esperado.out" "$test_dir/obtido.out"; then
    diff -u "$test_dir/esperado.out" "$test_dir/obtido.out"
    exit 1
fi
test ! -s "$test_dir/erro.out"

printf 'INC -1\n' > "$test_dir/entrada-invalida.in"
if make run INPUT="$test_dir/entrada-invalida.in" \
    > "$test_dir/obtido.out" 2> "$test_dir/erro.out"; then
    echo 'INC negativo deveria falhar' >&2
    exit 1
fi
test ! -s "$test_dir/obtido.out"
test -s "$test_dir/erro.out"

echo 'Issue #2: saída exata verificada'
