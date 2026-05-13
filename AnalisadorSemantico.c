// Trabalho realizado individualmente:
// Daniel Campos Soares - daniSoares08
//
// Nome do grupo no Canvas: RA3_10

/*
 * Ponto de entrada exigido pelo enunciado da Fase 3.
 *
 * O programa deve ser executado por linha de comando, sem menu interativo
 *
 *     ./AnalisadorSemantico teste1_valido.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analisador_semantico.h"

static size_t copiar_erros(
    ErroAnalise *destino,
    size_t destino_capacidade,
    size_t destino_inicio,
    const ErroAnalise *origem,
    size_t quantidade_origem
) {
    size_t i;
    size_t posicao = destino_inicio;

    for (i = 0; i < quantidade_origem && posicao < destino_capacidade; i++) {
        destino[posicao] = origem[i];
        posicao++;
    }

    return posicao;
}

int main(int argc, char **argv) {
    /*
     * Coordena a execucao completa do compilador da Fase 3.
     *
     * implementar conforme regras:
     * - Receber o nome do arquivo de teste como argumento.
     * - Nao abrir menu interativo.
     * - Apresentar arquivo analisado.
     * - Apresentar resultado da analise lexica.
     * - Apresentar resultado da analise sintatica.
     * - Apresentar resultado da analise semantica.
     * - Apresentar lista de erros encontrados, se houver.
     * - Apresentar caminhos dos arquivos de saida gerados.
     */
    const char *arquivo_teste;
    EntradaSemantica entrada;
    ResultadoTabelaSimbolos tabela;
    ResultadoTipos tipos;
    ArvoreAtribuida arvore_atribuida;
    ResultadoAssembly assembly;
    ErroAnalise erros[MAX_ERROS];
    size_t quantidade_erros = 0;
    size_t i;
    int status;

    if (argc != 2) {
        printf("Uso: ./AnalisadorSemantico <arquivo_teste>\n");
        return 2;
    }

    arquivo_teste = argv[1];
    entrada = prepararEntradaSemantica(arquivo_teste);
    tabela = construirTabelaSimbolos(&entrada.arvore);
    tipos = verificarTipos(&entrada.arvore, &tabela);
    arvore_atribuida = gerarArvoreAtribuida(&entrada.arvore, &tabela, &tipos);

    quantidade_erros = copiar_erros(
        erros,
        MAX_ERROS,
        quantidade_erros,
        entrada.erros_lexicos,
        entrada.quantidade_erros_lexicos
    );
    quantidade_erros = copiar_erros(
        erros,
        MAX_ERROS,
        quantidade_erros,
        entrada.erros_sintaticos,
        entrada.quantidade_erros_sintaticos
    );
    quantidade_erros = copiar_erros(
        erros,
        MAX_ERROS,
        quantidade_erros,
        tabela.erros_semanticos,
        tabela.quantidade_erros_semanticos
    );
    quantidade_erros = copiar_erros(
        erros,
        MAX_ERROS,
        quantidade_erros,
        tipos.erros_semanticos,
        tipos.quantidade_erros_semanticos
    );

    assembly.codigo = NULL;
    if (quantidade_erros == 0) {
        assembly = gerarAssembly(&arvore_atribuida);
    }

    status = salvarArtefatosUltimaExecucao(
        arquivo_teste,
        &tabela,
        erros,
        quantidade_erros,
        &arvore_atribuida,
        &assembly
    );
    if (status != 0) {
        adicionar_erro(
            erros,
            &quantidade_erros,
            0,
            "resultados",
            "Nao foi possivel salvar todos os artefatos obrigatorios."
        );
    }

    printf("Arquivo analisado: %s\n", arquivo_teste);
    printf("Resultado da analise lexica: %zu erro(s)\n", entrada.quantidade_erros_lexicos);
    printf("Resultado da analise sintatica: %zu erro(s)\n", entrada.quantidade_erros_sintaticos);
    printf("Resultado da analise semantica: %zu erro(s) total(is)\n", quantidade_erros);
    printf("Lista de erros encontrados:\n");
    if (quantidade_erros == 0) {
        printf("- nenhum erro registrado\n");
    } else {
        for (i = 0; i < quantidade_erros; i++) {
            printf(
                "- linha %d, elemento \"%s\": %s\n",
                erros[i].linha,
                erros[i].elemento,
                erros[i].causa
            );
        }
    }
    printf("Caminhos dos arquivos de saida gerados:\n");
    formatar_caminhos_saida();

    liberarAssembly(&assembly);
    liberarEntradaSemantica(&entrada);
    return quantidade_erros == 0 ? 0 : 1;
}
