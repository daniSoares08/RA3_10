# Sistema de regras de tipos em calculo de sequentes

Notacao: `Gamma` representa a tabela de simbolos e `R` representa o historico de resultados.

## Literais

```text
-------------------- [T-INT]
Gamma; R |- n : inteiro

-------------------- [T-REAL]
Gamma; R |- r : real

-------------------- [T-BOOL]
Gamma; R |- b : bool
```

`n` e um numero sem ponto decimal e sem expoente. `r` e um numero com ponto decimal ou expoente. `b` e um literal logico (`TRUE` ou `FALSE`).

## Variaveis e leitura de memoria

Uso de `x` como **operando** dentro de uma expressao (por exemplo `(x 1 +)`):

```text
Gamma(x) = T
-------------------- [T-VAR]
Gamma; R |- x : T
```

Se `x` for usado como operando e nao estiver em `Gamma`, ocorre erro semantico:
variavel usada antes da definicao.

A **leitura explicita de memoria** `(MEM)` (um identificador sozinho entre
parenteses) e um comando especial: retorna o valor armazenado ou `0` quando a
memoria nao foi inicializada (enunciado, "Comandos Especiais", `(MEM)`). Por isso
ela nao gera erro de uso antes da definicao:

```text
Gamma(x) = T
-------------------- [T-READ]
Gamma; R |- (x) : T

x nao pertence a Gamma
------------------------------- [T-READ-0]
Gamma; R |- (x) : inteiro
```

`[T-READ-0]` reflete o valor padrao `0` (inteiro) de uma memoria nao inicializada.
A distincao entre `[T-VAR]` (operando) e `[T-READ]/[T-READ-0]` (comando `(MEM)`) e
feita no AST: leitura isolada vira o no `LEITURA_MEM`, enquanto um identificador
usado como operando permanece `VARIAVEL`.

```text
Gamma; R |- e : T    x nao pertence a Gamma
-------------------------------------------- [T-DEF]
Gamma; R |- (e x) : T
```

```text
Gamma; R |- e : T    Gamma(x) = T
-------------------------------------------- [T-REDEF-OK]
Gamma; R |- (e x) : T
```

```text
Gamma; R |- e : T2    Gamma(x) = T1    T1 != T2
------------------------------------------------ [T-REDEF-ERRO]
Gamma; R |- (e x) : erro
```

## RES

```text
N > 0    R possui resultado N linhas antes, com tipo T
----------------------------------------------------- [T-RES]
Gamma; R |- (N RES) : T
```

`N` deve ser inteiro nao negativo no nivel lexico/sintatico, mas `N = 0` nao possui
resultado anterior correspondente, pois apontaria para o proprio comando atual.
Se `N = 0` ou se nao existir resultado anterior correspondente, ocorre erro
semantico.

## Operadores aritmeticos

```text
Gamma; R |- a : inteiro    Gamma; R |- b : inteiro
-------------------------------------------------- [T-IDIV]
Gamma; R |- (a b /) : inteiro

Gamma; R |- a : inteiro    Gamma; R |- b : inteiro
-------------------------------------------------- [T-MOD]
Gamma; R |- (a b %) : inteiro
```

`/` e `%` rejeitam operandos reais ou bool.

```text
Gamma; R |- a : numerico    Gamma; R |- b : numerico
---------------------------------------------------- [T-REAL-DIV]
Gamma; R |- (a b |) : real
```

```text
Gamma; R |- a : numerico    Gamma; R |- b : numerico
---------------------------------------------------- [T-ARIT]
Gamma; R |- (a b op) : promover(a,b)
```

`op` em `{+, -, *, ^}`. `promover(a,b)` resulta em `real` se algum operando for `real`; caso contrario, `inteiro`.

## Relacionais

```text
Gamma; R |- a : numerico    Gamma; R |- b : numerico
---------------------------------------------------- [T-REL]
Gamma; R |- (a b rel) : bool
```

`rel` em `{>, <, ==, !=, >=, <=}`.

## Operadores logicos

```text
Gamma; R |- a : bool    Gamma; R |- b : bool
-------------------------------------------- [T-AND]
Gamma; R |- (a b AND) : bool

Gamma; R |- a : bool    Gamma; R |- b : bool
-------------------------------------------- [T-OR]
Gamma; R |- (a b OR) : bool

Gamma; R |- a : bool
-------------------------------------------- [T-NOT]
Gamma; R |- (a NOT) : bool
```

`AND` e `OR` sao binarios e `NOT` e unario. Operando que nao seja `bool` (inteiro ou
real) gera erro semantico. Esses operadores atendem a exigencia do enunciado
(Secoes 7.3, 9.1 e 10.2) por operacoes logicas.

## Controle

```text
Gamma; R |- c : bool    Gamma; R |- corpo : T
---------------------------------------------------- [T-IF]
Gamma; R |- (c corpo IF) : T

Gamma; R |- c : bool    Gamma; R |- corpo : T
---------------------------------------------------- [T-WHILE]
Gamma; R |- (c corpo WHILE) : T
```

Condicao de `IF` ou `WHILE` que nao seja `bool` gera erro semantico.

## Literais logicos (resolvido)

O enunciado (Secao 2.3 e Secao 4) exige literais inteiros, reais e logicos. Os
projetos RA1-11 e RA2_12 nao definiam a sintaxe do literal logico. Esta fase adota
as palavras reservadas `TRUE` e `FALSE`, ambas com tipo `bool` pela regra `[T-BOOL]`.

A escolha segue o padrao de palavras-chave em maiusculas ja usado pela linguagem
(`RES`, `START`, `END`, `IF`, `WHILE`). Por serem reservadas, `TRUE` e `FALSE` nao
podem ser usadas como nomes de memoria (`MEM`), assim como `RES` ja nao pode. Com
isso, `bool` passa a surgir tanto de operadores relacionais quanto de literais
logicos, e pode ser inferido para uma memoria via `(TRUE MEM_NOME)`.
