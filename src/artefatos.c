// Trabalho realizado individualmente:
// Daniel Campos Soares - daniSoares08
//
// Nome do grupo no Canvas: RA3_10

/* Persistencia dos artefatos obrigatorios da ultima execucao. */

#include <stdio.h>
#include <stdlib.h>

#include "analisador_semantico.h"

static int criar_diretorio_resultados(void) {
#ifdef _WIN32
    return system("if not exist resultados mkdir resultados");
#else
    return system("mkdir -p resultados");
#endif
}

static int escrever_texto(const char *caminho, const char *texto) {
    FILE *arquivo = fopen(caminho, "w");

    if (arquivo == NULL) {
        return 1;
    }

    fputs(texto ? texto : "", arquivo);
    fclose(arquivo);
    return 0;
}

static int escrever_tabela(const ResultadoTabelaSimbolos *tabela_simbolos) {
    FILE *arquivo = fopen("resultados/tabela_simbolos.md", "w");
    size_t i;
    size_t j;

    if (arquivo == NULL) {
        return 1;
    }

    fprintf(arquivo, "# Tabela de simbolos\n\n");
    fprintf(arquivo, "| Identificador | Tipo | Escopo | Linha definicao | Linhas de uso |\n");
    fprintf(arquivo, "|---|---|---|---:|---|\n");
    for (i = 0; i < tabela_simbolos->quantidade_simbolos; i++) {
        const Simbolo *s = &tabela_simbolos->simbolos[i];
        fprintf(
            arquivo,
            "| %s | %s | %s | %d | ",
            s->identificador,
            nome_tipo_dado(s->tipo),
            s->escopo,
            s->linha_definicao
        );
        if (s->quantidade_usos == 0) {
            fprintf(arquivo, "-");
        } else {
            for (j = 0; j < s->quantidade_usos; j++) {
                fprintf(arquivo, "%s%d", j == 0 ? "" : ", ", s->linhas_uso[j]);
            }
        }
        fprintf(arquivo, " |\n");
    }
    fclose(arquivo);
    return 0;
}

static int escrever_erros(const ErroAnalise *erros, size_t quantidade_erros_semanticos) {
    FILE *arquivo = fopen("resultados/erros_semanticos.md", "w");
    size_t i;

    if (arquivo == NULL) {
        return 1;
    }

    fprintf(arquivo, "# Erros semanticos\n\n");
    if (quantidade_erros_semanticos == 0) {
        fprintf(arquivo, "Nenhum erro semantico registrado.\n");
    } else {
        for (i = 0; i < quantidade_erros_semanticos; i++) {
            fprintf(
                arquivo,
                "- linha %d, elemento `%s`: %s\n",
                erros[i].linha,
                erros[i].elemento,
                erros[i].causa
            );
        }
    }

    fclose(arquivo);
    return 0;
}

static void escrever_no(FILE *arquivo, const NoAst *no, int nivel) {
    int i;

    if (no == NULL) {
        return;
    }

    for (i = 0; i < nivel; i++) {
        fprintf(arquivo, "  ");
    }
    fprintf(
        arquivo,
        "- %s valor=`%s` operador=`%s` tipo=%s linha=%d\n",
        nome_tipo_no(no->tipo),
        no->valor,
        no->operador,
        nome_tipo_dado(no->tipo_dado),
        no->linha
    );

    escrever_no(arquivo, no->esquerda, nivel + 1);
    escrever_no(arquivo, no->direita, nivel + 1);
    escrever_no(arquivo, no->terceiro, nivel + 1);
    escrever_no(arquivo, no->proximo, nivel);
}

static int escrever_arvore(const ArvoreAtribuida *arvore_atribuida) {
    FILE *arquivo = fopen("resultados/arvore_sintatica_atribuida.md", "w");

    if (arquivo == NULL) {
        return 1;
    }

    fprintf(arquivo, "# Arvore sintatica atribuida\n\n");
    fprintf(arquivo, "%s\n\n", arvore_atribuida->descricao);
    escrever_no(arquivo, arvore_atribuida->raiz, 0);
    fclose(arquivo);
    return 0;
}

int salvarArtefatosUltimaExecucao(
    const char *arquivo_teste,
    const ResultadoTabelaSimbolos *tabela_simbolos,
    const ErroAnalise *erros_semanticos,
    size_t quantidade_erros_semanticos,
    const ArvoreAtribuida *arvore_atribuida,
    const ResultadoAssembly *assembly
) {
    /*
     * Salva os artefatos finais exigidos pelo enunciado.
     *
     * IMPLEMENTAR/COMPLETAR conforme as regras do trabalho:
     * - Tabela de simbolos da ultima execucao.
     * - Relatorio de erros semanticos, mesmo vazio.
     * - Arvore sintatica atribuida da ultima execucao.
     * - Assembly da ultima execucao semanticamente valida.
     * - Indicacao explicita do arquivo de teste usado nos artefatos finais.
     */
    FILE *arquivo;
    int falhou = 0;

    if (criar_diretorio_resultados() != 0) {
        return 1;
    }

    arquivo = fopen("resultados/arquivo_teste_final.md", "w");
    if (arquivo == NULL) {
        falhou = 1;
    } else {
        fprintf(arquivo, "# Arquivo de teste final\n\n%s\n", arquivo_teste);
        fclose(arquivo);
    }

    falhou |= escrever_tabela(tabela_simbolos);
    falhou |= escrever_erros(erros_semanticos, quantidade_erros_semanticos);
    falhou |= escrever_arvore(arvore_atribuida);
    falhou |= escrever_texto(
        "resultados/codigo_assembly_ultima_execucao.s",
        assembly && assembly->codigo
            ? assembly->codigo
            : "@ GERADO: nenhum Assembly disponivel nesta execucao.\n"
    );

    arquivo = fopen("resultados/relatorio_execucao.md", "w");
    if (arquivo == NULL) {
        falhou = 1;
    } else {
        fprintf(arquivo, "# Relatorio final de execucao\n\n");
        fprintf(arquivo, "- Arquivo analisado: `%s`\n", arquivo_teste);
        fprintf(arquivo, "- Simbolos registrados: %zu\n", tabela_simbolos->quantidade_simbolos);
        fprintf(arquivo, "- Erros registrados: %zu\n", quantidade_erros_semanticos);
        fclose(arquivo);
    }

    return falhou;
}
