# RA3_10 - Fase 3 - Analisador Semantico

## Informacoes

- Instituicao: PUCPR
- Ano: 2026
- Disciplina: Linguagens Formais e Compiladores
- Professor: Frank Coelho De Alcantara
- Trabalho realizado individualmente: Daniel Campos Soares - `daniSoares08`
- Nome do grupo no Canvas: RA3_10
- Linguagem de implementacao: C

## Como compilar

Com `gcc`:

```bash
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude -o AnalisadorSemantico.exe AnalisadorSemantico.c src\arquivo_saida.c src\artefatos.c src\arvore_atribuida.c src\assembly.c src\entrada.c src\funcoes_teste_semantico.c src\tabela_simbolos.c src\tipos.c
```

Com `make`, em ambiente que tenha `make` disponivel:

```bash
make
```

## Como executar

O programa recebe um unico arquivo de teste por argumento e nao usa menu interativo.

```bash
.\AnalisadorSemantico.exe teste1_valido.txt
```

## Como depurar

Compile com simbolos de depuracao:

```bash
gcc -g -std=c11 -Wall -Wextra -pedantic -Iinclude -o AnalisadorSemantico.exe AnalisadorSemantico.c src\arquivo_saida.c src\artefatos.c src\arvore_atribuida.c src\assembly.c src\entrada.c src\funcoes_teste_semantico.c src\tabela_simbolos.c src\tipos.c
```

Depois execute com o arquivo que deseja investigar:

```bash
.\AnalisadorSemantico.exe teste2_erros_semanticos.txt
```

## Linguagem implementada

A sintaxe segue a linguagem usada no RA2_12:

- Todo programa comeca com `(START)` e termina com `(END)`.
- Comentarios comecam em `*{` e terminam em `}*`.
- Expressoes usam RPN e podem ser aninhadas.
- Variavel e definida com `(valor VAR MEM)`.
- Variavel e lida com `(VAR)`.
- Resultado anterior e lido com `(N RES)`.
- Decisao: `((condicao) (corpo) IF)`.
- Repeticao: `((condicao) (corpo) WHILE)`.

Operadores aritmeticos:

- `+`, `-`, `*`, `^`: operandos numericos.
- `/`, `%`: apenas inteiros.
- `|`: divisao real.

Operadores relacionais:

- `>`, `<`, `==`, `!=`, `>=`, `<=`.

## Tipos suportados

- `inteiro`: literal numerico sem ponto decimal e sem expoente.
- `real`: literal numerico com ponto decimal ou expoente, tratado como double IEEE 754 na geracao de Assembly.
- `bool`: resultado de operadores relacionais.

Pendente para decisao do aluno antes da entrega final: o RA1-11 e o RA2_12 nao definem uma sintaxe para literais bool. Por fidelidade aos trabalhos anteriores, esta versao ainda nao inventa palavras como `TRUE` ou `FALSE`.

## Arquivos de teste

- `teste1_valido.txt`: programa semanticamente valido.
- `teste2_erros_semanticos.txt`: erros semanticos intencionais.
- `teste3_integracao.txt`: comentarios, variaveis, expressoes aninhadas, decisoes, repeticoes e RES.

## Arquivos de saida

Os artefatos da ultima execucao ficam em `resultados/`:

- `arquivo_teste_final.md`
- `tabela_simbolos.md`
- `erros_semanticos.md`
- `arvore_sintatica_atribuida.md`
- `codigo_assembly_ultima_execucao.s`
- `relatorio_execucao.md`

## Observacao sobre o RA1 e RA2

O RA3 em C foi preparado usando o que era aproveitavel dos projetos anteriores:

- RA1-11: ideia de lexer, memoria, RES e geracao ARMv7 com double/VFP.
- RA2_12: sintaxe de programa, IF, WHILE, operadores relacionais, parser LL(1) e arvore sintatica.

O erro de nota citado no RA1/RA2 foi tratado na direcao correta: a nova geracao de Assembly usa `.double`, pilha de 8 bytes, `vldr`, `vstr`, `vadd.f64`, `vsub.f64`, `vmul.f64` e `vdiv.f64`, em vez de truncar reais para inteiros.
