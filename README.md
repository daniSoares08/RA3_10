# RA3_10 - Fase 3 - Analisador Semantico

## Informacoes

- Instituicao: PUCPR
- Ano: 2026
- Disciplina: Linguagens Formais e Compiladores
- Professor: Frank Coelho De Alcantara
- Trabalho realizado individualmente: Daniel Campos Soares - `daniSoares08`
- Nome do grupo no Canvas: RA3_10
- Linguagem de implementacao oficial: Python

## Como executar

O programa recebe um unico arquivo de teste por argumento e nao usa menu interativo.

```bash
python AnalisadorSemantico.py teste1_valido.txt
```

## Como testar

Execute os tres arquivos de teste da entrega:

```bash
python AnalisadorSemantico.py teste1_valido.txt
python AnalisadorSemantico.py teste2_erros_semanticos.txt
python AnalisadorSemantico.py teste3_integracao.txt
```

O segundo teste possui erros semanticos intencionais e deve retornar codigo de erro.

Para validar a sintaxe dos modulos Python:

```bash
python -m py_compile AnalisadorSemantico.py funcoes_teste_semantico.py analisador\__init__.py analisador\arquivo_saida.py analisador\artefatos.py analisador\arvore_atribuida.py analisador\assembly.py analisador\cli.py analisador\entrada.py analisador\modelos.py analisador\tabela_simbolos.py analisador\tipos.py
```

## Estrutura

- `AnalisadorSemantico.py`: ponto de entrada exigido pelo enunciado.
- `analisador/cli.py`: coordena a execucao completa.
- `analisador/entrada.py`: leitura do arquivo, lexer, tratamento de comentarios, parser e arvore sintatica inicial.
- `analisador/modelos.py`: tokens, nos da AST, erros, simbolos e resultados compartilhados.
- `analisador/tabela_simbolos.py`: tabela de simbolos, uso de variaveis, `RES` e validacao semantica principal.
- `analisador/tipos.py`: consolidacao dos tipos anotados.
- `analisador/arvore_atribuida.py`: arvore sintatica atribuida.
- `analisador/assembly.py`: geracao de Assembly ARMv7 para entradas validas.
- `analisador/artefatos.py`: gravacao dos artefatos finais.
- `docs/`: gramatica atribuida, regras de tipos e interfaces.
- `resultados/`: artefatos da ultima execucao.
- `versao_c/`: versao C preservada apenas para referencia historica e comparacao.

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
- `real`: literal numerico com ponto decimal ou expoente.
- `bool`: resultado de operadores relacionais.

O RA1-11 e o RA2_12 nao definem uma sintaxe para literais bool. Por fidelidade aos trabalhos anteriores, esta versao nao inventa palavras como `TRUE` ou `FALSE`.

## Arquivos de teste

- `teste1_valido.txt`: programa semanticamente valido.
- `teste2_erros_semanticos.txt`: erros semanticos intencionais.
- `teste3_integracao.txt`: comentarios, variaveis, expressoes aninhadas, decisoes, repeticoes e `RES`.

## Arquivos de saida

Os artefatos da ultima execucao ficam em `resultados/`:

- `arquivo_teste_final.md`
- `tabela_simbolos.md`
- `erros_semanticos.md`
- `arvore_sintatica_atribuida.md`
- `codigo_assembly_ultima_execucao.s`
- `relatorio_execucao.md`

