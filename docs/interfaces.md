# Interfaces entre fases

## Entrada

`prepararEntradaSemantica(const char *arquivo)` recebe o caminho do arquivo de teste.

Saida:

- vetor de `Token`;
- arvore sintatica inicial;
- lista de erros lexicos;
- lista de erros sintaticos.

## Token

Campos:

- `tipo`: `ABRE_PAREN`, `FECHA_PAREN`, `NUMERO`, `IDENTIFICADOR`, `OPERADOR_ARIT`, `OPERADOR_REL`, `RES`, `START`, `END`, `IF`, `WHILE`, `EOF`;
- `lexema`;
- `linha`;
- `coluna`.

Comentarios `*{ ... }*` sao reconhecidos pelo analisador lexico e descartados antes do parser.

## Arvore sintatica

Cada `NoAst` guarda:

- categoria sintatica;
- tipo semantico atribuido;
- valor ou operador;
- linha;
- filhos `esquerda`, `direita`, `terceiro`;
- `proximo` para comandos em sequencia.

## Tabela de simbolos

Cada simbolo registra:

- identificador;
- tipo;
- escopo;
- linha de definicao;
- linha de ultimo uso;
- lista de linhas de uso.

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
