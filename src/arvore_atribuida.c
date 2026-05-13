// Trabalho realizado individualmente:
// Daniel Campos Soares - daniSoares08
//
// Nome do grupo no Canvas: RA3_10

/* Geracao da arvore sintatica atribuida. */

#include <stdio.h>
#include <string.h>

#include "analisador_semantico.h"

ArvoreAtribuida gerarArvoreAtribuida(
    const ArvoreSintatica *arvore,
    const ResultadoTabelaSimbolos *tabelaSimbolos,
    const ResultadoTipos *tipos
) {
    /*
     * Produz a arvore sintatica aumentada/atribuida.
     *
     * implementar conforme as regras:
     * - Anotar cada no relevante com tipo inferido ou verificado.
     * - Anotar categoria semantica e referencias para a tabela de simbolos.
     * - Incluir informacoes necessarias para justificar a geracao de Assembly.
     * - Integrar a arvore sintatica da Fase 2 com os resultados semanticos.
     * - Salvar a arvore atribuida em Markdown, texto ou JSON.
     */
    ArvoreAtribuida resultado;

    memset(&resultado, 0, sizeof(resultado));
    resultado.raiz = arvore ? arvore->raiz : NULL;
    snprintf(
        resultado.descricao,
        sizeof(resultado.descricao),
        "Arvore atribuida com %zu simbolo(s) e %zu tipo(s) anotado(s).",
        tabelaSimbolos ? tabelaSimbolos->quantidade_simbolos : 0,
        tipos ? tipos->quantidade_tipos : 0
    );
    return resultado;
}
