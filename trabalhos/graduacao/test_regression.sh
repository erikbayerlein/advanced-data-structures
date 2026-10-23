#!/bin/sh
# Regressao de saida exata: compila, roda testes/teste01..05 e compara o stdout
# byte a byte com os .out do professor. Sai com codigo != 0 em qualquer falha.
# Uso: sh test_regression.sh   (qualquer diretorio)
set -eu

# Quando chamado por `make test`, o make filho imprimiria "Entering directory".
MAKEFLAGS="${MAKEFLAGS:-} --no-print-directory"
export MAKEFLAGS

cd "$(dirname "$0")"
tests_dir=../../testes
status=0

work=$(mktemp -d ./.regression.XXXXXX)
trap 'rm -rf "$work"' 0

# 1. Build: nada pode vazar para stdout; stderr (avisos) tambem deve ficar vazio.
if ! make build > "$work/build.out" 2> "$work/build.err"; then
    cat "$work/build.out" "$work/build.err" >&2
    echo 'FALHA: make build' >&2
    exit 1
fi
if [ -s "$work/build.out" ] || [ -s "$work/build.err" ]; then
    cat "$work/build.out" "$work/build.err" >&2
    echo 'FALHA: make build produziu saida inesperada' >&2
    exit 1
fi

# 2. Casos oficiais: comparacao literal (cmp), sem nenhuma normalizacao.
for number in 01 02 03 04 05; do
    input="$tests_dir/teste${number}.in"
    expected="$tests_dir/teste${number}.out"
    if [ ! -f "$input" ] || [ ! -f "$expected" ]; then
        echo "FALHA: teste${number}: arquivos ausentes em $tests_dir" >&2
        status=1
        continue
    fi
    if ! make run INPUT="$input" > "$work/obtido.out" 2> "$work/erro.out"; then
        echo "FALHA: teste${number}: make run retornou erro" >&2
        cat "$work/erro.out" >&2
        status=1
        continue
    fi
    if ! cmp -s "$expected" "$work/obtido.out"; then
        echo "FALHA: teste${number}: stdout difere de teste${number}.out" >&2
        diff -u "$expected" "$work/obtido.out" | head -40 >&2 || true
        status=1
        continue
    fi
    if [ -s "$work/erro.out" ]; then
        echo "FALHA: teste${number}: stderr nao vazio" >&2
        cat "$work/erro.out" >&2
        status=1
        continue
    fi
    echo "ok   teste${number}"
done

# 3. Autoverificacao do comparador: saidas esperadas adulteradas DEVEM divergir.
reference="$tests_dir/teste05.out"
make run INPUT="$tests_dir/teste05.in" > "$work/ref.out" 2> /dev/null

expect_mismatch() {
    if cmp -s "$2" "$work/ref.out"; then
        echo "FALHA: adulteracao nao detectada ($1)" >&2
        status=1
    else
        echo "ok   detecta: $1"
    fi
}

sed 's/, /,/' "$reference" > "$work/m1.out"          # virgula sem espaco
sed 's/, /,  /' "$reference" > "$work/m2.out"        # espaco extra
{ cat "$reference"; printf '\n'; } > "$work/m3.out"  # quebra de linha extra
sed '$d' "$reference" > "$work/m4.out"               # linha final ausente
sed 's/^inf$/INF/' "$reference" > "$work/m5.out"     # inf adulterado
sed 's/^SUC /SUC  /' "$reference" > "$work/m6.out"   # eco do comando alterado
expect_mismatch 'virgula sem espaco' "$work/m1.out"
expect_mismatch 'espaco extra' "$work/m2.out"
expect_mismatch 'quebra de linha extra' "$work/m3.out"
expect_mismatch 'linha final ausente' "$work/m4.out"
expect_mismatch 'inf adulterado' "$work/m5.out"
expect_mismatch 'eco de comando alterado' "$work/m6.out"

cmp -s "$reference" "$work/ref.out" || {
    echo 'FALHA: referencia de autoverificacao difere do teste05.out' >&2
    status=1
}

if [ "$status" -ne 0 ]; then
    echo 'Regressao: FALHOU' >&2
    exit 1
fi
echo 'Regressao: 5/5 testes oficiais identicos byte a byte'