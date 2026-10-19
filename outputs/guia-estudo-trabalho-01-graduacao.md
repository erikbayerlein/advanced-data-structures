# Guia de estudo — Trabalho 01 de Graduação: lista ordenada parcialmente persistente

## Escopo e conclusão

**A estrutura pedida é uma lista encadeada ordenada com persistência parcial.** Isso está escrito em [TRABALHO.md](../TRABALHO.md), § Graduação, linhas 10–12, sob o objetivo geral de persistência parcial das linhas 1–6. A árvore binária de busca da seção seguinte é uma tarefa distinta. O [README de exemplo](../trabalhos/nome-equipe/README.md), linhas 13–22, também menciona uma árvore, mas é um modelo de documentação, não altera a seção específica da Graduação.

O enunciado **não especifica** se a lista é simples ou dupla e remete ao “método descrito em sala” ([TRABALHO.md](../TRABALHO.md), linha 3). Na [videoaula de persistência indicada pelo usuário, 18:23–19:05](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=1103s), a lista simples tem grau de entrada `p=1`; a exposição usa [até `p` ponteiros de retorno e `2p` registros de modificação por nó, 24:00–29:24](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=1440s), seguidos de cópia e reparo dos predecessores quando os registros se esgotam. Assim, a leitura mais bem sustentada é uma **lista simplesmente encadeada em ordem não decrescente, parcialmente persistente por cópia de nós**, com **dois registros de modificação e um retorno por nó**, além de uma cabeça por versão. A ordenação e as operações vêm do enunciado; a configuração `2p` vem da aula. [Driscoll et al., § 2.3, pp. 93–97](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf), as [notas MIT, §§ 2–3](https://courses.csail.mit.edu/6.851/spring21/scribe/lec1.pdf) e as [notas de Wild, §§ 4.5–4.6](https://www.wild-inter.net/teaching/advds/notes-04-persistence.pdf) corroboram a técnica e os custos. A aplicação de **um registro e caminho de predecessores** a esta lista é uma inferência a partir do exemplo de árvore com caminho único em [Driscoll et al., § 2.3.3, p. 95](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf), **não** a configuração ensinada nessa videoaula; o vídeo tampouco diz se tal especialização seria aceita para este trabalho.

| Leitura possível | Compatibilidade com o enunciado | Custo e decisão |
|---|---|---|
| Lista simplesmente encadeada ordenada, com cópia de nós | Satisfaz literalmente a seção Graduação e a persistência parcial; corresponde ao exemplo `p=1` da aula. | Preferida: `next` é o único elo, `p=1`, dois registros de modificação e um retorno para o predecessor corrente. |
| Lista duplamente encadeada ordenada, com cópia de nós | Também é uma lista encadeada; o enunciado não a proíbe. | Exige versionar e reparar `next` e `prev`; aumenta estados e casos de erro sem ajudar as operações pedidas. O grau de entrada continua constante. |
| Lista com cópia integral a cada versão ou cópia de todo o prefixo | Preserva respostas antigas e pode ter custo `O(n)` por atualização. | Em sequências de atualizações, usa até `Θ(v²)` nós, contrariando o espaço `O(v)` das linhas 5–6. |
| *Fat nodes* sem limite de modificações | Preserva o passado e usa `O(v)` espaço total. | A busca por uma modificação histórica geralmente acrescenta `O(log v)` por leitura de campo; não sustenta o fator `O(1)` pedido. |

As comparações de alternativas são inferências algorítmicas apoiadas nas [notas de Wild, slides pp. 8–15](https://www.wild-inter.net/teaching/advds/04-persistence.pdf) e em [Driscoll et al., §§ 2.2–2.3](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf). O enunciado fixa a lista e os limites; a videoaula esclarece a transformação geral, mas não especifica a implementação deste trabalho.

## 1. Requisitos obrigatórios da Graduação

As referências nesta seção são **documentos deste projeto**. “Versão” abaixo designa uma versão visível aos comandos de entrada; versões internas temporárias são permitidas, mas não exigidas.

| ID | Requisito obrigatório | Evidência no projeto | Componente da solução |
|---|---|---|---|
| R1 | Implementar lista encadeada **ordenada** e parcialmente persistente. | [TRABALHO.md](../TRABALHO.md), abertura, linhas 1–6, e § Graduação, linhas 10–12. | Nós ligados por `next`, cabeça de cada versão, registros de modificação e retornos correntes conforme a aula; os detalhes internos são solução proposta, não exigências textuais. |
| R2 | Cada nó guarda inteiro não negativo representável com pelo menos 16 bits; valores repetidos são aceitos. | [TRABALHO.md](../TRABALHO.md), linha 8. | Campo `chave`; comparação `≤` na lista; inclusão não deduplica. |
| R3 | `INC N` insere uma ocorrência, inclusive se `N` já existe, e cria versão. | [TRABALHO.md](../TRABALHO.md), § Operações, linhas 22–29. | Busca da posição, novo nó, ajuste persistente de um elo ou da cabeça. |
| R4 | `REM N` remove **uma** ocorrência. Se ausente, conteúdo fica igual; em todo caso nasce uma versão. | [TRABALHO.md](../TRABALHO.md), § Operações, linhas 31–40; § Versões, linhas 81–83. | Busca do primeiro igual; salto persistente sobre o nó ou repetição da cabeça anterior. |
| R5 | `SUC N M` imprime a linha do comando e, na linha seguinte, o menor valor **estritamente maior** que `N` na versão `M`, mesmo se `N` não existe. Sem tal valor, imprime `inf`. | [TRABALHO.md](../TRABALHO.md), § Operações, linhas 42–58. | Percurso versionado até a primeira chave `> N`; saída de duas linhas. Para `M` inexistente, aplicar a decisão complementar do usuário descrita no § 2. |
| R6 | `IMP M` imprime a linha do comando e, na linha seguinte, os elementos crescentes da versão `M`, separados por **vírgula e espaço**; se `M` não existe, usa a versão mais recente; lista vazia produz linha em branco. Não cria versão. | [TRABALHO.md](../TRABALHO.md), § Operações, linhas 60–76. | Consulta à tabela de cabeças, percurso versionado e formatação `, `. Os `.out` atuais do professor confirmam o separador; ver § 2. |
| R7 | A versão 0 é vazia. Cada `INC` e `REM` aumenta o identificador em um; `SUC`/`IMP` não aumentam. | [TRABALHO.md](../TRABALHO.md), § Versões, linhas 79–88. | Contador e tabela de cabeças. |
| R8 | Entregar código-fonte, `Makefile` com `build` e `run` operantes em Linux, e `README.md` da solução com linguagem/versão e descrição/local de cada função e estrutura. `run` lê o arquivo indicado por `INPUT`; saída no terminal. | [README.md](../README.md), §§ Envio e Entrada e saída, linhas 15–24 e 47–50. | Programa, integração por `make`, documentação da equipe. |

### Restrições de capacidade e custo

- O sobrecusto multiplicativo de uma operação em qualquer versão deve ser `O(1)` **amortizado** e o espaço **total** com `v` versões deve ser `O(v)` ([TRABALHO.md](../TRABALHO.md), linhas 5–6). No método de cópia de nós, a prova publicada também é amortizada por alteração atômica; cada leitura de um elo histórico tem custo `O(1)` ([Driscoll et al., § 2.3.4, pp. 96–97](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf)).
- Há no máximo **1000** comandos `INC`/`REM`, logo até **1001** versões públicas. Uma tabela de tamanho 1001 para identificadores de versões é **permitida**, não imposta. `SUC` e `IMP` não têm limite de quantidade e não podem fazer a memória crescer por consulta ([TRABALHO.md](../TRABALHO.md), § Versões, linhas 84–88).
- A equipe de Graduação pode ter até três integrantes ([README.md](../README.md), § Grupos, linhas 10–13).
- O caminho de entrada vem de `INPUT`. O [README.md](../README.md), linhas 20–21 e 29–30, exemplifica arquivo `.in`, como os arquivos publicados em `testes/`; a implementação trata `INPUT` como caminho de arquivo, sem depender da extensão. Os exemplos começam pelo primeiro comando e acabam no fim do arquivo ([teste01.in](../testes/teste01.in), linhas 1–20).

### Critérios de avaliação conhecidos

O [README.md](../README.md), § Testes, linhas 52–56 e 76–77, diz que **parte da nota** resulta da comparação entre a saída do programa e saídas esperadas: espaços e quebras de linha precisam coincidir exatamente. Não informa pesos, casos ocultos nem como serão aferidos tempo e espaço. A criação de pares adicionais de entrada/saída e o uso de `diff` são **sugestões** de teste ([README.md](../README.md), linhas 58–74), não entregáveis extras.

## 2. Conflitos, lacunas e decisões adotadas

As decisões desta seção foram dadas pelo usuário em 10/10/2026 e confirmadas em 19/10/2026 ou escolhidas como orientação de implementação a partir das fontes de apoio. **Elas não são novas exigências escritas no projeto.** O material atual do professor foi comparado byte a byte com os arquivos correspondentes deste projeto em 19/10/2026.

| Ponto | Evidência e leitura prudente | Impacto prático |
|---|---|---|
| **Separador de `IMP` confirmado.** | [TRABALHO.md](../TRABALHO.md), linhas 60–62 e 75–76, exige vírgula e espaço, como `1, 5`. Os cinco `.out` atuais do professor também usam `, `, por exemplo [teste01.out](../testes/teste01.out), linhas 3–4, e [teste04.out](../testes/teste04.out), linhas 1–2. | **Decisão atual do usuário:** casar exatamente com os arquivos do professor. Usar `, ` e comparar a saída diretamente, byte a byte, com os `.out` publicados. Os `.out` antigos deste projeto tinham só espaços; foram substituídos pelos arquivos atuais do professor. |
| **Remoção com e sem acerto.** | [TRABALHO.md](../TRABALHO.md), linhas 31–34, agora diz que uma ocorrência deve ser removida quando presente e uma nova versão criada em ambos os casos. [teste04.in](../testes/teste04.in), linhas 11–16, e [teste04.out](../testes/teste04.out), linhas 1–2, mostram remoções que mudam o conteúdo. | Quando há acerto, a nova versão tem uma ocorrência a menos; quando não há, o conteúdo permanece igual. A redação atual não diz mais que ambas seriam idênticas. |
| **`SUC` com versão inexistente.** | O fallback à última versão é declarado apenas para `IMP` ([TRABALHO.md](../TRABALHO.md), linhas 42–45 e 60–63). | **Decisão do usuário:** usar a versão mais recente também em `SUC`, preservando `M` original na linha que reproduz o comando. É uma convenção complementar; não atribuí-la ao texto do projeto. |
| **Alcance do método da aula.** | O texto remete a método “descrito em sala”, mas não reproduz seus passos nem especifica `next`/`prev` ([TRABALHO.md](../TRABALHO.md), linhas 3 e 12). A [videoaula, 18:23–19:05 e 24:00–29:24](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=1103s), demonstra a transformação geral por cópia de nós, com `p=1` para lista simples e `2p` registros; não fala dos comandos deste trabalho. | **Escolha técnica:** lista simples com dois registros e retorno por nó, exatamente a configuração da aula para `p=1`. Não é preciso depender da aceitação de uma variante com um registro. |
| **Domínio numérico e entradas inválidas.** | O enunciado exige valores de nó não negativos com pelo menos 16 bits, mas não fixa limite superior de `N`, gramática para linhas inválidas nem resposta a valores fora da faixa representável ([TRABALHO.md](../TRABALHO.md), linhas 8 e 22–76). | **Escolha técnica:** fazer a leitura com inteiro assinado de pelo menos 64 bits (ou precisão arbitrária). `INC N` exige `N ≥ 0`; `REM N` com `N < 0` não encontra chave e ainda cria versão; `SUC N M` com `N < 0` retorna a primeira chave, se houver. Assumir linhas bem formadas na avaliação; para linha malformada, `INC` negativo ou número fora da faixa escolhida, emitir erro em `stderr` e encerrar, sem inventar saída em `stdout`. |
| **Garantia temporal.** | O enunciado atual qualifica explicitamente o sobrecusto `O(1)` como amortizado ([TRABALHO.md](../TRABALHO.md), linhas 5–6); a [aula, 56:50–1:12:09](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=3410s) e o [artigo, § 2.3.4](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf) sustentam essa análise por alteração. | Usar a garantia amortizada do método ensinado para atualizações; cada leitura de elo histórico continua `O(1)` no pior caso. Uma operação completa na lista pode percorrer `O(n)` nós. |
| **Outras lacunas.** | Não há prazo, rubrica ou peso publicados para os critérios, nem ordem física exigida entre duplicatas. | Essas lacunas não acrescentam passos à estrutura; documentar apenas a convenção de duplicatas escolhida no § 5. |

Os exemplos publicados sugerem que `INC` e `REM` não imprimem linhas, pois os `.out` contêm apenas respostas a `SUC`/`IMP` (por exemplo [teste01.in](../testes/teste01.in) e [teste01.out](../testes/teste01.out)). Isso é evidência dos testes, não uma frase explícita do enunciado.

## 3. Ordem de estudo recomendada

| Etapa | O que dominar e por quê | Fonte de apoio específica |
|---|---|---|
| 1. Lista simples ordenada com repetições | Cabeça, `next`, predecessor/sucessor, inserção antes do primeiro `≥ N`, remoção de uma ocorrência, primeiro `> N`. É a parte não persistente de todos os comandos. | O próprio contrato em [TRABALHO.md](../TRABALHO.md), §§ Graduação e Operações; exemplo de lista ligada nas [notas MIT, figura 2, p. 4](https://courses.csail.mit.edu/6.851/spring21/scribe/lec1.pdf). |
| 2. Versões e máquina de ponteiros | Distinguir consultas ao passado de atualizações só no presente; entender que nós e campos têm tamanho constante e que uma lista simples tem grau de entrada limitado. | [Aula do professor, 0:38–4:45, 8:01–11:45 e 18:23–19:05](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=38s); MIT [vídeo L01](https://courses.csail.mit.edu/6.851/spring21/lectures/L01.html) e [notas §§ 2.1–2.2, pp. 1–3](https://courses.csail.mit.edu/6.851/spring21/scribe/lec1.pdf); Wild [vídeos 4-1](https://youtu.be/KFka3cj-7RY) e [4-4](https://youtu.be/HcdK-32bryA); [anotações pessoais](</Users/erikbayerlein/Documents/Obsidian Vault/Estrutura de Dados Avançadas.md>), linhas 7–29. |
| 3. Por que soluções óbvias não bastam | Distinguir cópia total, cópia do prefixo e *fat nodes* da transformação com slots limitados. Relacionar cada alternativa aos dois limites do trabalho. | Wild [vídeo 4-3](https://youtu.be/0mejHvK2SjQ) e [slides pp. 8–15](https://www.wild-inter.net/teaching/advds/04-persistence.pdf); [Driscoll et al., §§ 2.1–2.3, pp. 91–93](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf). |
| 4. Cópia de nós e leitura versionada | Entender campo base, até dois registros `(versão, campo, valor)` e um retorno por nó na lista simples; ler a última modificação aplicável, copiar nó cheio e reparar o elo do predecessor corrente ou a cabeça. | [Aula do professor, 21:43–49:42](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=1303s); MIT [notas § 3, pp. 3–5, figuras 2–3](https://courses.csail.mit.edu/6.851/spring21/scribe/lec1.pdf); Wild [vídeo 4-5](https://youtu.be/EbFycg7Q7V0) e [notas pp. 34–37](https://www.wild-inter.net/teaching/advds/notes-04-persistence.pdf); [anotações pessoais](</Users/erikbayerlein/Documents/Obsidian Vault/Estrutura de Dados Avançadas.md>), linhas 33–58. |
| 5. Amortização e interface do trabalho | Separar custo da busca na lista do custo da transformação; provar espaço `O(v)` e testar cabeças históricas, `REM` ausente e formatação exata. | [Aula do professor, 56:50–1:12:09](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=3410s); MIT [vídeo L01](https://courses.csail.mit.edu/6.851/spring21/lectures/L01.html); Wild [vídeo 4-6](https://youtu.be/XWryMMf5YBA) e [notas pp. 38–39](https://www.wild-inter.net/teaching/advds/notes-04-persistence.pdf); [Driscoll et al., § 2.3.4, pp. 96–97](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf); [TRABALHO.md](../TRABALHO.md), § Versões; [README.md](../README.md), § Testes. |

Não são necessários, para esta entrega, os tópicos de localização planar, retroatividade, estruturas para inteiros, árvores de busca avançadas ou persistência total que aparecem nos cursos de apoio. Sucessor aqui é uma busca linear em uma lista ordenada, não uma exigência de van Emde Boas ou árvore especializada.

## 4. Estrutura e algoritmo para esta lista

### Representação sugerida pela aula

Considere `head[0..atual]`, com `head[0] = ∅`; `head[t]` é a primeira posição da versão pública `t`. Ao começar uma atualização, a cabeça candidata é `head[t−1]`; ela só muda se a operação altera a primeira posição ou uma cópia se propaga até lá. Um nó físico da **lista simples** guarda `chave`, `nextBase`, até **dois** registros opcionais `(versão, campo, valor)` e um retorno para quem aponta para ele **na versão corrente**. A chave pode ser imutável; então o único campo que precisa de versões é `next`, e o nome do campo em cada registro pode ser implícito. O limite de dois registros e um retorno resulta de `p=1` na configuração `2p`/`p` da [aula, 24:00–29:24](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=1440s). `head[t]` é um ponteiro de acesso versionado, guardado fora dos nós, como no [artigo, § 2.3.1, pp. 93–94](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf).

A leitura `next(nó,t)` escolhe, entre os até dois registros de `next` com carimbo `≤ t`, aquele de maior versão; se nenhum serve, usa `nextBase`. A inspeção é `O(1)` por ser um conjunto de tamanho fixo. Um nó criado ou copiado em `t` só pode ser alcançado a partir de cabeças de `t` em diante. Os retornos são **auxiliares apenas para a versão mais recente** e não devem ser seguidos numa consulta histórica; é essa distinção que permite encontrar em tempo constante o predecessor a reparar, sem versionar os retornos ([aula, 25:02–27:11 e 29:29–35:12](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=1502s); [Driscoll et al., § 2.3.1, p. 94](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf)).

**Invariantes após cada comando de atualização:**

1. `head[t]` alcança exatamente as ocorrências da versão `t`, em ordem não decrescente; `head[0]` é nula.
2. Uma versão antiga nunca muda de conteúdo observável: cada consulta usa seu `head[t]` e, em cada nó, somente o elo vigente em `t`.
3. Um nó físico tem tamanho constante. Cópias recebem a mesma chave e o estado corrente dos campos; nós antigos permanecem para as versões que os alcançam.
4. Em cada nó da lista corrente que não é a cabeça, o retorno identifica exatamente seu predecessor corrente; a cabeça não tem predecessor. Ao redirecionar um elo, os retornos dos destinos afetados são atualizados.
5. A última versão é a única atualizada. Os comandos de consulta apenas percorrem a estrutura e imprimem; não alocam nós ou versões.

**Reparo de um elo `x.next ← destino` ao criar a versão `t`:**

1. Localize `x` na versão corrente e registre seu predecessor corrente pelo retorno (ou reconheça que `x` é a cabeça). Se houver registro livre em `x`, anote `(t, next, destino)` e ajuste os retornos correntes do destino antigo e do novo. O valor antigo segue acessível por consultas com versão menor que `t`.
2. Se os **dois** registros estiverem ocupados, crie `x'` com a chave de `x`, `nextBase = destino` e registros vazios. `x` permanece no histórico. Redirecione o elo do predecessor corrente de `x` para `x'` por este mesmo procedimento; caso `x` fosse a cabeça, faça `head[t] = x'`. A cópia pode, portanto, propagar-se até a cabeça.
3. Ao concluir cada redirecionamento, corrija os retornos dos nós da lista corrente. Uma implementação deve guardar o predecessor antes de alterar os retornos e tratar a criação/remoção de uma cabeça separadamente. Só publique `head[t]` depois de terminar todos os reparos.

Essa é uma descrição da transformação aplicada à lista; a [aula, 36:32–49:42](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=2192s) mostra a gravação do registro, a cópia do nó cheio, o reparo dos ponteiros que entram nele e a atualização da raiz. O [artigo, § 2.3.2, pp. 94–95](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf) detalha o processamento de uma operação inteira quando há várias alterações internas.

Exemplo com **dois registros** por nó; a terceira mudança em `A.next` provoca cópia:

```text
v0: ∅
v1: INC 2  → head[1] = A(2), A.nextBase = ∅
v2: INC 5  → head[2] = A, A.mod1 = (2, next, B(5))
v3: INC 3  → head[3] = A, A.mod2 = (3, next, C(3)), C.nextBase = B
v4: INC 3  → A está cheio; head[4] = A'(2), A'.nextBase = D(3),
              D.nextBase = C(3), C.nextBase = B(5)

IMP 1 observa 2; IMP 2 observa 2, 5; IMP 3 observa 2, 3, 5;
IMP 4 observa 2, 3, 3, 5. A continua acessível nas versões 1–3.
```

No exemplo, a pontuação de `IMP` segue **`, `**, como no enunciado e nos `.out` atuais do professor (§ 2).

### Operações

- **`INC N`**: a partir de `head[atual]`, achar o primeiro nó com chave `≥ N` (ou o fim), guardando seu predecessor. Criar um nó de chave `N` que aponta para esse sucessor. Se não há predecessor, a nova cabeça é o nó; senão, substituir de forma persistente o `next` do predecessor. Acrescentar `head[atual+1]`. Escolher antes do primeiro igual ou após o último igual não muda o multiconjunto observável; a primeira opção simplifica a busca.
- **`REM N`**: achar o primeiro nó com chave `≥ N`. Se a chave não é `N`, acrescentar nova entrada de versão com a mesma cabeça e sem tocar nos nós. Se é `N`, substituir o elo do predecessor pelo `next` do nó encontrado; se o encontrado é a cabeça, a nova cabeça é seu `next`. O nó removido continua disponível para versões antigas.
- **`SUC N M`**: escolher `versãoEscolhida = M` se existe e `atual` caso contrário, conforme decisão complementar do usuário. Percorrer `head[versãoEscolhida]` com `next(nó, versãoEscolhida)` até a primeira chave `> N`; imprimir essa chave ou `inf`. Chaves iguais a `N` são ignoradas e `N` não precisa estar na lista. Imprimir o comando original, inclusive o `M` recebido, e o resultado em linhas distintas.
- **`IMP M`**: usar `M` se existe, senão `atual`, e percorrer `next(nó, versãoEscolhida)` até o fim, imprimindo as chaves em ordem com separador **`, `** e sem separador após o último elemento. Imprimir o comando antes; se não há nós, ainda emitir uma linha vazia. Esse formato coincide com o enunciado e os `.out` atuais do professor.

Uma atualização pode ter várias mudanças internas com o **mesmo** carimbo da nova versão pública, desde que nenhuma consulta observe o estado intermediário. O artigo trata esse cuidado no processamento de uma operação inteira ([Driscoll et al., §§ 2.3.2–2.3.3, pp. 94–95](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf)). Se for mais simples numerar mudanças internas separadamente, mantenha um mapa explícito de cada versão pública ao estado interno **final**, como admite [TRABALHO.md](../TRABALHO.md), § Versões, linha 88; não exponha estados parciais por `IMP`/`SUC`.

### Custo e justificativa

Se `n` é o tamanho da versão consultada, `next(nó, versão)` leva `O(1)`, pois inspeciona um número constante de campos. Logo, `SUC` custa `O(n+1)` no pior caso; `IMP`, `O(n+1)` mais o custo de escrever os caracteres. Para atualizações, tome `n` como o tamanho da versão corrente: `INC`/`REM` localizam a posição em `O(n+1)` e alteram apenas `O(1)` elos da estrutura efêmera; a cópia pode propagar-se por até `O(n)` predecessores, todos já visitados na busca. Assim a operação completa desta lista fica `O(n+1)` no pior caso, embora **o custo de uma alteração isolada da transformação seja apenas `O(1)` amortizado, não necessariamente no pior caso**. Isso distingue a garantia geral de [Driscoll et al., § 2.3.4, pp. 96–97](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf) do custo específico da lista.

Com `p=1` predecessor por nó na lista corrente (a cabeça de acesso é guardada à parte) e **`2p=2` registros** por nó, a configuração da aula fornece `O(1)` **tempo e espaço amortizados por alteração atômica de campo**. Cada `INC` cria um nó e altera no máximo um elo existente (ou cabeça); cada `REM` com acerto altera no máximo um elo (ou cabeça). `REM` sem acerto só acrescenta uma entrada de cabeça. Como há `v−1` atualizações públicas, surgem `O(v)` nós físicos e entradas de versão ao todo, inclusive cópias propagadas. Os retornos e registros ocupam espaço constante em cada nó. Consultas ilimitadas devem ser lidas e respondidas em fluxo, sem guardar todas as linhas ou saídas, para preservar `O(v)` espaço independentemente da quantidade de consultas.

Para enxergar a amortização **da configuração ensinada**, use como potencial uma constante vezes o número de registros ocupados nos nós da versão corrente. Gravar num registro livre aumenta esse potencial em uma unidade constante. Copiar um nó cheio remove `2p` registros ocupados da parte corrente, embora possa exigir redirecionar no máximo `p` ponteiros que chegam ao nó; para a lista simples, são dois registros liberados contra no máximo um reparo de predecessor. Por isso uma atualização isolada pode copiar muitos nós, mas o custo somado das cópias é linear no número de alterações originais ([aula, 56:50–1:12:09](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=3410s); [notas de Wild, pp. 38–39](https://www.wild-inter.net/teaching/advds/notes-04-persistence.pdf)). [Driscoll et al., § 2.3.4, pp. 96–97](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf) apresentam uma análise mais geral que admite menos registros sob condições apropriadas; isso **não** muda os `2p` usados na prova da videoaula.

## 5. Decisões de implementação

| Decisão | Alternativas e impacto | Direção prudente |
|---|---|---|
| Lista simples ou dupla | Ambas podem ser parcialmente persistentes com grau de entrada constante; a dupla requer versionar dois elos e manter mais invariantes. | **Adotar lista simples:** ela satisfaz todos os comandos e é o caso `p=1` exemplificado na aula. |
| Número de registros e reparo de cópias | A [aula, 24:00–29:24](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=1440s) usa `2p` registros e até `p` retornos: nesta lista, dois e um. [Driscoll et al., §§ 2.3.3–2.3.4, pp. 95–97](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf) apresentam uma árvore com um registro e a condição geral para dispensar retornos quando o caminho de acesso inclui os predecessores; adaptar ambos à lista é uma inferência. | **Adotar dois registros e um retorno** por nó, tal como na aula. Não depender da aceitação da especialização alternativa. |
| Versões internas de uma atualização | A [aula, 21:00–22:36](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=1260s) associa versão a mudança de campo, enquanto `INC`/`REM` criam uma versão pública por comando no enunciado. Pode-se carimbar as mudanças internas com o mesmo número, se não houver conflito de leitura, ou usar números internos distintos e mapear só o estado final para a versão pública. | Garantir que `SUC`/`IMP` nunca enxerguem estado intermediário e testar operações que disparam cascata. |
| Representação de nós | Ponteiros estáveis ou índices em um reservatório crescente; endereços de objetos em vetor realocável podem invalidar elos. | Escolher uma representação de identidade estável conforme a linguagem, sem impor linguagem pelo README de exemplo. |
| Valor de `N` e parsing | A restrição de inteiro não negativo e pelo menos 16 bits se refere explicitamente aos **valores dos nós**. O projeto não define limite superior nem saída para comandos malformados. | **Adotar inteiro assinado de pelo menos 64 bits** ou precisão arbitrária, ler até EOF, recusar `INC` negativo e tratar `REM`/`SUC` negativos pela ordem matemática; erros de sintaxe ou faixa vão para `stderr`. Essa é uma política local, não um limite oficial. |
| Duplicatas | Inserir antes ou depois das iguais produz as mesmas chaves impressas e o mesmo sucessor; `REM` tira uma ocorrência. | Inserir antes da primeira chave `≥ N` e remover a primeira chave `= N`. |
| `SUC` em versão inválida | O enunciado só define o fallback para `IMP`. | **Usar a última versão**, conforme decisão do usuário, mantendo a linha do comando original na saída. |
| Formato de `IMP` | O enunciado e os cinco `.out` atuais usam vírgula seguida de espaço. | **Usar `, `** e comparar a saída diretamente com os `.out` do professor. |
| Custo da transformação | Os materiais de apoio provam custo amortizado por escrita; uma cascata isolada pode custar `O(n)`. | **Usar a análise amortizada** da aula e distinguir esse custo do percurso `O(n)` da lista (§ 4). |

## 6. Plano de implementação e casos de teste

1. **Fixar o contrato de entrada e saída:** registrar `IMP` com `, ` do enunciado e dos `.out` e fallback à última versão para `SUC` inválido por decisão do usuário. Manter separadas as origens dessas regras; usar os `.out` publicados como oráculo dos cinco casos fornecidos.
2. **Implementar primeiro as operações de lista ordenada** na versão corrente: busca de predecessor, duplicatas, remoção de uma ocorrência, sucessor estrito e percurso. Verificar ordenação depois de cada atualização.
3. **Acrescentar a tabela de cabeças e a leitura versionada de `next`**, com `head[0] = ∅`; consultar versões antigas após várias inclusões.
4. **Introduzir dois registros, retorno corrente e cópia propagada.** Testar a primeira e a segunda gravação em registros livres, a terceira com cópia de um nó e uma cascata até a cabeça. Conferir os retornos após inserção, remoção e cópia; só publicar `head[t]` após concluir a atualização.
5. **Integrar o protocolo de entrada/saída e entrega:** ler o arquivo apontado por `INPUT` até EOF, responder consultas em fluxo, escrever o `Makefile` Linux e o README exigido. Comparar a saída **byte a byte** com os cinco `.out` publicados; criar oráculos adicionais para casos que eles não cobrem.
6. **Auditar os limites:** contar nós e cabeças em sequências de até 1000 atualizações; repetir muitas consultas sem aumento de memória; executar os cinco pares publicados e exigir igualdade literal com os `.out` atuais.

| Caso de teste | Sequência ou verificação | Propriedade esperada |
|---|---|---|
| Estado inicial | `IMP 0`, `SUC 0 0` | Linha de conteúdo vazia; sucessor `inf`; versão continua 0. |
| Inserção em três posições | `INC 5`, `INC 2`, `INC 4`, `INC 9` | Versão 4 contém `2, 4, 5, 9` em ordem; versões 1–3 continuam inalteradas. |
| Zero e largura numérica | Inserir `0`, `32767` e `65535`; consultar históricos | Zero não é confundido com sentinela. `32767` cabe até num inteiro assinado de 16 bits; `65535` é coberto pela representação ampla escolhida, mas não deve ser atribuído como garantia inequívoca ao enunciado. |
| Repetidos | `INC 5` três vezes, depois `REM 5`; consultar `SUC 5 M` na versão resultante | A nova versão ainda tem duas ocorrências; `SUC 5 M` retorna `inf`, pois chaves iguais não são sucessoras estritas. |
| Remoção na cabeça, meio e cauda | Criar `2, 4, 6, 8`; remover `2`, depois `6`, depois `8` | Cada remoção altera apenas a versão nova; a versão anterior a elas conserva os quatro valores. |
| Esvaziar e remover do vazio | `INC 0`, `REM 0`, `REM 0`, `IMP 1`, `IMP 2`, `IMP 3` | A versão 1 contém `0`; as versões 2 e 3 são vazias; a última remoção ainda cria versão. |
| Remoção ausente | `INC 2`, `REM 9`, `IMP 1`, `IMP 2` | Versões 1 e 2 têm mesmo conteúdo, mas identificadores diferentes. |
| Sucessor estrito | Em uma versão `M` com `2, 5, 5, 9`, consultar `SUC 1 M`, `SUC 2 M`, `SUC 5 M`, `SUC 9 M` | Respostas `2`, `5`, `9`, `inf`; não exige que a chave consultada exista. |
| `SUC` com versão inexistente | `INC 7`, `SUC 0 99`, `SUC 7 99`, `SUC 0 -1` | Consultar a versão 1, mas reproduzir cada comando com o `M` recebido; respostas `7`, `inf` e `7`. Consultas não criam versões. |
| Limiar negativo e remoção ausente | `INC 0`, `SUC -1 1`, `REM -1`, `IMP 2` | Pela política local, `SUC -1 1` devolve `0`; `REM -1` cria a versão 2 sem alterar seu conteúdo. |
| Dois registros, cópia e histórico | `INC 2`, `INC 5`, `INC 3`, `INC 3`; depois `IMP 1` a `IMP 4` | Conteúdos `2`; `2, 5`; `2, 3, 5`; `2, 3, 3, 5`. A terceira mudança em `A.next` força cópia; as versões anteriores não mudam. |
| Cascata até a cabeça | `INC 30`, `INC 20`, `INC 10`, `INC 20`, `REM 20`, `INC 30`, `REM 30`, `INC 25`; consultar versões 3, 5, 7 e 8 | Inserir antes da primeira igual e remover a primeira igual preenche os dois registros dos nós 10 e 20, mantendo a lista `10, 20, 30`; a última inserção força cópia de 20 e depois de 10. A versão 8 é `10, 20, 25, 30`; as anteriores permanecem `10, 20, 30`. |
| Limites de versões | 1000 atualizações misturadas; consultar 0 e 1000 | `head[1000]` existe, `head[0]` permanece vazia; espaço cresce linearmente. |
| Versão inexistente em `IMP` | `INC 7`, `IMP 99` | Deve imprimir a versão mais recente, preservando `IMP 99` na linha de comando. |
| Entrada e bytes de saída | `make build`; `make run INPUT=testes/teste01.in`; comparação direta com `testes/teste01.out` | Conferir caminho `.in`, ausência de saídas extras, vírgulas, espaços e quebras de linha. Um `diff` bruto deve passar após implementar todas as operações. |
| Muitas consultas | Repetir `SUC`/`IMP` sem novas atualizações | Número de versões e nós não cresce; memória não depende da quantidade de consultas. |

Os cinco pares em [`testes/`](../testes/) cobrem inserções, remoções, consultas históricas, `REM` ausente, versão 0 e `IMP` fora do intervalo; **não** cobrem duplicatas nem `SUC` com versão inválida. O separador `, ` coincide com a regra adotada. Um oráculo simples de multiconjuntos por versão serve para comparar a semântica em testes pequenos, mas a estrutura entregue deve respeitar `O(v)` espaço.

## 7. Fontes usadas e limites de acesso

### Fontes normativas deste projeto

- [TRABALHO.md](../TRABALHO.md), abertura e §§ Graduação, Operações, Versões, linhas 1–88: estrutura, operações, limites, versionamento e formato textual de `IMP`.
- [README.md](../README.md), §§ Grupos, Envio, Entrada e saída, Testes, linhas 10–77: entrega Linux, `INPUT`, saída e comparação exata.
- [testes](../testes/) 01–05: exemplos efetivos de entrada/saída, com `, ` nas linhas de `IMP`; são casos publicados, não uma especificação substituta.

### Fontes explicativas externas e anotações

**Vídeos selecionados e evidência efetivamente acessada.** A [aula MIT L01](https://courses.csail.mit.edu/6.851/spring21/lectures/L01.html) e os cinco vídeos abaixo da [Unit 4 de Sebastian Wild](https://www.wild-inter.net/teaching/advds/04-persistence) foram processados com `watch`. Para a videoaula fornecida pelo usuário, `watch` não conseguiu baixar a mídia porque o YouTube pediu confirmação de acesso; foi possível exportar **legendas automáticas em português, com timestamps**, pela página do vídeo no navegador e conferir quadros nos pontos indicados abaixo. Portanto, as observações sobre a fala vêm das legendas, com amostras visuais de apoio; **não** houve inspeção visual contínua da gravação. Tempos aproximados identificam trechos do vídeo, não páginas do PDF. Os diagramas e as provas dos cursos de apoio também foram conferidos nos PDFs, citados à parte.

**Videoaula da disciplina indicada pelo usuário:** [“Estruturas de Dados Avançadas: 03 Persistência”](https://www.youtube.com/watch?v=ZRFl8UyXmxY), publicada em 29 jul. 2020 no canal Victor Campos, duração 1:38:45. O usuário a identifica como aula de seu professor; a página confirma título e canal, mas não identifica com segurança quem fala. Os quadros foram amostrados em 13:00, 18:30, 24:00, 31:20, 37:00, 43:00, 48:00 e 1:10:00. As legendas são geradas automaticamente e podem trocar termos e fórmulas; os pontos centrais de `p`, `2p`, cópia e amortização foram cruzados com esses quadros e com o [artigo original, § 2.3](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf).

| Trecho da videoaula | Evidência revisada e uso no guia |
|---|---|
| [0:38–4:45](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=38s) e [8:01–20:26](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=481s) | Persistência parcial, máquina de ponteiros, grau de entrada limitado e `p=1` na lista simples: §§ 3–4. |
| [21:43–35:12](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=1303s) | Exemplo de `next` em versões distintas; nó com até `p` retornos correntes e `2p` registros `(versão,campo,valor)`; leitura da última alteração aplicável: representação e `next(nó,t)` no § 4. |
| [36:32–49:42](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=2192s) | Atualizar registro livre ou copiar nó cheio, redirecionar os apontadores correntes e guardar a raiz de cada versão: reparo e invariantes no § 4. |
| [56:50–1:12:09](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=3410s) | Cópias em cascata e análise por potencial com `2p` registros; a garantia por atualização de campo é amortizada: custos e testes no §§ 4–6. |

A partir de [1:13:00](https://www.youtube.com/watch?v=ZRFl8UyXmxY&t=4380s), a aula passa à persistência total, fora do escopo da Graduação. A legenda não menciona o Trabalho 01, seus comandos nem a ordenação da lista. A descrição liga um Jamboard da aula, hoje indisponível; não foi usado como leitura ou PDF.

- **MIT 6.851, L01, “Persistence”, Erik Demaine, 1:37:54, gravação de 7 fev. 2012:** em [12:10](https://courses.csail.mit.edu/6.851/spring21/lectures/L01.html), o quadro distingue atualização apenas da última versão e histórico linear; em torno de 20:04 apresenta a transformação de uma estrutura de ponteiros com grau de entrada limitado; em 22:01–26:02 aparecem ponteiros de retorno, histórico de modificações e leitura pela última alteração aplicável; em 28:02–38:05, cópia do nó cheio, reparo recursivo e análise por potencial. A aula também trata persistência total e outros modelos, que não são necessários aqui. O vídeo foi processado inteiro com transcrição local WhisperX `small` (idioma detectado automaticamente, não verificado) e 50 quadros distribuídos pelos 97 minutos; essa amostragem é esparsa. A prova e os detalhes não visíveis nos quadros foram conferidos nas [notas de escriba, § 3, pp. 3–5](https://courses.csail.mit.edu/6.851/spring21/scribe/lec1.pdf), em vez de inferidos de um quadro isolado.

Os cinco vídeos Wild seguem a ordem 4-1, 4-3, 4-4, 4-5 e 4-6. A revisão usou legendas automáticas em inglês e quadros amostrados.

| Vídeo, duração | Evidência observada no vídeo | Uso neste guia |
|---|---|---|
| [4-1, “Types of Persistent Data Structures”, 06:10](https://youtu.be/KFka3cj-7RY) | Por volta de 00:43 contrapõe atualização efêmera e persistência; em 03:44, quadro define consultas em qualquer versão e atualizações só na última, com histórico linear. | Definição de persistência parcial na etapa 2 e na invariante de versões. |
| [4-3, “Simple approaches to persistence”, 20:58](https://youtu.be/0mejHvK2SjQ) | Em 07:55 mostra cópia integral; em 10:24, cópia propagada a predecessores; em 12:53–14:14, o custo de copiar lista dupla; em 18:08, *fat nodes* com acesso `O(log m)` por campo e espaço `O(1)` por etapa. | Comparação de alternativas e explicação de por que não copiar cada versão inteira. |
| [4-4, “Bounded in-degree linked data structures”, 13:14](https://youtu.be/HcdK-32bryA) | Em 05:10–07:09 define acesso por ponteiros e passos atômicos; em 08:09–10:03 trata grau de entrada `p` limitado; em 11:38–11:51 distingue uma atualização de alto nível das escritas internas. | Justificativa de `p=1` na lista simples e dos custos por passo. |
| [4-5, “Partial persistence”, 23:43](https://youtu.be/EbFycg7Q7V0) | A página da unidade chama o link de “Partial persistence transformation transformation”; o título reproduzido é “Partial persistence”. Em 05:07–09:59, quadros de slots constantes e leitura pela alteração aplicável; em 11:38–16:43, cópias vivas, ponteiros de retorno e entradas por versão; em 21:20–22:21, reparo posterior dos predecessores. | Representação, `next(nó,t)` e propagação de cópias na etapa 4. |
| [4-6, “Partial Persistence Analysis”, 52:13](https://youtu.be/XWryMMf5YBA) | Em 06:04–22:44 mostra atualização, conjunto de cópias e reparo recursivo; em 23:18–37:06 usa potencial dos slots ocupados para limitar cópias amortizadamente; em 40:23–40:57 aplica a transformação a uma lista dupla. | Prova de `O(v)` e distinção entre custo amortizado por escrita e tempo de percorrer a lista. |

O vídeo 4-2, “Planar Point Location via Persistence”, listado na [página da unidade](https://www.wild-inter.net/teaching/advds/04-persistence), não foi selecionado nem revisado: trata uma aplicação geométrica que não é necessária para implementar esta lista. As legendas automáticas dos vídeos selecionados podem conter erros; no 4-4 a seleção automática trouxe uma tradução alemã, e a revisão foi refeita com legendas inglesas. A amostragem de quadros do 4-6 é relativamente esparsa devido à sua duração; as notas PDF sustentam os detalhes da prova.

- Erik D. Demaine, **MIT 6.851, Lecture 1 — Temporal / Persistent Data Structures**, [página de aulas Spring 2021](https://courses.csail.mit.edu/6.851/spring21/lectures/) e [notas de escriba, Lecture 01, 11 pp.](https://courses.csail.mit.edu/6.851/spring21/scribe/lec1.pdf), §§ 2.1–3.2, pp. 1–5. A página de 2021 identifica a gravação como aula de Spring 2012; a nota distingue os dois níveis de persistência, apresenta a figura de lista ligada e analisa a cópia de nós. O PDF foi lido pelo texto extraído e pelas páginas/figuras renderizadas. As observações do vídeo estão separadas acima.
- Sebastian Wild, **CS 650, Unit 4: Persistence**, [página da unidade](https://www.wild-inter.net/teaching/advds/04-persistence), [slides, 23 pp.](https://www.wild-inter.net/teaching/advds/04-persistence.pdf), §§ 4.1, 4.3–4.6, e [notas anotadas, 42 pp.](https://www.wild-inter.net/teaching/advds/notes-04-persistence.pdf), sobretudo pp. 31–39. A aplicação de localização planar da § 4.2 e o exercício sobre grafos da Sheet 7 não são necessários para a lista do trabalho.
- James R. Driscoll, Neil Sarnak, Daniel D. Sleator e Robert E. Tarjan, [“Making Data Structures Persistent”](https://www.cs.cmu.edu/~sleator/papers/making-data-structures-persistent.pdf), *Journal of Computer and System Sciences* 38(1), 1989, pp. 86–124; §§ 2.2–2.3, especialmente pp. 93–97. Artigo original para *fat nodes*, cópia de nós, ponteiros inversos e prova amortizada. As páginas relevantes de seu PDF digitalizado foram inspecionadas visualmente.
- [Anotações pessoais, “Estrutura de Dados Avançadas”](</Users/erikbayerlein/Documents/Obsidian Vault/Estrutura de Dados Avançadas.md>), linhas 7–65: máquina de ponteiros, persistência parcial, leitura por carimbo, cópias e esboço de amortização. Servem como apoio e indício do conteúdo estudado, **não** como extensão dos requisitos do projeto.

### Lacunas que permanecem nos documentos

- O [enunciado](../TRABALHO.md) e os [`.out`](../testes/) atuais concordam no separador **`, `**. Ainda não há descrição dos casos ocultos nem garantia de que todos os detalhes de formatação estejam cobertos pelos exemplos.
- Não há limite máximo de `N` nem saída prescrita para comandos malformados ou números fora da faixa escolhida. Inteiro amplo e erro em `stderr` são decisões locais (§§ 2 e 5), não requisitos publicados.
- As fontes não dizem se a especialização de um registro seria aceita, mas a implementação recomendada usa os **dois registros e retornos da aula**. A prova vista na aula é amortizada, em acordo com o `O(1)` amortizado agora explícito no enunciado; não há exigência de custo `O(1)` no pior caso por alteração.
- A rubrica, os pesos e eventuais casos ocultos de avaliação não aparecem nos documentos disponíveis.

