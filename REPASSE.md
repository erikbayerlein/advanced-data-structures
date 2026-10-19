# Repasse — Trabalho 01 de Graduação

**Estado atualizado em 19/10/2026.** A issue [#1](https://github.com/erikbayerlein/advanced-data-structures/issues/1) foi implementada e verificada. As issues [#2](https://github.com/erikbayerlein/advanced-data-structures/issues/2) a [#7](https://github.com/erikbayerlein/advanced-data-structures/issues/7) seguem como próximas etapas, na ordem do [GitHub Project advanced-data-structures](https://github.com/users/erikbayerlein/projects/6).

## Escopo e fontes

O trabalho de **Graduação** pede uma **lista encadeada ordenada com persistência parcial**, não a árvore da seção de Pós-graduação ([TRABALHO.md](TRABALHO.md), abertura e § Graduação). Os comandos são `INC N`, `REM N`, `SUC N M` e `IMP M`. A versão 0 é vazia; cada inclusão ou remoção cria uma versão pública; há no máximo 1000 atualizações e, portanto, até 1001 versões. Consultas não criam versões. A entrega precisa de código-fonte, `Makefile` com `build`/`run` em Linux e README da solução ([README.md](README.md), §§ Envio e Entrada e saída).

Use [TRABALHO.md](TRABALHO.md) e [README.md](README.md) como fontes dos requisitos. O [guia de estudo](outputs/guia-estudo-trabalho-01-graduacao.md) reúne a fundamentação da estrutura, algoritmos, custos, casos de teste e lacunas. As issues são o plano executável: cada uma contém critérios de aceite, fontes e dependência. Este repasse indica a sequência e o próximo passo; não substitui esses documentos.

**Decisões já tomadas:** `IMP` deve separar os valores com **`, `**, conforme o enunciado e a escolha do usuário. Para `SUC` com versão `M` inexistente, consultar a versão mais recente, também por escolha do usuário; o enunciado explicita esse fallback somente para `IMP`. A implementação recomendada no guia é uma lista simples, com cópia de nós, dois registros de modificação e um retorno para o predecessor corrente por nó (`p=1`), como na videoaula do professor. Essa representação é uma escolha técnica fundamentada, não uma exigência textual de quantidade de campos.

**Atenção à avaliação:** os cinco arquivos `.out` publicados usam espaço simples em `IMP`, em conflito com o `, ` do enunciado. Conservar os exemplos originais e criar saídas esperadas próprias para a decisão adotada. A comparação de saída é literal, incluindo espaços e quebras de linha. O formato que o corretor usará não foi esclarecido.

## O que já foi feito

1. Análise dos documentos e dos cinco pares de testes, restrita à Graduação, registrada no [guia de estudo](outputs/guia-estudo-trabalho-01-graduacao.md).
2. Criação e vinculação do [GitHub Project](https://github.com/users/erikbayerlein/projects/6) ao repositório; as issues [#1](https://github.com/erikbayerlein/advanced-data-structures/issues/1) a [#7](https://github.com/erikbayerlein/advanced-data-structures/issues/7) foram publicadas e ordenadas por prefixo `[01/07]` a `[07/07]`.
3. O repositório já oferece os casos de entrada e saída em [`testes/`](testes/) e modelos em [`trabalhos/nome-equipe/`](trabalhos/nome-equipe/). **São materiais de partida, não a solução:** o `Makefile` do modelo depende de `main.cpp` inexistente e seu README descreve uma BST.
4. **Issue #1 concluída:** a solução começou em [`trabalhos/graduacao/`](trabalhos/graduacao/), em **C++17**, com `Makefile`, parser, tabela de cabeças iniciada na versão 0 vazia, `IMP` dessa versão e [teste executável](trabalhos/graduacao/test_issue01.sh). `make build` e `sh test_issue01.sh` passaram no macOS e em um contêiner Linux Alpine com `g++` e `make`. O teste compara `IMP 0` e `IMP 99` byte a byte, repete com `.txt`, e verifica arquivo ausente e comando malformado. `INC`, `REM` e `SUC` são reconhecidos, mas ainda retornam erro sem resposta fictícia.

**Ainda não existe** a lista persistente, nem as operações completas `INC`, `REM`, `SUC` e `IMP` para versões não vazias. A suíte abrangente é a issue #5; o README da solução ainda é inicial e será completado na #7. O nome `trabalhos/graduacao` pode ser ajustado se a equipe definir outro nome de entrega. O Project está privado, embora as issues sejam públicas; quem for usar o quadro precisa ter acesso.

## O que falta fazer, em ordem

### 2. [#2 — `INC` persistente e `IMP` histórico](https://github.com/erikbayerlein/advanced-data-structures/issues/2)

**Fazer:** implementar a lista simples em ordem não decrescente, com duplicatas; uma cabeça por versão pública; leitura de `next` pela última modificação aplicável; e escrita persistente com dois registros, cópia de nó cheio e reparo dos predecessores correntes. `INC` cria exatamente uma nova versão e não imprime resposta. Completar `IMP` para versões históricas e atuais, com separador `, ` e fallback à última versão quando `M` não existe. Usar identidade estável para os nós.

**Testar:** executar, em linhas separadas, `INC 2`, `INC 5`, `INC 3`, `INC 3` e `IMP 1` até `IMP 4`. Os conteúdos esperados são, respectivamente, `2`; `2, 5`; `2, 3, 5`; `2, 3, 3, 5`. Verificar que uma terceira mudança no mesmo elo força cópia sem mudar as versões antigas. Incluir `0`, `32767`, `65535`, cabeça/meio/cauda, duplicatas e as entradas `teste01.in` e `teste02.in`.

### 3. [#3 — `REM` persistente de uma ocorrência](https://github.com/erikbayerlein/advanced-data-structures/issues/3)

**Fazer:** remover somente a primeira ocorrência encontrada na versão corrente. Atualizar persistentemente o elo do predecessor ou a cabeça. Se a chave estiver ausente, publicar uma nova versão com a mesma cabeça e sem mudar o conteúdo. Reutilizar o mecanismo de cópias da #2 e manter os retornos correntes coerentes. A frase “idêntica em ambos os casos” do enunciado é contraditória; exemplos e operação de remoção sustentam conteúdo idêntico apenas quando não houve acerto.

**Testar:** cobrir remoção na cabeça, no meio, na cauda, de duplicatas, de chave ausente e da lista vazia. Para uma cascata de cópias, executar `INC 30`, `INC 20`, `INC 10`, `INC 20`, `REM 20`, `INC 30`, `REM 30`, `INC 25`: versões 3, 5 e 7 devem conter `10, 20, 30`; versão 8, `10, 20, 25, 30`. Conferir as entradas `teste04.in` e `teste05.in`.

### 4. [#4 — `SUC` estrito em versões históricas](https://github.com/erikbayerlein/advanced-data-structures/issues/4)

**Fazer:** escolher a versão `M` se existir ou a última se não existir. Percorrer seus elos versionados até o primeiro valor **maior** que `N`; `N` não precisa estar presente. Imprimir a linha original do comando e, abaixo, o valor encontrado ou `inf`. A consulta não cria versão nem nó; não calcular `N+1`, para evitar estouro.

**Testar:** em uma versão com `2, 5, 5, 9`, limiares `1`, `2`, `5`, `9` devem produzir `2`, `5`, `9`, `inf`. Após `INC 7`, `SUC 0 99`, `SUC 7 99`, `SUC 0 -1` devem responder `7`, `inf`, `7`, usando a última versão sem alterar a linha do comando. Consultar versões antigas após remoções e executar `teste03.in`, `teste04.in` e `teste05.in`.

### 5. [#5 — Regressão de comandos e saída exata](https://github.com/erikbayerlein/advanced-data-structures/issues/5)

**Fazer:** criar um comando de teste reproduzível que compile, execute arquivos de entrada e compare o `stdout` **byte a byte**. Exercitar os cinco `.in` publicados e criar oráculos próprios com `, ` para `IMP`; não editar os `.out` originais nem normalizar espaços. Acrescentar casos ausentes nos exemplos: duplicatas, versões inválidas em `IMP`/`SUC`, `REM` ausente, versão 0, consultas intercaladas e saída vazia.

**Testar:** o comando de teste deve passar na solução correta e falhar ao mudar uma vírgula, espaço, quebra de linha, `inf`, versão ou resposta histórica. Comparar também os exemplos publicados para separar a divergência conhecida de separador de qualquer erro semântico. O `stdout` de `INC`/`REM` deve ficar vazio.

### 6. [#6 — Invariantes e limites de tempo e memória](https://github.com/erikbayerlein/advanced-data-structures/issues/6)

**Fazer:** instrumentar testes para verificar ordenação, cabeças históricas estáveis, escolha do registro versionado, retornos da versão corrente e cópias em cascata. Contar nós e cabeças para sustentar espaço total `O(v)`, sem guardar uma cópia inteira da lista por versão. Distinguir a leitura `O(1)` por elo, o percurso `O(n)` de uma operação na lista e o custo **amortizado** `O(1)` da transformação por escrita de elo; uma cascata isolada pode levar `O(n)`.

**Testar:** gerar 1000 atualizações mistas e conferir versões `0..1000`; comparar sequências pequenas com um oráculo de multiconjuntos usado **só nos testes**. Repetir muitas consultas e verificar que nós, registros e cabeças permanentes não crescem. Reexecutar a cascata da #3 e toda a suíte da #5. Evitar um limite frágil de tempo de parede como único teste de complexidade.

### 7. [#7 — README e entrega Linux](https://github.com/erikbayerlein/advanced-data-structures/issues/7)

**Fazer:** concluir o README da **solução** na pasta da entrega: linguagem e versão, comandos de compilação/execução, descrição de cada função e estrutura com seu arquivo, invariantes, custos e decisões locais. Conferir que código-fonte, `Makefile` e README estão versionados e não dependem de caminhos absolutos ou artefatos gerados no computador do autor. Não reutilizar a descrição de BST do modelo.

**Testar:** em um checkout Linux limpo, a partir da pasta entregue, rodar `make build`, `make run INPUT=<caminho-para-um-.in>` e a suíte da #5; conferir também as propriedades da #6. Fazer uma leitura cruzada do README com os nomes reais do código. A saída comparada não pode conter eco de comando do `make` nem mensagens de compilação/depuração.

## Como executar a verificação de saída

Entre na pasta [`trabalhos/graduacao/`](trabalhos/graduacao/). A verificação automatizada da etapa concluída é `sh test_issue01.sh`. Depois de implementar as issues #2 a #4, um exemplo para as entradas publicadas será:

```sh
make build
make run INPUT=../../testes/teste01.in > /tmp/trabalho01-obtido.out
diff -u /tmp/trabalho01-esperado.out /tmp/trabalho01-obtido.out
```

Crie `/tmp/trabalho01-esperado.out` com as linhas `IMP` separadas por `, `; os `.out` publicados não servem como oráculo literal dessas linhas. Ajuste o caminho relativo de `INPUT` se a pasta escolhida mudar. Um `diff` sem saída indica igualdade de bytes; a suíte da #5 deverá tornar uma diferença em falha. **Ainda não rode os cinco `.in` como teste de aceite:** eles exigem as operações das issues #2 a #4.

## Decisões ainda abertas e limites da informação

- Confirmar se `trabalhos/graduacao/` será o nome definitivo da pasta da equipe. A linguagem inicial escolhida é C++17.
- O enunciado não fixa um limite superior de `N` nem a resposta a comandos inválidos. O guia sugere inteiro assinado de pelo menos 64 bits (ou precisão arbitrária), rejeitar `INC` negativo e reportar entrada inválida em `stderr`. Documentar a política escolhida sem tratá-la como requisito oficial.
- Não há informação publicada sobre pesos da avaliação, casos ocultos, prazo, ou se o corretor seguirá o enunciado ou os `.out` no separador de `IMP`. A escolha registrada do usuário é `, `.

## Skills sugeridas para continuar

- `implement`: executar as issues na ordem acima e manter cada etapa funcional.
- `diagnosing-bugs`: investigar falhas concretas da suíte ou de persistência histórica.
- `code-review`: revisar a implementação final contra o enunciado, as invariantes e a saída literal.
