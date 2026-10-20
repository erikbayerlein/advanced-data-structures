#!/bin/sh
set -eu

cd "$(dirname "$0")"
make build

test_dir=$(mktemp -d ./.issue03.XXXXXX)
trap 'rm -rf "$test_dir"' 0

"${CXX:-c++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic \
    persistent_list.cpp test_issue03.cpp -o "$test_dir/structural_test"
"$test_dir/structural_test"

for number in 04 05; do
    python3 - "$number" "$test_dir" <<'PY'
from pathlib import Path
import sys

number, directory = sys.argv[1:]
source = Path('../../testes')
target = Path(directory)
input_lines = (source / f'teste{number}.in').read_bytes().splitlines(keepends=True)
output_lines = (source / f'teste{number}.out').read_bytes().splitlines(keepends=True)
assert len(output_lines) % 2 == 0
queries = list(zip(output_lines[::2], output_lines[1::2]))
assert all(command.startswith((b'IMP ', b'SUC ')) for command, _ in queries)
assert [command for command, _ in queries] == [
    line for line in input_lines if line.startswith((b'IMP ', b'SUC '))]
expected_imp_count = sum(line.startswith(b'IMP ') for line in input_lines)
assert expected_imp_count == sum(command.startswith(b'IMP ') for command, _ in queries)
(target / 'entrada.in').write_bytes(
    b''.join(line for line in input_lines if not line.startswith(b'SUC ')))
(target / 'esperado.out').write_bytes(
    b''.join(command + answer for command, answer in queries
             if command.startswith(b'IMP ')))
PY
    make run INPUT="$test_dir/entrada.in" \
        > "$test_dir/obtido.out" 2> "$test_dir/erro.out"
    if ! cmp -s "$test_dir/esperado.out" "$test_dir/obtido.out"; then
        diff -u "$test_dir/esperado.out" "$test_dir/obtido.out"
        exit 1
    fi
    test ! -s "$test_dir/erro.out"
done

cat > "$test_dir/entrada.in" <<'EOF'
REM 5
IMP 1
INC 2
INC 2
REM 2
IMP 3
IMP 4
REM 2
REM -1
IMP 5
IMP 6
EOF
cat > "$test_dir/esperado.out" <<'EOF'
IMP 1

IMP 3
2, 2
IMP 4
2
IMP 5

IMP 6

EOF
make run INPUT="$test_dir/entrada.in" \
    > "$test_dir/obtido.out" 2> "$test_dir/erro.out"
if ! cmp -s "$test_dir/esperado.out" "$test_dir/obtido.out"; then
    diff -u "$test_dir/esperado.out" "$test_dir/obtido.out"
    exit 1
fi
test ! -s "$test_dir/erro.out"

echo 'Issue #3: consultas IMP dos testes 04 e 05 verificadas'
