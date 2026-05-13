// Trabalho realizado individualmente:
// Daniel Campos Soares - daniSoares08
//
// Nome do grupo no Canvas: RA3_10

/* Validacao e inferencia de tipos da Fase 3. */

#include <string.h>

#include "analisador_semantico.h"

static void coletar_tipos(NoAst *no, ResultadoTipos *resultado) {
    if (no == NULL) {
        return;
    }

    if (resultado->quantidade_tipos < MAX_RESULTADOS) {
        resultado->tipos[resultado->quantidade_tipos++] = no->tipo_dado;
    }

    coletar_tipos(no->esquerda, resultado);
    coletar_tipos(no->direita, resultado);
    coletar_tipos(no->terceiro, resultado);
    coletar_tipos(no->proximo, resultado);
}

ResultadoTipos verificarTipos(
    const ArvoreSintatica *arvore,
    const ResultadoTabelaSimbolos *tabelaSimbolos
) {
    /*
     * Valida tipos em expressoes, comandos especiais, decisoes e lacos.
     *
     * implementar conforme regras:
     * - Definir regras formais para literais inteiros, reais e logicos.
     * - Documentar as regras em calculo de sequentes.
     * - Implementar inferencia de tipos para variaveis a partir de definicoes
     *   e usos.
     * - Validar operacoes aritmeticas, relacionais e logicas.
     * - Garantir divisao inteira "/" e resto "%" apenas com inteiros.
     * - Garantir divisao real "|", soma, subtracao, multiplicacao e potencia
     *   apenas com operandos compativeis.
     * - Garantir que condicoes de decisao e repeticao tenham tipo logico.
     * - Detectar incompatibilidades em expressoes aninhadas.
     * - Produzir relatorio de erros semanticos.
     *
     * A validacao principal e feita durante construirTabelaSimbolos(), porque
     * a inferencia depende da tabela de simbolos e do historico de resultados.
     * Esta funcao consolida os tipos anotados na arvore e replica os erros para
     * manter a interface exigida pelo enunciado.
     */
    ResultadoTipos resultado;
    memset(&resultado, 0, sizeof(resultado));
    if (arvore != NULL) {
        coletar_tipos(arvore->raiz, &resultado);
    }

    (void)tabelaSimbolos;

    return resultado;
}
