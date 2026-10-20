# Trabalho 01 — Graduação

Linguagem: **C++17**. Esta pasta contém a solução em construção para a lista encadeada ordenada parcialmente persistente. No estado da issue [#2](https://github.com/erikbayerlein/advanced-data-structures/issues/2), `INC` e `IMP` estão implementados. `REM` e `SUC` são reconhecidos e validados, mas ainda retornam erro em `stderr` até as próximas etapas.

## Executar

Nesta pasta, com um compilador C++17 e `make`:

```sh
make build
make run INPUT=caminho/para/entrada.in
sh test_issue01.sh
sh test_issue02.sh
```

`run` recebe o caminho de `INPUT` como argumento do programa. A extensão do arquivo não importa. Respostas válidas vão somente para `stdout`; erros de entrada vão para `stderr` com código de saída diferente de zero.

`IMP` separa elementos com **vírgula e um espaço** (`1, 2`), exatamente como em `TRABALHO.md` e nos `.out` atuais do professor. `test_issue02.sh` compara a saída com `teste01.out` e `teste02.out` byte a byte e verifica inserções adicionais, histórico, cópias de nós e predecessores correntes.

## Arquivos e componentes atuais

- `main.cpp`: `parse_integer` valida inteiros assinados de 64 bits; `parse_command` valida o nome e os argumentos dos quatro comandos; `main` abre o arquivo, lê até EOF, executa `INC` e responde `IMP`.
- `Command` e `Operation`, em `main.cpp`: representam o comando já validado.
- `persistent_list.hpp` e `persistent_list.cpp`: `PersistentList` mantém a tabela de cabeças por versão e possui `include`, `print`, `version_count`, `node_count` e `validate_current_links`. Cada `Node` tem chave, `next` base, dois registros versionados e retorno para o predecessor da versão corrente. `next_at` escolhe o elo válido para uma versão; `write_next` usa um registro livre ou copia o nó cheio e repara predecessores até a cabeça. Os nós pertencem a `unique_ptr`, preservando seus endereços durante o crescimento do vetor.
- `Makefile`: alvos `build` e `run`, com `INPUT` repassado ao programa.
- `test_issue01.sh`: verificação da saída exata da versão 0 e dos erros básicos.
- `test_issue02.cpp` e `test_issue02.sh`: verificação das invariantes da lista e comparação literal dos dois testes publicados que usam apenas `INC`/`IMP`.
