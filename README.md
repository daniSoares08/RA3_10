# RA3_10 - Fase 3 - Analisador Semantico

## Informacoes

- Instituicao: PUCPR
- Ano: 2026
- Disciplina: Linguagens Formais e Compiladores
- Professor: Frank Coelho De Alcantara
- Integrantes do grupo (ordem alfabetica): Daniel Campos Soares - `daniSoares08`
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

As funcoes de teste da Secao 3.5 do enunciado (erros lexicos, sintaticos, semanticos,
estruturas de controle validas e invalidas, expressoes aninhadas e casos extremos)
podem ser executadas isoladamente:

```bash
python funcoes_teste_semantico.py
```

Para validar a sintaxe dos modulos Python:

```bash
python -m py_compile AnalisadorSemantico.py funcoes_teste_semantico.py analisador\__init__.py analisador\arquivo_saida.py analisador\artefatos.py analisador\arvore_atribuida.py analisador\assembly.py analisador\cli.py analisador\entrada.py analisador\modelos.py analisador\tabela_simbolos.py analisador\tipos.py
```

## Estrutura

- `AnalisadorSemantico.py`: ponto de entrada exigido pelo enunciado.
- `analisador/cli.py`: coordena a execucao completa.
- `analisador/entrada.py`: leitura do arquivo, lexer, tratamento de comentarios, parser e arvore sintatica inicial.
- `analisador/modelos.py`: tokens, nos da AST, erros, simbolos e resultados compartilhados.
- `analisador/tabela_simbolos.py`: tabela de simbolos e erros de declaracao — uso de variavel antes da definicao, redefinicao incompativel de tipo e referencias `RES`. Tambem infere e anota os tipos dos nos.
- `analisador/tipos.py`: `verificarTipos()` valida a compatibilidade de tipos nas operacoes (aritmeticas, relacionais e logicas) e nas condicoes de `IF`/`WHILE`.
- `analisador/arvore_atribuida.py`: arvore sintatica atribuida.
- `analisador/assembly.py`: geracao de Assembly ARMv7 para entradas validas.
- `analisador/artefatos.py`: gravacao dos artefatos finais.
- `docs/`: gramatica atribuida, regras de tipos e interfaces.
- `resultados/`: artefatos da ultima execucao.

## Linguagem implementada

A sintaxe segue a linguagem usada no RA2_12:

- Todo programa comeca com `(START)` e termina com `(END)`.
- Comentarios comecam em `*{` e terminam em `}*`.
- Expressoes usam RPN e podem ser aninhadas.
- Variavel e definida com `(valor VAR MEM)`.
- Variavel e lida com `(VAR)`.
- Resultado anterior e lido com `(N RES)`.
- Literais logicos: `TRUE` e `FALSE`, ambos do tipo `bool`.
- Decisao: `((condicao) (corpo) IF)`.
- Repeticao: `((condicao) (corpo) WHILE)`.

Operadores aritmeticos:

- `+`, `-`, `*`, `^`: operandos numericos.
- `/`, `%`: apenas inteiros.
- `|`: divisao real.

Operadores relacionais:

- `>`, `<`, `==`, `!=`, `>=`, `<=`: operandos numericos, resultado `bool`.

Operadores logicos:

- `AND`, `OR` (binarios): `(a b AND)`, `(a b OR)` — exigem dois operandos `bool` e resultam em `bool`.
- `NOT` (unario): `(a NOT)` — exige um operando `bool` e resulta em `bool`.

## Tipos suportados

- `inteiro`: literal numerico sem ponto decimal e sem expoente.
- `real`: literal numerico com ponto decimal ou expoente.
- `bool`: literais logicos `TRUE` e `FALSE`, alem do resultado de operadores relacionais (`>`, `<`, ...) e dos operadores logicos `AND`, `OR` e `NOT`.

O enunciado (Secao 2.3 e Secao 4) exige literais inteiros, reais e
logicos. Como RA1-11 e RA2_12 nao definiam a sintaxe do literal logico, esta fase
adota as palavras reservadas `TRUE` e `FALSE`. A escolha segue o padrao de
palavras-chave em maiusculas ja usado pela linguagem (`RES`, `START`, `END`, `IF`,
`WHILE`); por serem reservadas, `TRUE` e `FALSE` nao podem ser usadas como nomes de
memoria (`MEM`), exatamente como ja ocorre com `RES`.

