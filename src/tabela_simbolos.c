// Trabalho realizado individualmente:
// Daniel Campos Soares - daniSoares08
//
// Nome do grupo no Canvas: RA3_10

/* Construcao e validacao inicial da tabela de simbolos. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analisador_semantico.h"

static Simbolo *buscar_simbolo(ResultadoTabelaSimbolos *resultado, const char *nome) {
    size_t i;

    for (i = 0; i < resultado->quantidade_simbolos; i++) {
        if (strcmp(resultado->simbolos[i].identificador, nome) == 0) {
            return &resultado->simbolos[i];
        }
    }
    return NULL;
}

static Simbolo *criar_simbolo(
    ResultadoTabelaSimbolos *resultado,
    const char *nome,
    TipoDado tipo,
    int linha
) {
    Simbolo *simbolo;

    if (resultado->quantidade_simbolos >= MAX_SIMBOLOS) {
        adicionar_erro(
            resultado->erros_semanticos,
            &resultado->quantidade_erros_semanticos,
            linha,
            nome,
            "Quantidade maxima de simbolos excedida."
        );
        return NULL;
    }

    simbolo = &resultado->simbolos[resultado->quantidade_simbolos++];
    memset(simbolo, 0, sizeof(*simbolo));
    snprintf(simbolo->identificador, sizeof(simbolo->identificador), "%s", nome);
    snprintf(simbolo->escopo, sizeof(simbolo->escopo), "%s", "arquivo");
    simbolo->tipo = tipo;
    simbolo->linha_definicao = linha;
    simbolo->linha_ultimo_uso = linha;
    return simbolo;
}

static TipoDado tipo_numero(const char *valor) {
    if (strchr(valor, '.') != NULL || strchr(valor, 'e') != NULL || strchr(valor, 'E') != NULL) {
        return TIPO_REAL;
    }
    return TIPO_INTEIRO;
}

static TipoDado tipo_operacao(
    const char *operador,
    TipoDado esquerdo,
    TipoDado direito,
    int linha,
    ResultadoTabelaSimbolos *resultado
) {
    char causa[TAMANHO_CAUSA];

    if (strcmp(operador, ">") == 0 || strcmp(operador, "<") == 0 ||
        strcmp(operador, "==") == 0 || strcmp(operador, "!=") == 0 ||
        strcmp(operador, ">=") == 0 || strcmp(operador, "<=") == 0) {
        if (!tipo_numerico(esquerdo) || !tipo_numerico(direito)) {
            adicionar_erro(
                resultado->erros_semanticos,
                &resultado->quantidade_erros_semanticos,
                linha,
                operador,
                "Operador relacional exige operandos numericos."
            );
            return TIPO_ERRO;
        }
        return TIPO_BOOL;
    }

    if (strcmp(operador, "/") == 0 || strcmp(operador, "%") == 0) {
        if (esquerdo != TIPO_INTEIRO || direito != TIPO_INTEIRO) {
            snprintf(
                causa,
                sizeof(causa),
                "Operador %s exige operandos inteiros.",
                operador
            );
            adicionar_erro(
                resultado->erros_semanticos,
                &resultado->quantidade_erros_semanticos,
                linha,
                operador,
                causa
            );
            return TIPO_ERRO;
        }
        return TIPO_INTEIRO;
    }

    if (strcmp(operador, "|") == 0) {
        if (!tipo_numerico(esquerdo) || !tipo_numerico(direito)) {
            adicionar_erro(
                resultado->erros_semanticos,
                &resultado->quantidade_erros_semanticos,
                linha,
                operador,
                "Divisao real exige operandos numericos."
            );
            return TIPO_ERRO;
        }
        return TIPO_REAL;
    }

    if (strcmp(operador, "+") == 0 || strcmp(operador, "-") == 0 ||
        strcmp(operador, "*") == 0 || strcmp(operador, "^") == 0) {
        if (!tipo_numerico(esquerdo) || !tipo_numerico(direito)) {
            adicionar_erro(
                resultado->erros_semanticos,
                &resultado->quantidade_erros_semanticos,
                linha,
                operador,
                "Operador aritmetico exige operandos numericos."
            );
            return TIPO_ERRO;
        }
        return (esquerdo == TIPO_REAL || direito == TIPO_REAL) ? TIPO_REAL : TIPO_INTEIRO;
    }

    adicionar_erro(
        resultado->erros_semanticos,
        &resultado->quantidade_erros_semanticos,
        linha,
        operador,
        "Operador nao reconhecido."
    );
    return TIPO_ERRO;
}

static TipoDado analisar_no(
    NoAst *no,
    ResultadoTabelaSimbolos *resultado,
    size_t resultado_atual
) {
    Simbolo *simbolo;
    TipoDado esquerdo;
    TipoDado direito;
    TipoDado tipo_valor;
    long deslocamento;
    char *fim = NULL;
    char causa[TAMANHO_CAUSA];

    if (no == NULL) {
        return TIPO_DESCONHECIDO;
    }

    switch (no->tipo) {
        case NO_NUMERO:
            no->tipo_dado = tipo_numero(no->valor);
            return no->tipo_dado;

        case NO_VARIAVEL:
            simbolo = buscar_simbolo(resultado, no->valor);
            if (simbolo == NULL) {
                adicionar_erro(
                    resultado->erros_semanticos,
                    &resultado->quantidade_erros_semanticos,
                    no->linha,
                    no->valor,
                    "Variavel usada antes da definicao."
                );
                no->tipo_dado = TIPO_ERRO;
                return TIPO_ERRO;
            }
            if (simbolo->quantidade_usos < MAX_RESULTADOS) {
                simbolo->linhas_uso[simbolo->quantidade_usos++] = no->linha;
            }
            simbolo->linha_ultimo_uso = no->linha;
            no->tipo_dado = simbolo->tipo;
            return no->tipo_dado;

        case NO_ATRIBUICAO:
            tipo_valor = analisar_no(no->esquerda, resultado, resultado_atual);
            simbolo = buscar_simbolo(resultado, no->valor);
            if (simbolo == NULL) {
                criar_simbolo(resultado, no->valor, tipo_valor, no->linha);
            } else if (simbolo->tipo != tipo_valor && tipo_valor != TIPO_ERRO) {
                snprintf(
                    causa,
                    sizeof(causa),
                    "Redefinicao incompativel: variavel era %s e recebeu %s.",
                    nome_tipo_dado(simbolo->tipo),
                    nome_tipo_dado(tipo_valor)
                );
                adicionar_erro(
                    resultado->erros_semanticos,
                    &resultado->quantidade_erros_semanticos,
                    no->linha,
                    no->valor,
                    causa
                );
                tipo_valor = TIPO_ERRO;
            }
            no->tipo_dado = tipo_valor;
            return no->tipo_dado;

        case NO_RES:
            deslocamento = strtol(no->valor, &fim, 10);
            if (fim == no->valor || *fim != '\0' || deslocamento < 0) {
                adicionar_erro(
                    resultado->erros_semanticos,
                    &resultado->quantidade_erros_semanticos,
                    no->linha,
                    no->valor,
                    "RES exige N inteiro nao negativo."
                );
                no->tipo_dado = TIPO_ERRO;
                return TIPO_ERRO;
            }
            if (deslocamento == 0 || (size_t)deslocamento > resultado_atual) {
                adicionar_erro(
                    resultado->erros_semanticos,
                    &resultado->quantidade_erros_semanticos,
                    no->linha,
                    no->valor,
                    "Referencia RES nao possui resultado anterior correspondente."
                );
                no->tipo_dado = TIPO_ERRO;
                return TIPO_ERRO;
            }
            no->tipo_dado = resultado->resultados[resultado_atual - (size_t)deslocamento];
            return no->tipo_dado;

        case NO_BINARIO:
            esquerdo = analisar_no(no->esquerda, resultado, resultado_atual);
            direito = analisar_no(no->direita, resultado, resultado_atual);
            no->tipo_dado = tipo_operacao(no->operador, esquerdo, direito, no->linha, resultado);
            return no->tipo_dado;

        case NO_IF:
        case NO_WHILE:
            esquerdo = analisar_no(no->esquerda, resultado, resultado_atual);
            direito = analisar_no(no->direita, resultado, resultado_atual);
            if (esquerdo != TIPO_BOOL && esquerdo != TIPO_ERRO) {
                adicionar_erro(
                    resultado->erros_semanticos,
                    &resultado->quantidade_erros_semanticos,
                    no->linha,
                    no->tipo == NO_IF ? "IF" : "WHILE",
                    "Condicao de decisao ou repeticao deve ser bool."
                );
                no->tipo_dado = TIPO_ERRO;
                return TIPO_ERRO;
            }
            no->tipo_dado = direito;
            return no->tipo_dado;

        default:
            no->tipo_dado = TIPO_ERRO;
            return TIPO_ERRO;
    }
}

ResultadoTabelaSimbolos construirTabelaSimbolos(const ArvoreSintatica *arvore) {
    /*
     * Percorre a arvore sintatica e registra simbolos da linguagem.
     *
     * Implementar conforme regras:
     * - Registrar identificadores, tipos inferidos, escopo, linha de definicao
     *   e linhas de uso.
     * - Verificar se toda variavel foi definida antes de ser usada.
     * - Controlar os comandos especiais "(V MEM)", "(MEM)" e "(N RES)".
     * - Registrar o tipo de cada variavel no momento da definicao.
     * - Impedir uso posterior de variavel com tipo incompativel.
     * - Validar referencias a resultados anteriores feitas com RES.
     * - Gerar erros semanticos claros com linha, variavel e causa.
     * - Salvar a tabela de simbolos em Markdown, texto ou JSON.
     */
    ResultadoTabelaSimbolos resultado;
    NoAst *comando;
    TipoDado tipo;

    memset(&resultado, 0, sizeof(resultado));
    if (arvore == NULL || arvore->raiz == NULL) {
        return resultado;
    }

    comando = arvore->raiz->esquerda;
    while (comando != NULL) {
        tipo = analisar_no(comando, &resultado, resultado.quantidade_resultados);
        if (resultado.quantidade_resultados < MAX_RESULTADOS) {
            resultado.resultados[resultado.quantidade_resultados++] = tipo;
        }
        comando = comando->proximo;
    }

    return resultado;
}
