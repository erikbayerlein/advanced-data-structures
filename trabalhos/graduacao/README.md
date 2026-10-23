# Trabalho 01 — Graduação

Linguagem: **C++17**. Esta pasta contém a solução em construção para a lista encadeada ordenada parcialmente persistente. No estado da issue [#3](https://github.com/erikbayerlein/advanced-data-structures/issues/3), `INC`, `REM` e `IMP` estão implementados.
No estado da issue [#5](https://github.com/erikbayerlein/advanced-data-structures/issues/5), `INC`, `REM`, `SUC` e `IMP` estão implementados e os cinco testes publicados passam byte a byte.

## Executar

Nesta pasta, com um compilador C++17 e `make`:

```sh
make build
make run INPUT=caminho/para/entrada.in
sh test_issue01.sh
sh test_issue02.sh
sh test_issue03.sh
sh test_issue04.sh
make test        
# regressão: teste01..05 byte a byte (sh test_regression.sh)
```

`run` recebe o caminho de `INPUT` como argumento do programa. A extensão do arquivo não importa. Respostas válidas vão somente para `stdout`; erros de entrada vão para `stderr` com código de saída diferente de zero.

`IMP` separa elementos com **vírgula e um espaço** (`1, 2`), exatamente como em `TRABALHO.md` e nos `.out` atuais do professor. `test_issue02.sh` compara a saída com `teste01.out` e `teste02.out` byte a byte e verifica inserções adicionais, histórico, cópias de nós e predecessores correntes.

`test_issue03.sh` verifica remoções na cabeça, meio e cauda, duplicatas, remoção ausente e da lista vazia, histórico e cópias em cascata. Ele compara os comandos `INC`/`REM`/`IMP` de `teste04` e `teste05` com as linhas `IMP` correspondentes dos `.out` do professor; as linhas `SUC` são retiradas apenas das entradas temporárias do teste, pois essa operação pertence à issue #4.
Esse script usa Python 3 somente para preparar esses arquivos temporários; `make build` e `make run` não dependem de Python.

## Arquivos e componentes atuais

- `main.cpp`: `parse_integer` valida inteiros assinados de 64 bits; `parse_command` valida o nome e os argumentos dos quatro comandos; `main` abre o arquivo, lê até EOF, executa `INC`/`REM` e responde `IMP`.
- `Command` e `Operation`, em `main.cpp`: representam o comando já validado.
- `persistent_list.hpp` e `persistent_list.cpp`: `PersistentList` mantém a tabela de cabeças por versão e possui `include`, `remove`, `print`, `version_count`, `node_count` e `validate_current_links`. Cada `Node` tem chave, `next` base, dois registros versionados e retorno para o predecessor da versão corrente. `next_at` escolhe o elo válido para uma versão; `write_next` usa um registro livre ou copia o nó cheio e repara predecessores até a cabeça. `remove` reutiliza essa escrita para tirar uma ocorrência e registra uma nova cabeça mesmo sem acerto. Os nós pertencem a `unique_ptr`, preservando seus endereços durante o crescimento do vetor.
- `Makefile`: alvos `build` e `run`, com `INPUT` repassado ao programa e `test` (executa `test_regression.sh`).
- `test_issue01.sh`: verificação da saída exata da versão 0 e dos erros básicos.
- `test_issue02.cpp` e `test_issue02.sh`: verificação das invariantes da lista e comparação literal dos dois testes publicados que usam apenas `INC`/`IMP`.
- `test_issue03.cpp` e `test_issue03.sh`: verificação das remoções e comparação literal das consultas `IMP` nos testes publicados 04 e 05 após excluir `SUC` das entradas temporárias.
- `test_issue04.sh`: verifica `SUC` estrito (duplicatas, `N` ausente, fallback para a última versão com eco do `M` original, lista vazia, limites de 64 bits) e compara `teste03` a `teste05` por completo.
- `test_regression.sh` (`make test`): compila, roda `teste01` a `teste05` com `make run INPUT=...` e compara o `stdout` com os `.out` do professor via `cmp`, sem normalização. Exige `stderr` vazio e verifica que saídas adulteradas (vírgula, espaço, quebra de linha, `inf`) são detectadas.
