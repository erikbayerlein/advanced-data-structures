# Repasse de implementação do trabalho de Graduação de van Emde Boas

**Estado em 23/11/2026.** O enunciado foi analisado, a arquitetura foi decidida e as atividades foram publicadas. A implementação da vEB ainda precisa começar. Inicie pela [etapa 01, issue #11](https://github.com/erikbayerlein/advanced-data-structures/issues/11).

- [GitHub Project advanced-data-structures](https://github.com/users/erikbayerlein/projects/6) e [repositório das issues](https://github.com/erikbayerlein/advanced-data-structures/issues)
- [Enunciado](TRABALHO.md), [regras de entrega](README.md) e [pares de teste oficiais](testes/)
- [Guia de estudo](../outputs/guia-estudo-trabalho-veb-graduacao.md) e [videoaula de Victor Campos](https://www.youtube.com/watch?v=q5QBydFU3GU)
- [Revisão do repositório do professor usada no planejamento](https://github.com/victoitor/Trabalho-EDA-2026.2-vEB/commit/2d018722c2acb8a372583c27a80424adbadd9cb9)

## O que já foi feito

1. Revisão do enunciado, do README, dos modelos da equipe, dos oito pares oficiais e das fontes de apoio. O guia registra a especificação dos comandos, os custos, as alternativas e as referências por seção.
2. Segunda revisão da aula de Victor Campos com legendas automáticas em português e quadros dos algoritmos. A representação proposta abaixo segue os campos, as transições e a decomposição do professor. As adaptações para multiplicidade e para a tabela específica do trabalho foram decididas com base no enunciado.
3. Conferência dos oito `.out` originais com um oráculo de multiconjunto. O oráculo reproduziu os arquivos byte a byte, inclusive linhas em branco, `inf` e `-inf`. A execução dos oito testes com a solução será feita na etapa 07.
4. Publicação das nove issues de [#11](https://github.com/erikbayerlein/advanced-data-structures/issues/11) a [#19](https://github.com/erikbayerlein/advanced-data-structures/issues/19) no quadro, ordenadas pelos prefixos `[vEB 01/09]` a `[vEB 09/09]` em `Todo` e com dependências nativas no GitHub. Cada issue tem critérios de aceite, exemplos de teste e referências acessíveis pelo GitHub.

A pasta [`trabalhos/nome-equipe/`](trabalhos/nome-equipe/) contém o modelo de entrega. Seu `Makefile` referencia `main.cpp`, que ainda não existe neste projeto, e o README de exemplo descreve uma árvore persistente. A localização inicial proposta para a implementação é `trabalhos/graduacao/`, dentro desta pasta do trabalho. Ela pode assumir o nome definitivo da equipe na entrega. Os números das issues pertencem ao repositório `erikbayerlein/advanced-data-structures`, enquanto a origem deste checkout local é o repositório do professor.

## Decisões para levar à implementação

| Tema | Escolha e motivo | Evidência |
| --- | --- | --- |
| Representação | Guarde apenas `min` fora dos clusters e use `max` como cópia da maior chave. Em cada nó, um resumo vEB guarda os índices de clusters ocupados. Use os bits altos para selecionar o cluster e os baixos como chave local. | [Aula, 30:40](https://www.youtube.com/watch?v=q5QBydFU3GU&t=1840s) |
| Transições dos filhos | Materialize filho, tabela de posições e resumo à medida que aparecem entradas. Na inclusão de um filho unitário, acrescente um índice ao resumo. Na remoção que o esvazia, retire o índice e libere as estruturas vazias. A primeira inclusão ou última remoção de um filho termina imediatamente fora da tabela, deixando no máximo uma descida que continua em cada nível. | [Inclusão, 1:01:20](https://www.youtube.com/watch?v=q5QBydFU3GU&t=3680s) · [Remoção, 1:09:00](https://www.youtube.com/watch?v=q5QBydFU3GU&t=4140s) · [Espaço, 1:39:40](https://www.youtube.com/watch?v=q5QBydFU3GU&t=5980s) |
| `SUC` e `PRE` | Use extremos locais para escolher uma descida, no cluster ou no resumo. `PRE` segue a lógica simétrica, com uma exceção da variante da aula: o próprio mínimo pode ser a resposta quando falta cluster anterior. Use um resultado opcional para preservar todo o domínio de chaves. | [Definição de vizinhos, 23:00](https://www.youtube.com/watch?v=q5QBydFU3GU&t=1380s) · [Sucessor, 53:40](https://www.youtube.com/watch?v=q5QBydFU3GU&t=3220s) |
| Duplicatas | Guarde a frequência no registro canônico da chave. Com isso, a troca ou promoção de mínimo descrita pelo professor move um registro inteiro. O `max` continua uma cópia da chave sem frequência própria, e os índices dos resumos são distintos. Essa adaptação preserva o desenho da aula e evita um segundo índice de dados. | [Aula, 1:01:20 e troca do mínimo](https://www.youtube.com/watch?v=q5QBydFU3GU&t=3680s) · [Enunciado, linhas 8–10](TRABALHO.md#L8) |
| Hash dos clusters | Implemente sondagem linear circular e tabulação simples com quatro conjuntos compartilhados de 256 palavras aleatórias. Use capacidade inicial 8, dobra quando a próxima inserção ultrapassaria `1/2`, metade abaixo de `1/8` e liberação quando vazia. Apague com deslocamento para trás para preservar as sequências de sondagem e evitar o acúmulo de marcas. A aula propõe uma tabela esparsa por nó, o enunciado fixa a família de hash e a sondagem, e os limiares e a técnica de remoção são escolhas locais. | [Aula, 1:39:40](https://www.youtube.com/watch?v=q5QBydFU3GU&t=5980s) · [Enunciado, linhas 12–28](TRABALHO.md#L12) |
| Base, tipos e propriedade | Use largura de 32 bits na raiz e dois contadores na base de 1 bit. Proposta inicial em C++17 com `std::uint32_t` para chaves, `std::uint64_t` para frequências, presença explícita e propriedade RAII dos nós. Resolva 0 e 1 diretamente no caso base. Detecte estouro do contador como política local. A largura da chave decorre do domínio, e C++17 segue o exemplo de compilação do README. | [Enunciado, linhas 1–10](TRABALHO.md#L1) · [README, exemplo de Makefile](README.md#L27) · [Notas MIT 6.046J, p. 6](resources/c02af0a02710642161276560ab21510d_MIT6_046JS12_lec15.pdf#page=6) |
| `IMP` e entrada | Para `IMP`, emita o mínimo seguido dos clusters na ordem de seus índices no resumo, repetindo conforme a frequência. A travessia usa a ordem já mantida pela vEB e funciona sem buffer proporcional às ocorrências. Preserve no eco o corpo da linha do comando. Para arquivo inválido, proposta de diagnóstico em `stderr` e código de erro, com `stdout` reservado para as respostas previstas. | [Enunciado, operações e formatação](TRABALHO.md#L30) · [Aula, resumo e clusters aos 30:40](https://www.youtube.com/watch?v=q5QBydFU3GU&t=1840s) |

A aula descreve a tabela esparsa com acesso constante com alta probabilidade na análise. Para a tabela exigida no trabalho, explique a garantia em termos de **custo esperado da sondagem** e **amortização** da reconstrução. A redução pela metade limita espaço em função das chaves **atuais**. `IMP` deve contabilizar também as ocorrências emitidas. [Enunciado, linhas 12–28](TRABALHO.md#L12) · [Pătrașcu e Thorup, § 5](https://people.csail.mit.edu/mip/papers/charhash/charhash.pdf#page=24)

## O que falta fazer, em ordem

### 01. [#11 — Preparar execução Linux, parser e `IMP` vazio](https://github.com/erikbayerlein/advanced-data-structures/issues/11)

**Como fazer.** Criar o executável da solução, ler o arquivo indicado por `make run INPUT=...`, validar chave decimal sem sinal e preservar o texto da operação. Começar com o caso `IMP` vazio, com segunda linha em branco. Silenciar o eco de receitas do Make.

**Como testar.** Compile em Linux. Passe um arquivo chamado `entrada vazia.in` com duas linhas `IMP`. Compare com `IMP\n\nIMP\n\n`. Teste arquivo inexistente, entrada vazia e limites do parser, incluindo `4294967296` como erro de entrada.

### 02. [#12 — Implementar `INC` e `IMP` com a vEB esparsa](https://github.com/erikbayerlein/advanced-data-structures/issues/12)

**Como fazer.** Implemente a decomposição em bits, o caso base, os extremos e o resumo. Na primeira inserção num nó, guarde `min = max` e encerre. Na troca de mínimo, recompute as coordenadas do registro deslocado. Implemente a tabela de clusters do zero, com tabulação compartilhada, colisões por tentativa linear e crescimento por dobra. Emita as chaves distintas de `IMP` percorrendo o resumo em ordem e recompondo os resíduos. A próxima etapa acrescenta multiplicidades.

**Como testar.** Compare `teste01` e `teste02` completos. Insira `65536`, `4294967295`, `65535`, `0` e confira `0, 65535, 65536, 4294967295`. Teste colisão, retorno circular, busca ausente e rehash isoladamente. Um único registro na árvore não deve materializar filhos nem tabela.

### 03. [#13 — Aplicar multiplicidades a `INC` e `IMP`](https://github.com/erikbayerlein/advanced-data-structures/issues/13)

**Como fazer.** Localize a chave pelo mínimo e pelo cluster correspondente. Incremente o contador se já existir. O registro canônico contém a chave e sua frequência, e é movido inteiro na troca do mínimo. `IMP` emite a frequência sem duplicar nós ou índices.

**Como testar.** `INC 9`, `INC 9`, `INC 1`, `INC 9`, `IMP` deve emitir `1, 9, 9, 9`. Cubra repetições no mínimo, no máximo e no caso base. Repita a mesma chave e verifique que a quantidade de nós, índices e posições alocadas não cresce.

### 04. [#14 — Implementar sucessor estrito](https://github.com/erikbayerlein/advanced-data-structures/issues/14)

**Como fazer.** Use `N < min` e `N >= max` para decidir as fronteiras. Escolha procurar no cluster se seu máximo local é maior que o resíduo. Caso contrário, encontre o próximo índice no resumo e use o mínimo do filho correspondente. Ecoe a linha e imprima `inf` quando ausente.

**Como testar.** Com `{0, 65534, 65535, 65536, 65536, 65538, 131072, 4294967295}`, `SUC 65534`, `SUC 65535`, `SUC 65536` e `SUC 4294967295` devolvem `65535`, `65536`, `65538` e `inf`. Compare `teste03` integralmente. A consulta também precisa funcionar com limiar ausente, como `65537`.

### 05. [#15 — Implementar predecessor estrito](https://github.com/erikbayerlein/advanced-data-structures/issues/15)

**Como fazer.** Use `N <= min` para ausência e `N > max` para retornar o máximo. Se o cluster atual não oferece resposta, busque o índice anterior no resumo e use seu máximo. Sem índice anterior, retorne o mínimo externo quando ele for menor que `N`. Ecoe a linha e imprima `-inf` quando ausente.

**Como testar.** Com `{0, 65535, 65536, 65536, 65538, 4294967295}`, `PRE 0`, `PRE 1`, `PRE 65536`, `PRE 65538` e `PRE 4294967295` devem devolver `-inf`, `0`, `65535`, `65536` e `65538`. As etapas 04 e 05 podem ser implementadas em paralelo depois da 03.

### 06. [#16 — Implementar `REM`, liberação e redução de tabelas](https://github.com/erikbayerlein/advanced-data-structures/issues/16)

**Como fazer.** Remova uma ocorrência ou preserve o estado se a chave está ausente. Na última ocorrência do mínimo, promova o menor registro do primeiro cluster e retire esse registro por completo do filho. Atualize `max`. Se um cluster esvaziar, apague sua entrada da tabela e seu índice do resumo. Use deslocamento para trás para fechar lacunas da sondagem e reduza o vetor pela metade abaixo do limiar. Desalocar estruturas vazias inclui devolver a capacidade retida.

**Como testar.** Insira `9` duas vezes e `1` uma vez. Remova `1` e confirme `9, 9`. Remova `9` duas vezes, imprimindo após cada remoção, e confirme `9` seguido de uma segunda linha vazia na próxima impressão. Compare `teste04` a `teste08`. No teste de tabela, use origens `6, 6, 7` em capacidade 8 e remova a primeira entrada, verificando as buscas que atravessam o fim do vetor. Cresça com chaves de partes altas distintas, reduza, esvazie e reinsira.

### 07. [#17 — Automatizar a regressão funcional](https://github.com/erikbayerlein/advanced-data-structures/issues/17)

**Como fazer.** Crie uma instrução única para rodar os oito pares oficiais e comparar o `stdout` byte a byte. Adicione cenários próprios com saída explícita e compare sequências variadas com um oráculo de multiconjunto que só existe nos testes.

**Como testar.** Rode a suíte em Linux. Confira `inf`, `-inf`, `, `, a linha vazia de `IMP` e a ausência de saída de `INC` e `REM`. Altere um caractere numa cópia temporária de uma expectativa para conferir que a suíte detecta erro. Use semente reproduzível nas sequências diferenciais.

### 08. [#18 — Verificar invariantes e custos](https://github.com/erikbayerlein/advanced-data-structures/issues/18)

**Como fazer.** Verifique que os índices do resumo coincidem com os filhos vivos, a representação dos extremos está correta e as frequências são positivas. Conte nós, resumos, entradas e capacidade. Registre a recorrência em bits, de profundidade `O(log w)`, e o argumento de espaço para 32 bits, incluindo índices dos resumos, tabela por nó e tabulação compartilhada.

**Como testar.** Examine todos os subconjuntos de um universo de três bits. Em 32 bits, compare muitas ocorrências de uma só chave com muitas chaves distintas, examine o espaço depois de reduzir e esvaziar e confirme que consultas não materializam subestruturas. Rode as verificações de memória e a suíte funcional no mesmo código.

### 09. [#19 — Documentar e verificar a entrega em Linux](https://github.com/erikbayerlein/advanced-data-structures/issues/19)

**Como fazer.** Na pasta da equipe, reúna código, `Makefile` e README da solução. Informe a linguagem e a versão, descreva cada função e estrutura com o arquivo correspondente e documente invariantes, custos e comandos. Confira o limite de três integrantes de Graduação.

**Como testar.** Use um checkout limpo em Linux. Execute `make build`, `make run INPUT=...`, os oito testes e os testes de invariantes. Releia o README comparando nomes, caminhos e comandos com o código. Certifique-se de que o resultado redirecionado contém somente as respostas exigidas.

## Como comparar os oito pares oficiais

A partir da raiz desta pasta do trabalho, execute o exemplo abaixo depois que as operações estiverem prontas e a pasta proposta `trabalhos/graduacao/` existir. Ajuste o primeiro caminho caso a equipe escolha outro nome.

```sh
(
    set -eu
    cd trabalhos/graduacao
    make build
    resultados=$(mktemp -d)
    trap 'rm -rf "$resultados"' EXIT
    for caso in ../../testes/teste*.in; do
        nome=$(basename "$caso" .in)
        make run INPUT="$caso" > "$resultados/$nome.out"
        diff -u "${caso%.in}.out" "$resultados/$nome.out"
        printf '%s OK\n' "$nome"
    done
)
```

A suíte da etapa 07 deve oferecer um comando equivalente. Durante a implementação, use os subconjuntos de testes indicados em cada issue. Com `diff -u`, uma execução sem diferenças emite somente as confirmações do laço acima.

## Pontos em aberto no enunciado

- O nome definitivo da pasta da equipe, prazo e canal de envio dependem de orientação do professor. O README local apresenta um modelo e lista os arquivos obrigatórios.
- O trabalho não fixa comportamento para comandos inválidos, linhas vazias, espaços adicionais ou multiplicidade além da largura escolhida para o contador. As decisões defensivas ficam fora dos requisitos de avaliação publicados.
- A comparação exata e as metas assintóticas estão documentadas. Pesos da nota e limites de tempo ou memória do ambiente de avaliação ainda não foram informados nesses documentos.

## Skills sugeridas para continuar

- `implement` para executar a primeira issue pendente e seguir as dependências.
- `diagnosing-bugs` para investigar divergência de saída ou violação de invariantes.
- `code-review` para revisar a solução final contra o enunciado e os testes.
