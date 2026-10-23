#!/bin/sh
set -eu

cd "$(dirname "$0")"
make build

test_dir=$(mktemp -d ./.issue04.XXXXXX)
trap 'rm -rf "$test_dir"' 0

run_case() {
    make run INPUT="$test_dir/entrada.in" \
        > "$test_dir/obtido.out" 2> "$test_dir/erro.out"
    if ! cmp -s "$test_dir/esperado.out" "$test_dir/obtido.out"; then
        echo "falha no caso: $1" >&2
        diff -u "$test_dir/esperado.out" "$test_dir/obtido.out" >&2
        exit 1
    fi
    test ! -s "$test_dir/erro.out"
}

# Duplicatas: N nunca e aceito como sucessor; N ausente tambem funciona.
cat > "$test_dir/entrada.in" <<'IN'
INC 5
INC 2
INC 9
INC 5
SUC 1 4
SUC 2 4
SUC 5 4
SUC 9 4
SUC 4 4
SUC 0 0
SUC 0 3
IN
cat > "$test_dir/esperado.out" <<'OUT'
SUC 1 4
2
SUC 2 4
5
SUC 5 4
9
SUC 9 4
inf
SUC 4 4
5
SUC 0 0
inf
SUC 0 3
2
OUT
run_case "duplicatas e limiares"

# Fallback para a ultima versao (M invalido), preservando o M original no eco;
# consultas nao criam versao; historico preservado apos INC/REM.
cat > "$test_dir/entrada.in" <<'IN'
INC 2
INC 5
INC 5
INC 9
SUC 0 99
INC 7
SUC 0 99
SUC 7 99
SUC 0 -1
SUC 0 5
REM 5
SUC 2 5
SUC 2 6
SUC 2 7
SUC 5 6
IMP 99
IN
cat > "$test_dir/esperado.out" <<'OUT'
SUC 0 99
2
SUC 0 99
2
SUC 7 99
9
SUC 0 -1
2
SUC 0 5
2
SUC 2 5
5
SUC 2 6
5
SUC 2 7
5
SUC 5 6
7
IMP 99
2, 5, 7, 9
OUT
run_case "fallback, historico e consultas sem versao"

# Versao vazia, lista vazia e valores extremos de 64 bits (sem calcular N+1).
cat > "$test_dir/entrada.in" <<'IN'
SUC 0 0
SUC 9223372036854775807 0
INC 9223372036854775807
SUC 0 0
SUC 0 1
SUC 9223372036854775807 1
SUC -5 1
IN
cat > "$test_dir/esperado.out" <<'OUT'
SUC 0 0
inf
SUC 9223372036854775807 0
inf
SUC 0 0
inf
SUC 0 1
9223372036854775807
SUC 9223372036854775807 1
inf
SUC -5 1
9223372036854775807
OUT
run_case "versao vazia e limites"

# Consultas intercaladas nao criam versoes.
cat > "$test_dir/entrada.in" <<'IN'
INC 1
SUC 0 1
SUC 0 1
IMP 2
INC 3
IMP 2
IMP 3
IN
cat > "$test_dir/esperado.out" <<'OUT'
SUC 0 1
1
SUC 0 1
1
IMP 2
1
IMP 2
1, 3
IMP 3
1, 3
OUT
run_case "consultas nao criam versao"

# Testes oficiais que usam SUC: comparacao completa.
for number in 03 04 05; do
    make run INPUT="../../testes/teste${number}.in" \
        > "$test_dir/obtido.out" 2> "$test_dir/erro.out"
    if ! cmp -s "../../testes/teste${number}.out" "$test_dir/obtido.out"; then
        diff -u "../../testes/teste${number}.out" "$test_dir/obtido.out" >&2
        exit 1
    fi
    test ! -s "$test_dir/erro.out"
done

echo 'Issue #4: SUC estrito verificado'