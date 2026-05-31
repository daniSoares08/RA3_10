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

## Variaveis

```text
Gamma(x) = T
-------------------- [T-VAR]
Gamma; R |- x : T
```

Se `x` nao estiver em `Gamma`, ocorre erro semantico: variavel usada antes da definicao.

```text
Gamma; R |- e : T    x nao pertence a Gamma
-------------------------------------------- [T-DEF]
Gamma; R |- (e x MEM) : T
```

```text
Gamma; R |- e : T    Gamma(x) = T
-------------------------------------------- [T-REDEF-OK]
Gamma; R |- (e x MEM) : T
```

```text
Gamma; R |- e : T2    Gamma(x) = T1    T1 != T2
------------------------------------------------ [T-REDEF-ERRO]
Gamma; R |- (e x MEM) : erro
```

## RES

```text
R possui resultado N linhas antes, com tipo T
-------------------------------------------- [T-RES]
Gamma; R |- (N RES) : T
```

Se nao existir resultado anterior correspondente, ocorre erro semantico.

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
logicos, e pode ser inferido para uma memoria via `(TRUE MEM_NOME MEM)`.
