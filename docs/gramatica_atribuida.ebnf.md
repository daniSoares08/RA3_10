# Gramatica atribuida em EBNF

Convencao (Secao 3.1 do enunciado): **nao-terminais em letras minusculas** e
**terminais em letras maiusculas** (ou simbolos literais entre aspas). Os terminais
nomeados correspondem as classes de token produzidas pelo analisador lexico
(`START`, `END`, `IF`, `WHILE`, `RES`, `TRUE`, `FALSE`, `AND`, `OR`, `NOT`,
`NUMERO`, `IDENTIFICADOR`).

```ebnf
programa            ::= inicio lista_comando fim ;
inicio              ::= "(" START ")" ;
fim                 ::= "(" END ")" ;

lista_comando       ::= comando lista_comando | vazio ;
comando             ::= "(" expressao ")" ;

expressao           ::= atomo
                      | atomo atomo operador_arit
                      | atomo atomo operador_rel
                      | atomo atomo operador_logico_bin
                      | atomo operador_logico_un
                      | atomo IDENTIFICADOR
                      | NUMERO RES
                      | atomo atomo IF
                      | atomo atomo WHILE ;

atomo               ::= NUMERO
                      | literal_logico
                      | IDENTIFICADOR
                      | "(" expressao ")" ;

literal_logico      ::= TRUE | FALSE ;
operador_arit       ::= "+" | "-" | "*" | "|" | "/" | "%" | "^" ;
operador_rel        ::= ">" | "<" | "==" | "!=" | ">=" | "<=" ;
operador_logico_bin ::= AND | OR ;
operador_logico_un  ::= NOT ;
```

Observacao: no comando especial `(V MEM)` do enunciado, `MEM` representa o nome da
memoria que esta sendo definida, nao uma palavra-chave literal.

Regras semanticas associadas:

- `IDENTIFICADOR` usado como leitura deve existir na tabela de simbolos.
- `atomo IDENTIFICADOR` define a variavel `IDENTIFICADOR` com o tipo de `atomo`.
- Uma variavel definida com um tipo nao pode ser redefinida com outro tipo.
- `NUMERO RES` referencia resultado anterior.
- `literal_logico` (`TRUE` ou `FALSE`) tem tipo `bool`.
- `operador_arit`/`operador_rel` exigem operandos numericos (ver calculo de sequentes);
  `/` e `%` exigem inteiros.
- `operador_logico_bin` (`AND`, `OR`) exige dois operandos `bool` e produz `bool`.
- `operador_logico_un` (`NOT`) exige um operando `bool` e produz `bool`.
- `IF` e `WHILE` exigem a primeira expressao (condicao) com tipo `bool`.
- Assembly so e gerado quando nao houver erro lexico, sintatico ou semantico.

Literais e operadores logicos: o enunciado (Secoes 2.3, 4, 7.3, 9.1 e 10.2) exige
literais inteiros, reais e logicos, alem de operacoes aritmeticas, relacionais e
logicas. As palavras reservadas `TRUE`/`FALSE` sao os literais `bool`, e `AND`/`OR`/
`NOT` sao os operadores logicos. Todas seguem o padrao de palavras-chave em maiusculas
ja usado pela linguagem (`RES`, `START`, `END`, `IF`, `WHILE`) e, por serem
reservadas, nao podem ser usadas como nomes de memoria. Alem dos literais e
operadores logicos, `bool` tambem e produzido por operadores relacionais.
