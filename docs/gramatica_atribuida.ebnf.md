# Gramatica atribuida em EBNF

Convencao (Secao 3.1 do enunciado): **nao-terminais em letras minusculas** e
**terminais em letras maiusculas** (ou simbolos literais entre aspas). Os terminais
nomeados correspondem as classes de token produzidas pelo analisador lexico
(`START`, `END`, `IF`, `WHILE`, `RES`, `TRUE`, `FALSE`, `AND`, `OR`, `NOT`,
`NUMERO`, `IDENTIFICADOR`).

```ebnf
programa              ::= inicio lista_comando fim EOF ;
inicio                ::= "(" START ")" ;
fim                   ::= "(" END ")" ;

lista_comando         ::= comando lista_comando | vazio ;
comando               ::= "(" expressao ")" ;

expressao             ::= item expressao_cont ;

expressao_cont        ::= vazio
                        | NOT
                        | RES
                        | item fechamento_binario ;

fechamento_binario    ::= operador_binario
                        | IF
                        | WHILE
                        | vazio ;

item                  ::= NUMERO
                        | literal_logico
                        | IDENTIFICADOR
                        | "(" expressao ")" ;

literal_logico        ::= TRUE | FALSE ;
operador_binario      ::= operador_arit
                        | operador_rel
                        | operador_logico_bin ;
operador_arit         ::= "+" | "-" | "*" | "|" | "/" | "%" | "^" ;
operador_rel          ::= ">" | "<" | "==" | "!=" | ">=" | "<=" ;
operador_logico_bin   ::= AND | OR ;
```

Esta forma fatorada evita alternativas de `expressao` com o mesmo prefixo inicial
(`item`). A implementacao em `analisador/entrada.py` segue a mesma divisao em
funcoes: `_parse_expressao()`, `_parse_expressao_cont()`,
`_parse_fechamento_binario()` e `_parse_item()`.

Observacao: no comando especial `(V MEM)` do enunciado, `MEM` representa o nome da
memoria que esta sendo definida, nao uma palavra-chave literal.

Regras semanticas associadas:

- `IDENTIFICADOR` usado como leitura deve existir na tabela de simbolos.
- `item IDENTIFICADOR` sem operador seguinte define a variavel `IDENTIFICADOR` com
  o tipo de `item`.
- Uma variavel definida com um tipo nao pode ser redefinida com outro tipo.
- `NUMERO RES` referencia resultado anterior.
- `literal_logico` (`TRUE` ou `FALSE`) tem tipo `bool`.
- `operador_arit`/`operador_rel` exigem operandos numericos (ver calculo de sequentes);
  `/` e `%` exigem inteiros.
- `operador_logico_bin` (`AND`, `OR`) exige dois operandos `bool` e produz `bool`.
- `operador_logico_un` (`NOT`) exige um operando `bool` e produz `bool`.
- `IF` e `WHILE` exigem a primeira expressao (condicao) com tipo `bool`.
- Assembly so e gerado quando nao houver erro lexico, sintatico ou semantico.

Restricao semantica da producao vazia em `fechamento_binario`: quando a expressao
tem dois `item` e nenhum operador antes de `)`, ela so e aceita como atribuicao se
o segundo item for `IDENTIFICADOR`, correspondendo ao comando especial `(V MEM)`.
Assim, `(10 X)` e valido, mas `(10 20)` continua sendo erro sintatico.

Conjuntos usados para justificar a analise preditiva:

```text
FIRST(item) = { NUMERO, TRUE, FALSE, IDENTIFICADOR, "(" }
FIRST(expressao) = FIRST(item)
FIRST(expressao_cont) = { NOT, RES, NUMERO, TRUE, FALSE, IDENTIFICADOR, "(", vazio }
FIRST(fechamento_binario) = { "+", "-", "*", "|", "/", "%", "^",
                              ">", "<", "==", "!=", ">=", "<=",
                              AND, OR, IF, WHILE, vazio }
FOLLOW(expressao) = { ")" }
FOLLOW(expressao_cont) = { ")" }
FOLLOW(fechamento_binario) = { ")" }
```

As escolhas com `vazio` sao resolvidas pelo lookahead `)`, exatamente como a tabela
LL(1) discutida nos materiais de FIRST/FOLLOW: uma celula nao preenchida corresponde
a erro sintatico.

Literais e operadores logicos: o enunciado (Secoes 2.3, 4, 7.3, 9.1 e 10.2) exige
literais inteiros, reais e logicos, alem de operacoes aritmeticas, relacionais e
logicas. As palavras reservadas `TRUE`/`FALSE` sao os literais `bool`, e `AND`/`OR`/
`NOT` sao os operadores logicos. Todas seguem o padrao de palavras-chave em maiusculas
ja usado pela linguagem (`RES`, `START`, `END`, `IF`, `WHILE`) e, por serem
reservadas, nao podem ser usadas como nomes de memoria. Alem dos literais e
operadores logicos, `bool` tambem e produzido por operadores relacionais.
