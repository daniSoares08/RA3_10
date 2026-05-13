# Gramatica atribuida em EBNF

Nao-terminais usam letras minusculas. Terminais usam letras maiusculas.

```ebnf
programa       ::= START_CMD lista_cmd END_CMD ;
START_CMD      ::= "(" START ")" ;
END_CMD        ::= "(" END ")" ;

lista_cmd      ::= comando lista_cmd | vazio ;
comando        ::= "(" expressao ")" ;

expressao      ::= atomo
                 | atomo atomo OPERADOR_ARIT
                 | atomo atomo OPERADOR_REL
                 | atomo IDENTIFICADOR MEM
                 | NUMERO RES
                 | expressao expressao IF
                 | expressao expressao WHILE ;

atomo          ::= NUMERO
                 | IDENTIFICADOR
                 | "(" expressao ")" ;

OPERADOR_ARIT  ::= "+" | "-" | "*" | "|" | "/" | "%" | "^" ;
OPERADOR_REL   ::= ">" | "<" | "==" | "!=" | ">=" | "<=" ;
```

Regras semanticas associadas:

- `IDENTIFICADOR` usado como leitura deve existir na tabela de simbolos.
- `atomo IDENTIFICADOR MEM` define a variavel `IDENTIFICADOR` com o tipo de `atomo`.
- Uma variavel definida com um tipo nao pode ser redefinida com outro tipo.
- `NUMERO RES` referencia resultado anterior.
- `IF` e `WHILE` exigem primeira expressao com tipo `bool`.
- Assembly so e gerado quando nao houver erro lexico, sintatico ou semantico.

Ponto pendente por falta de definicao nos RA1/RA2: literal bool explicito. Atualmente `bool` e produzido por operadores relacionais.
