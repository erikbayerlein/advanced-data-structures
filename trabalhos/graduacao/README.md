# Estruturas de Dados Avançadas — Trabalho 01 (Graduação)

Lista encadeada ordenada **parcialmente persistente**, pelo método de cópia de nós visto em sala (`p = 1`, `2p = 2` registros de modificação por nó).

## Equipe

<!-- TODO: nomes e matrículas -->

## Linguagem e ambiente

- **Linguagem:** C++17 (`-std=c++17`), apenas biblioteca padrão.
- **Testado em:** Linux, GCC 16.2.1 e GNU Make 4.4.1. Qualquer compilador C++17 serve (`make build CXX=clang++`).

## Compilar e executar

Nesta pasta:

```sh
make build
make run INPUT=../../testes/teste01.in
```

- `build` compila `main.cpp` e `persistent_list.cpp` no executável `programa`, sem imprimir nada.
- `run` compila se necessário e passa `INPUT` como **caminho** do arquivo de comandos. O caminho pode ser relativo ou absoluto, e a extensão não importa.
- O `stdout` contém somente as respostas. Erros (linha inválida, arquivo ausente) vão para o `stderr`, com código de saída 2.
- `make -C trabalhos/graduacao run INPUT=...` também não acrescenta linhas: o `Makefile` usa `--no-print-directory`.

## Arquivos, estruturas e funções

### `persistent_list.hpp` / `persistent_list.cpp`: lista persistente

- **`PersistentList`** é a estrutura principal.
  - `heads_` (`std::vector<Node*>`): cabeça de cada versão pública. A posição `0` é a versão vazia, e cada `INC`/`REM` acrescenta uma entrada.
  - `nodes_` (`std::vector<std::unique_ptr<Node>>`): dono de todos os nós físicos. Com `unique_ptr`, os endereços não mudam quando o vetor cresce.
- **`Node`**: nó físico.
  - `key`: chave, imutável.
  - `base_next`: elo original.
  - `modifications`: até **2** registros `Modification`.
  - `modification_count`: quantos registros estão em uso.
  - `predecessor`: ponteiro de retorno para o predecessor na **versão corrente**. Não é versionado e não é usado em consultas históricas.
- **`Modification`**: par `(version, next)`, que dá o valor de `next` a partir de `version`.
- **Funções de atualização e consulta:**
  - `next_at(node, version)`: entre `base_next` e os registros com carimbo `≤ version`, escolhe o de maior carimbo.
  - `write_next(node, target, version, head)`: grava `node.next ← target` na nova versão. Usa um registro livre, se houver. Se o nó estiver cheio, cria uma cópia com `base_next = target` e grava, recursivamente, o elo do predecessor corrente para a cópia. Se a cascata chegar à cabeça, a cópia vira a nova cabeça. Também atualiza os ponteiros de retorno.
  - `include(key)` (`INC`): procura o primeiro nó com chave `≥ key`, cria o nó novo antes dele e liga o predecessor por `write_next` (ou troca a cabeça). Publica a nova cabeça.
  - `remove(key)` (`REM`): procura a primeira ocorrência de `key`. Se a encontra, liga o predecessor ao sucessor por `write_next` (ou troca a cabeça). Se não encontra, repete a cabeça anterior. Cria uma versão nos dois casos.
  - `successor(key, version)` (`SUC`): percorre a versão e devolve a primeira chave `> key`, ou `std::nullopt` (impresso como `inf`).
  - `print(version, output)` (`IMP`): imprime as chaves da versão separadas por `, `. Uma lista vazia gera uma linha em branco.
  - `resolve_version(m)`: devolve `m` se a versão existe; senão, a mais recente.
- **Diagnóstico, usado só pelos testes:**
  - `version_count()`, `node_count()` e `modification_count()`: contadores estruturais.
  - `validate_current_links()`: ordem e ponteiros de retorno da versão corrente.
  - `validate_version(v)`: término, ordem e limite de registros na versão `v`.

### `main.cpp`: leitura e saída

- `Operation` e `Command`: comando já validado.
- `parse_integer`: converte texto em inteiro assinado de 64 bits (`std::from_chars`), recusando sobras e números fora da faixa.
- `parse_command`: reconhece `INC N`, `REM N`, `SUC N M` e `IMP M` e recusa `INC` negativo.
- `main`: abre `argv[1]` e lê linha a linha até o EOF (aceita `\r\n`). Executa `INC`/`REM` sem imprimir. Para `SUC`/`IMP`, ecoa a linha original e imprime a resposta. Processa em fluxo, sem guardar comandos nem respostas.

