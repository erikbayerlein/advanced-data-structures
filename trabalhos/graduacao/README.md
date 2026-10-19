# Trabalho 01 — Graduação

Linguagem: **C++17**. Esta pasta contém a solução em construção para a lista encadeada ordenada parcialmente persistente. No estado da issue [#1](https://github.com/erikbayerlein/advanced-data-structures/issues/1), somente `IMP` sobre a versão 0 vazia está implementado. `INC`, `REM` e `SUC` são reconhecidos e validados, mas ainda retornam erro em `stderr` até as próximas etapas.

## Executar

Nesta pasta, com um compilador C++17 e `make`:

```sh
make build
make run INPUT=caminho/para/entrada.in
sh test_issue01.sh
```

`run` recebe o caminho de `INPUT` como argumento do programa. A extensão do arquivo não importa. Respostas válidas vão somente para `stdout`; erros de entrada vão para `stderr` com código de saída diferente de zero.

Ao implementar `IMP` para listas não vazias na issue #2, separar os elementos com **vírgula e um espaço** (`1, 2`), exatamente como em `TRABALHO.md` e nos cinco `testes/*.out` atuais do professor. A versão atual ainda só imprime a lista vazia.

## Arquivos e componentes atuais

- `main.cpp`: `parse_integer` valida inteiros assinados de 64 bits; `parse_command` valida o nome e os argumentos dos quatro comandos; `main` abre o arquivo, lê até EOF e responde `IMP` da lista vazia.
- `Command` e `Operation`, em `main.cpp`: representam o comando já validado. `Node` ainda é apenas uma declaração antecipada; a issue #2 implementará os nós da lista.
- `heads`, em `main.cpp`: tabela de cabeças iniciada com `head[0] = nullptr`; consultas não acrescentam versões.
- `Makefile`: alvos `build` e `run`, com `INPUT` repassado ao programa.
- `test_issue01.sh`: verificação da saída exata da versão 0 e dos erros básicos.
