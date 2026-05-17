# Sistema de regras de tipos em calculo de sequentes

Notacao: `Gamma` representa a tabela de simbolos e `R` representa o historico de resultados.

## Literais

```text
-------------------- [T-INT]
Gamma; R |- n : inteiro

-------------------- [T-REAL]
Gamma; R |- r : real
```

`n` e um numero sem ponto decimal e sem expoente. `r` e um numero com ponto decimal ou expoente.

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

## Relacionais e controle

```text
Gamma; R |- a : numerico    Gamma; R |- b : numerico
---------------------------------------------------- [T-REL]
Gamma; R |- (a b rel) : bool
```

```text
Gamma; R |- c : bool    Gamma; R |- corpo : T
---------------------------------------------------- [T-IF]
Gamma; R |- (c corpo IF) : T

Gamma; R |- c : bool    Gamma; R |- corpo : T
---------------------------------------------------- [T-WHILE]
Gamma; R |- (c corpo WHILE) : T
```

Condicao de `IF` ou `WHILE` que nao seja `bool` gera erro semantico.

## Pendente

O enunciado exige literais bool, mas os projetos RA1-11 e RA2_12 nao definem a sintaxe desses literais. Antes da entrega final, e necessario escolher e documentar essa sintaxe sem conflitar com identificadores em letras maiusculas.