## Regras para definicao e uso de variaveis

- Uma variavel e **definida** com o comando `(valor NOME MEM)`. `NOME` e formado
  apenas por letras latinas maiusculas e nao pode ser uma palavra reservada
  (`START`, `END`, `RES`, `IF`, `WHILE`, `TRUE`, `FALSE`, `AND`, `OR`, `NOT`, `MEM`).
- O **tipo** da variavel e inferido a partir do `valor` no momento da definicao.
  A tipagem e **estatica e forte**: o tipo nao muda depois de definido.
- O valor e **lido** com `(NOME)`. Usar uma variavel antes de defini-la e um erro
  semantico ("variavel usada antes da definicao").
- **Redefinir** uma variavel com um tipo incompativel (por exemplo, `inteiro`
  recebendo `bool`) e um erro semantico.
- `(N RES)` referencia o resultado do comando `N` posicoes antes do atual; `N` deve
  ser um inteiro positivo e existir um resultado correspondente.

## Exemplo semanticamente valido

```text
(START)
(10 X MEM)            *{ define X : inteiro }*
(X)                   *{ le X }*
((X 5 >) (X 2 *) IF)  *{ decisao com condicao bool (relacional) }*
(TRUE FLAG MEM)       *{ define FLAG : bool }*
((FLAG) (1 1 +) WHILE) *{ repeticao com condicao bool (memoria logica) }*
((FLAG (X 0 >) AND) (X 1 -) IF) *{ condicao com operador logico AND }*
(END)
```

Todas as operacoes usam tipos compativeis e as condicoes de `IF`/`WHILE` sao `bool`,
entao o programa passa na analise semantica e gera Assembly.

## Exemplo semanticamente invalido

```text
(START)
(Y)                   *{ erro: variavel usada antes da definicao }*
(TRUE 1 +)            *{ erro: operacao aritmetica com operando bool }*
((1 2 +) (3 4 +) IF)  *{ erro: condicao de IF nao e bool }*
(10 Z MEM)
(3.14 Z MEM)          *{ erro: redefinicao de inteiro para real }*
(END)
```

Cada linha comentada gera um erro semantico (com linha, elemento e causa) e o
programa **nao** gera Assembly.

## Tabela de simbolos

Construida por `construirTabelaSimbolos()` e salva em `resultados/tabela_simbolos.md`.
Para cada variavel registra: identificador, tipo inferido, escopo (`arquivo`, pois
cada arquivo e um escopo independente), linha de definicao e linhas de uso. E a
estrutura usada para detectar uso antes da definicao e redefinicoes incompativeis.

## Arvore sintatica atribuida

Produzida por `gerarArvoreAtribuida()` e salva em
`resultados/arvore_sintatica_atribuida.md`. Cada no traz a categoria sintatica, o
valor ou operador, a linha e o **tipo semantico inferido/verificado** (`tipo_dado`:
`inteiro`, `real`, `bool`, ...). Esses tipos justificam a geracao do Assembly, que so
ocorre quando a arvore nao possui erros.

## Arquivos de teste

- `teste1_valido.txt`: programa semanticamente valido.
- `teste2_erros_semanticos.txt`: erros semanticos intencionais.
- `teste3_integracao.txt`: comentarios, variaveis, expressoes aninhadas, decisoes, repeticoes e `RES`.

## Arquivos de saida gerados

Os artefatos da ultima execucao ficam em `resultados/`, e cada um cobre uma parte da
analise:

- `arquivo_teste_final.md`: indica qual arquivo de teste gerou os artefatos.
- `tabela_simbolos.md`: a tabela de simbolos (identificadores, tipos, escopo, linhas).
- `erros_semanticos.md`: relatorio de erros semanticos (vazio quando nao ha erros).
- `arvore_sintatica_atribuida.md`: a arvore sintatica com os tipos anotados em cada no.
- `codigo_assembly_ultima_execucao.s`: o Assembly ARMv7 gerado, apenas quando o
  programa nao tem erros lexicos, sintaticos ou semanticos.
- `relatorio_execucao.md`: resumo da execucao (arquivo, total de simbolos e de erros).