### `Makefile`

Alvos `build`, `run` (com `INPUT`) e `test` (roda `test_regression.sh`).

## Decisões locais

Pontos que o enunciado deixa em aberto:

- **Duplicatas:** são aceitas. `INC` insere antes da primeira chave igual e `REM` remove a primeira ocorrência.
- **Faixa numérica:** inteiros de 64 bits com sinal. `INC N` exige `N ≥ 0`. `REM N` com `N` negativo ou ausente não altera o conteúdo, mas cria versão. `SUC` com `N` negativo devolve a menor chave.
- **Versão inexistente:** `IMP M` e `SUC N M` usam a versão mais recente. A linha ecoada mantém o `M` original.
- **Formato de `IMP`:** vírgula e espaço (`1, 2, 3`), como em `TRABALHO.md` e nos `.out` do professor.
- **Entrada malformada:** mensagem no `stderr` e término com código 2, sem saída inventada no `stdout`.

## Invariantes

1. `heads_[t]` alcança exatamente as chaves da versão `t`, em ordem não decrescente. `heads_[0]` é nula.
2. Uma versão publicada nunca muda: as escritas da versão `t` levam carimbo `t`, e as leituras de versões `< t` as ignoram.
3. Todo nó tem no máximo 2 registros. Um nó cheio é copiado, e o original continua disponível para as versões antigas.
4. Na versão corrente, o `predecessor` de cada nó é seu predecessor real. A cabeça não tem predecessor.
5. Só `INC`/`REM` criam versões e nós. `SUC`/`IMP` apenas leem.

## Custos

Seja `n` o tamanho da versão envolvida e `v` o número de versões.

- **Leitura de um elo numa versão (`next_at`):** `O(1)` no pior caso. Examina `base_next` e no máximo 2 registros.
- **`SUC` e `IMP`:** `O(n)` no pior caso, sem alocação.
- **`INC` e `REM`:** `O(n)` para achar a posição. A operação faz uma única alteração de elo (ou troca a cabeça), mas essa alteração pode disparar uma cascata de cópias de até `O(n)` nós. Por isso **não** há custo constante no pior caso para cada atualização.
- **Amortizado:** considere o potencial Φ = número de registros ocupados nos nós da versão corrente. Gravar num registro livre custa `O(1)` e aumenta Φ em 1. Copiar um nó cheio custa `O(1)`, tira 2 registros ocupados da versão corrente e gera no máximo 1 nova gravação (no predecessor). Somando, cada alteração atômica de elo custa `O(1)` amortizado: é o sobrecusto multiplicativo constante pedido.
- **Espaço:** cada atualização cria no máximo 1 nó novo e termina com no máximo 1 gravação em registro livre. Como um nó só é copiado uma vez, o total é ≤ `v` registros e ≤ 1,5·`v` nós físicos, ou seja, `O(v)`. Consultas não alocam memória.

## Testes

```sh
make test               # teste01..05 do professor, byte a byte (test_regression.sh)
sh test_issue01.sh      # versão 0, extensão de INPUT, erros de entrada
sh test_issue02.sh      # INC/IMP, histórico, cópias e retornos (test_issue02.cpp)
sh test_issue03.sh      # REM: cabeça/meio/cauda, ausente, cascatas (test_issue03.cpp)
sh test_issue04.sh      # SUC estrito, duplicatas, versão inválida
sh test_issue06.sh      # invariantes, 1000 atualizações, espaço linear (test_issue06.cpp)
```

- `test_issue06.cpp` compara todas as versões com um oráculo de multiconjunto (200 sequências com semente fixa).
- Também força a 3ª escrita num mesmo elo e uma cascata até a cabeça.
- Confere 1001 versões após 1000 atualizações e imprime as contagens de nós e registros para 100, 500 e 1000 atualizações.
- Verifica que 100 mil consultas não alteram nenhum contador.
- O oráculo existe apenas no teste.

`test_issue03.sh` usa Python 3 só para preparar entradas temporárias. `make build` e `make run` não dependem de Python.
