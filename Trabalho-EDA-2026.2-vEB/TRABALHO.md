# van Emde Boas com espaço linear

Implemente uma estrutura van Emde Boas com espaço linear.
Ela deve armazenar inteiros não negativos de 32 bits.

A mesma implementação deve ser feita tanto pelas equipes de graduação quanto pelas de pós-graduação.

Valores repetidos são permitidos.
Uma chave pode ser armazenada uma única vez, junto com sua quantidade de ocorrências.
A remoção de uma ocorrência não deve remover a chave enquanto houver outras.

## Espaço e tabela de dispersão

Para `n` chaves distintas presentes, a estrutura deve usar `O(n)` espaço.
Não aloque antecipadamente vetores de tamanho proporcional ao universo ou ao número de clusters possíveis nas estruturas recursivas.
Crie clusters e resumos conforme a necessidade.
Libere as estruturas vazias.

O armazenamento dos clusters deve usar uma tabela de dispersão implementada no trabalho, com:

- endereçamento aberto e tentativa linear;
- aumento da capacidade por **table doubling** e redução por **table halving**. Use limites de ocupação que evitem redimensionamentos consecutivos;
- **simple tabulation hashing**: divida a chave de 32 bits em quatro partes de 8 bits. Combine por XOR os valores de quatro tabelas de 256 valores aleatórios. Compartilhe essas tabelas entre todas as tabelas de dispersão do trabalho.

As operações da estrutura devem seguir a decomposição recursiva da van Emde Boas.
Inclusão, remoção, sucessor e predecessor devem ter tempo esperado amortizado `O(log w)`, para palavras com `w = 32` bits.
A impressão depende também da quantidade de elementos impressos.
Não é permitido substituir a estrutura ou a tabela de dispersão exigidas por estruturas prontas de biblioteca.

## Operações

Cada linha de entrada contém uma operação.
`N` é um inteiro decimal não negativo de 32 bits.
As operações de inclusão e remoção não produzem saída.

- **Inclusão:** `INC N` adiciona uma ocorrência de `N`, mesmo que a chave já esteja presente.

  ```
  INC 80
  ```

- **Remoção:** `REM N` remove apenas uma ocorrência de `N`.
  Se `N` não estiver presente, a estrutura não é alterada.

  ```
  REM 17
  ```

- **Sucessor:** `SUC N` imprime a linha da operação.
  Na linha seguinte, imprime a menor chave presente estritamente maior que `N`.
  A chave `N` não precisa estar presente.
  Se não houver sucessor, imprime `inf`.

  Exemplo de entrada:
  ```
  SUC 2
  ```

  Exemplo de saída:
  ```
  SUC 2
  5
  ```

- **Predecessor:** `PRE N` imprime a linha da operação.
  Na linha seguinte, imprime a maior chave presente estritamente menor que `N`.
  A chave `N` não precisa estar presente.
  Se não houver predecessor, imprime `-inf`.

  Exemplo de entrada:
  ```
  PRE 5
  ```

  Exemplo de saída:
  ```
  PRE 5
  2
  ```

- **Imprimir:** `IMP` imprime a linha da operação.
  Na linha seguinte, imprime todos os elementos presentes em ordem crescente, separados por vírgula e espaço.
  Imprime cada valor repetido tantas vezes quanto ele estiver presente.
  Se a estrutura estiver vazia, a segunda linha deve ficar em branco.

  Exemplo de entrada:
  ```
  IMP
  ```

  Exemplo de saída:
  ```
  IMP
  1, 5, 5, 13, 16, 21, 42, 50, 65, 80, 100
  ```

