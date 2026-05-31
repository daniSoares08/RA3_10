# Interfaces entre fases

## Entrada

`prepararEntradaSemantica(arquivo)` recebe o caminho do arquivo de teste.

Saida:

- vetor de `Token` (`EntradaSemantica.tokens`, usado pelo parser);
- lista de tokens de comentario reconhecidos e descartados do vetor do parser;
- arvore sintatica inicial;
- lista de erros lexicos;
- lista de erros sintaticos.

## Token

Campos:

- `tipo`: `ABRE_PAREN`, `FECHA_PAREN`, `COMENTARIO`, `NUMERO`, `LOGICO`, `IDENTIFICADOR`, `OPERADOR_ARIT`, `OPERADOR_REL`, `OPERADOR_LOGICO`, `RES`, `START`, `END`, `IF`, `WHILE`, `EOF`;
- `lexema`;
- `linha`;
- `coluna`.

Comentarios `*{ ... }*` sao reconhecidos como tokens `COMENTARIO`, armazenados em `EntradaSemantica.comentarios` e descartados antes do vetor `EntradaSemantica.tokens` usado pelo parser.

## Arvore sintatica

Cada `NoAst` guarda:

- categoria sintatica;
- tipo semantico atribuido;
- valor ou operador;
- linha;
- filhos `esquerda`, `direita`, `terceiro`;
- `proximo` para comandos em sequencia.

Estas estruturas sao `dataclasses` em `analisador/modelos.py`.

## Tabela de simbolos

Cada simbolo registra:

- identificador;
- tipo;
- escopo;
- linha de definicao;
- linha de ultimo uso;
- lista de linhas de uso.

## Semantica

A analise semantica e dividida em duas funcoes, conforme a Secao 7 do enunciado:

- `construirTabelaSimbolos(arvore)` (Secao 7.2): infere e anota os tipos dos nos,
  monta a tabela de simbolos e reporta os erros de declaracao — uso de variavel antes
  da definicao, redefinicao incompativel de tipo e referencias `RES` invalidas.
- `verificarTipos(arvore, tabelaSimbolos)` (Secao 7.3): percorre a arvore ja anotada e
  reporta os erros de compatibilidade de tipos das operacoes (aritmeticas, relacionais,
  logicas `AND`/`OR`/`NOT`, divisao inteira/real e resto) e das condicoes de
  `IF`/`WHILE`, usando as regras do calculo de sequentes
  (`docs/regras_tipos_sequentes.md`).

A regra de cada operacao fica centralizada em `avaliar_operacao()` (binaria:
aritmetica, relacional e logica `AND`/`OR`) e `avaliar_operacao_unaria()` (logica
`NOT`), ambas em `analisador/modelos.py`, compartilhadas pelas duas funcoes para
evitar divergencia. O `cli.py` combina os dois conjuntos de erros semanticos e os
ordena por linha.

## Erros

Cada erro registra:

- linha;
- elemento envolvido;
- causa.

## Artefatos

`salvarArtefatosUltimaExecucao()` grava:

- `resultados/arquivo_teste_final.md`;
- `resultados/tabela_simbolos.md`;
- `resultados/erros_semanticos.md`;
- `resultados/arvore_sintatica_atribuida.md`;
- `resultados/codigo_assembly_ultima_execucao.s`;
- `resultados/relatorio_execucao.md`.
