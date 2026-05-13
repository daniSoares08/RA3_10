// Trabalho realizado individualmente:
// Daniel Campos Soares - daniSoares08
//
// Nome do grupo no Canvas: RA3_10

/* Geracao de Assembly para Cpulator-ARMv7 DEC1-SOC(v16.1). */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analisador_semantico.h"

typedef struct {
    char *texto;
    size_t tamanho;
    size_t capacidade;
} Buffer;

static int buffer_garantir(Buffer *buffer, size_t extra) {
    size_t nova_capacidade;
    char *novo;

    if (buffer->tamanho + extra + 1 <= buffer->capacidade) {
        return 0;
    }

    nova_capacidade = buffer->capacidade == 0 ? 4096 : buffer->capacidade;
    while (nova_capacidade < buffer->tamanho + extra + 1) {
        nova_capacidade *= 2;
    }

    novo = (char *)realloc(buffer->texto, nova_capacidade);
    if (novo == NULL) {
        return 1;
    }
    buffer->texto = novo;
    buffer->capacidade = nova_capacidade;
    return 0;
}

static int buffer_appendf(Buffer *buffer, const char *formato, ...) {
    va_list args;
    va_list copia;
    int necessario;

    va_start(args, formato);
    va_copy(copia, args);
    necessario = vsnprintf(NULL, 0, formato, copia);
    va_end(copia);
    if (necessario < 0) {
        va_end(args);
        return 1;
    }
    if (buffer_garantir(buffer, (size_t)necessario) != 0) {
        va_end(args);
        return 1;
    }
    vsnprintf(buffer->texto + buffer->tamanho, buffer->capacidade - buffer->tamanho, formato, args);
    buffer->tamanho += (size_t)necessario;
    va_end(args);
    return 0;
}

static int variavel_ja_coletada(char nomes[][TAMANHO_LEXEMA], size_t quantidade, const char *nome) {
    size_t i;

    for (i = 0; i < quantidade; i++) {
        if (strcmp(nomes[i], nome) == 0) {
            return 1;
        }
    }
    return 0;
}

static void coletar_variaveis(NoAst *no, char nomes[][TAMANHO_LEXEMA], size_t *quantidade) {
    if (no == NULL) {
        return;
    }

    if ((no->tipo == NO_ATRIBUICAO || no->tipo == NO_VARIAVEL) &&
        no->valor[0] != '\0' &&
        !variavel_ja_coletada(nomes, *quantidade, no->valor) &&
        *quantidade < MAX_SIMBOLOS) {
        snprintf(nomes[*quantidade], TAMANHO_LEXEMA, "%s", no->valor);
        (*quantidade)++;
    }

    coletar_variaveis(no->esquerda, nomes, quantidade);
    coletar_variaveis(no->direita, nomes, quantidade);
    coletar_variaveis(no->terceiro, nomes, quantidade);
    coletar_variaveis(no->proximo, nomes, quantidade);
}

static void emitir_rotinas(Buffer *texto) {
    buffer_appendf(
        texto,
        "\n"
        "push_d0:\n"
        "    ldr r1, =pilha_topo\n"
        "    ldr r2, [r1]\n"
        "    ldr r3, =pilha_valores\n"
        "    add r3, r3, r2\n"
        "    vstr d0, [r3]\n"
        "    add r2, r2, #8\n"
        "    str r2, [r1]\n"
        "    bx lr\n\n"
        "pop_d0:\n"
        "    ldr r1, =pilha_topo\n"
        "    ldr r2, [r1]\n"
        "    cmp r2, #0\n"
        "    beq pop_d0_zero\n"
        "    sub r2, r2, #8\n"
        "    str r2, [r1]\n"
        "    ldr r3, =pilha_valores\n"
        "    add r3, r3, r2\n"
        "    vldr d0, [r3]\n"
        "    bx lr\n"
        "pop_d0_zero:\n"
        "    ldr r0, =const_zero_double\n"
        "    vldr d0, [r0]\n"
        "    bx lr\n\n"
        "pop_d1:\n"
        "    bl pop_d0\n"
        "    vmov.f64 d1, d0\n"
        "    bx lr\n\n"
        "bool_from_r0:\n"
        "    cmp r0, #0\n"
        "    beq bool_false\n"
        "    ldr r0, =const_one_double\n"
        "    vldr d0, [r0]\n"
        "    bx lr\n"
        "bool_false:\n"
        "    ldr r0, =const_zero_double\n"
        "    vldr d0, [r0]\n"
        "    bx lr\n"
    );
}

static void emitir_divisao_inteira(Buffer *texto) {
    buffer_appendf(
        texto,
        "\n"
        "dividir_inteiros_assinados:\n"
        "    mov r2, #0\n"
        "    mov r3, #0\n"
        "    cmp r0, #0\n"
        "    bge div_dividendo_ok\n"
        "    rsb r0, r0, #0\n"
        "    eor r2, r2, #1\n"
        "    mov r3, #1\n"
        "div_dividendo_ok:\n"
        "    cmp r1, #0\n"
        "    bge div_divisor_ok\n"
        "    rsb r1, r1, #0\n"
        "    eor r2, r2, #1\n"
        "div_divisor_ok:\n"
        "    mov r12, #0\n"
        "div_loop:\n"
        "    cmp r0, r1\n"
        "    blt div_fim_loop\n"
        "    sub r0, r0, r1\n"
        "    add r12, r12, #1\n"
        "    b div_loop\n"
        "div_fim_loop:\n"
        "    cmp r2, #0\n"
        "    beq div_sinal_quociente_ok\n"
        "    rsb r12, r12, #0\n"
        "div_sinal_quociente_ok:\n"
        "    cmp r3, #0\n"
        "    beq div_sinal_resto_ok\n"
        "    rsb r0, r0, #0\n"
        "div_sinal_resto_ok:\n"
        "    mov r2, r12\n"
        "    mov r3, r0\n"
        "    bx lr\n"
    );
}

static void emitir_no(NoAst *no, Buffer *data, Buffer *texto, int *constante, int *label);

static void emitir_operador(const char *op, Buffer *texto, int *label) {
    int id;

    buffer_appendf(texto, "    bl pop_d1\n");
    buffer_appendf(texto, "    bl pop_d0\n");

    if (strcmp(op, "+") == 0) {
        buffer_appendf(texto, "    vadd.f64 d0, d0, d1\n    bl push_d0\n");
    } else if (strcmp(op, "-") == 0) {
        buffer_appendf(texto, "    vsub.f64 d0, d0, d1\n    bl push_d0\n");
    } else if (strcmp(op, "*") == 0) {
        buffer_appendf(texto, "    vmul.f64 d0, d0, d1\n    bl push_d0\n");
    } else if (strcmp(op, "|") == 0) {
        buffer_appendf(texto, "    vdiv.f64 d0, d0, d1\n    bl push_d0\n");
    } else if (strcmp(op, "/") == 0 || strcmp(op, "%") == 0) {
        buffer_appendf(
            texto,
            "    vcvt.s32.f64 s0, d0\n"
            "    vcvt.s32.f64 s1, d1\n"
            "    vmov r0, s0\n"
            "    vmov r1, s1\n"
            "    cmp r1, #0\n"
            "    beq int_div_zero_%d\n"
            "    bl dividir_inteiros_assinados\n",
            *label
        );
        if (strcmp(op, "/") == 0) {
            buffer_appendf(texto, "    vmov s0, r2\n");
        } else {
            buffer_appendf(texto, "    vmov s0, r3\n");
        }
        buffer_appendf(
            texto,
            "    vcvt.f64.s32 d0, s0\n"
            "    bl push_d0\n"
            "    b int_div_fim_%d\n"
            "int_div_zero_%d:\n"
            "    ldr r0, =const_zero_double\n"
            "    vldr d0, [r0]\n"
            "    bl push_d0\n"
            "int_div_fim_%d:\n",
            *label,
            *label,
            *label
        );
        (*label)++;
    } else if (strcmp(op, "^") == 0) {
        id = (*label)++;
        buffer_appendf(
            texto,
            "    vcvt.s32.f64 s0, d1\n"
            "    vmov r0, s0\n"
            "    ldr r1, =temp_pow_base\n"
            "    vstr d0, [r1]\n"
            "    ldr r1, =const_one_double\n"
            "    vldr d0, [r1]\n"
            "pow_loop_%d:\n"
            "    cmp r0, #0\n"
            "    beq pow_fim_%d\n"
            "    ldr r1, =temp_pow_base\n"
            "    vldr d2, [r1]\n"
            "    vmul.f64 d0, d0, d2\n"
            "    sub r0, r0, #1\n"
            "    b pow_loop_%d\n"
            "pow_fim_%d:\n"
            "    bl push_d0\n",
            id,
            id,
            id,
            id
        );
    } else {
        id = (*label)++;
        buffer_appendf(texto, "    vcmp.f64 d0, d1\n    vmrs APSR_nzcv, FPSCR\n");
        if (strcmp(op, ">") == 0) {
            buffer_appendf(texto, "    bgt cmp_true_%d\n", id);
        } else if (strcmp(op, "<") == 0) {
            buffer_appendf(texto, "    blt cmp_true_%d\n", id);
        } else if (strcmp(op, "==") == 0) {
            buffer_appendf(texto, "    beq cmp_true_%d\n", id);
        } else if (strcmp(op, "!=") == 0) {
            buffer_appendf(texto, "    bne cmp_true_%d\n", id);
        } else if (strcmp(op, ">=") == 0) {
            buffer_appendf(texto, "    bge cmp_true_%d\n", id);
        } else if (strcmp(op, "<=") == 0) {
            buffer_appendf(texto, "    ble cmp_true_%d\n", id);
        }
        buffer_appendf(
            texto,
            "    mov r0, #0\n"
            "    b cmp_done_%d\n"
            "cmp_true_%d:\n"
            "    mov r0, #1\n"
            "cmp_done_%d:\n"
            "    bl bool_from_r0\n"
            "    bl push_d0\n",
            id,
            id,
            id
        );
    }
}

static void emitir_no(NoAst *no, Buffer *data, Buffer *texto, int *constante, int *label) {
    int id;

    if (no == NULL) {
        return;
    }

    switch (no->tipo) {
        case NO_NUMERO:
            buffer_appendf(data, "const_%d: .double %s\n", *constante, no->valor);
            buffer_appendf(texto, "    ldr r0, =const_%d\n    vldr d0, [r0]\n    bl push_d0\n", *constante);
            (*constante)++;
            break;

        case NO_VARIAVEL:
            buffer_appendf(texto, "    ldr r0, =mem_%s\n    vldr d0, [r0]\n    bl push_d0\n", no->valor);
            break;

        case NO_ATRIBUICAO:
            emitir_no(no->esquerda, data, texto, constante, label);
            buffer_appendf(texto, "    bl pop_d0\n    ldr r0, =mem_%s\n    vstr d0, [r0]\n    bl push_d0\n", no->valor);
            break;

        case NO_RES:
            buffer_appendf(texto, "    @ RES %s: valor recuperado dos resultados anteriores deve ser ligado ao historico\n", no->valor);
            buffer_appendf(texto, "    ldr r0, =const_zero_double\n    vldr d0, [r0]\n    bl push_d0\n");
            break;

        case NO_BINARIO:
            emitir_no(no->esquerda, data, texto, constante, label);
            emitir_no(no->direita, data, texto, constante, label);
            emitir_operador(no->operador, texto, label);
            break;

        case NO_IF:
            id = (*label)++;
            emitir_no(no->esquerda, data, texto, constante, label);
            buffer_appendf(
                texto,
                "    bl pop_d0\n"
                "    ldr r0, =const_zero_double\n"
                "    vldr d1, [r0]\n"
                "    vcmp.f64 d0, d1\n"
                "    vmrs APSR_nzcv, FPSCR\n"
                "    beq if_fim_%d\n",
                id
            );
            emitir_no(no->direita, data, texto, constante, label);
            buffer_appendf(texto, "if_fim_%d:\n", id);
            break;

        case NO_WHILE:
            id = (*label)++;
            buffer_appendf(texto, "while_ini_%d:\n", id);
            emitir_no(no->esquerda, data, texto, constante, label);
            buffer_appendf(
                texto,
                "    bl pop_d0\n"
                "    ldr r0, =const_zero_double\n"
                "    vldr d1, [r0]\n"
                "    vcmp.f64 d0, d1\n"
                "    vmrs APSR_nzcv, FPSCR\n"
                "    beq while_fim_%d\n",
                id
            );
            emitir_no(no->direita, data, texto, constante, label);
            buffer_appendf(texto, "    b while_ini_%d\nwhile_fim_%d:\n", id, id);
            break;

        default:
            buffer_appendf(texto, "    @ no invalido ignorado na geracao de Assembly\n");
            break;
    }
}

ResultadoAssembly gerarAssembly(const ArvoreAtribuida *arvoreAtribuida) {
    /*
     * Gera Assembly a partir da arvore sintatica atribuida.
     *
     * implementar conforme as regras:
     * - Gerar codigo Assembly compativel com Cpulator-ARMv7 DEC1-SOC(v16.1).
     * - Gerar Assembly diretamente da arvore sintatica atribuida.
     * - Nao usar representacao intermediaria.
     * - Gerar Assembly apenas para programas sem erros lexicos, sintaticos ou
     *   semanticos.
     * - Garantir compatibilidade entre o Assembly e as anotacoes da arvore.
     * - Salvar o Assembly correspondente a ultima execucao semanticamente valida.
     */
    ResultadoAssembly resultado;
    Buffer data = {0};
    Buffer texto = {0};
    char variaveis[MAX_SIMBOLOS][TAMANHO_LEXEMA];
    size_t quantidade_variaveis = 0;
    size_t i;
    int constante = 0;
    int label = 0;
    NoAst *comando;

    resultado.codigo = NULL;
    if (arvoreAtribuida == NULL || arvoreAtribuida->raiz == NULL) {
        return resultado;
    }

    coletar_variaveis(arvoreAtribuida->raiz, variaveis, &quantidade_variaveis);

    buffer_appendf(&data, ".data\n.align 3\n");
    buffer_appendf(&data, "const_zero_double: .double 0.0\n");
    buffer_appendf(&data, "const_one_double: .double 1.0\n");
    buffer_appendf(&data, "temp_pow_base: .double 0.0\n");
    for (i = 0; i < quantidade_variaveis; i++) {
        buffer_appendf(&data, "mem_%s: .double 0.0\n", variaveis[i]);
    }
    buffer_appendf(&data, "pilha_topo: .word 0\n");
    buffer_appendf(&data, "pilha_valores: .space 4096\n\n");

    buffer_appendf(
        &texto,
        ".syntax unified\n"
        ".fpu vfpv3\n"
        ".global _start\n"
        ".text\n"
        "_start:\n"
        "    ldr r0, =pilha_topo\n"
        "    mov r1, #0\n"
        "    str r1, [r0]\n"
    );

    comando = arvoreAtribuida->raiz->esquerda;
    while (comando != NULL) {
        emitir_no(comando, &data, &texto, &constante, &label);
        comando = comando->proximo;
    }

    buffer_appendf(&texto, "fim:\n    b fim\n");
    emitir_rotinas(&texto);
    emitir_divisao_inteira(&texto);

    buffer_garantir(&data, texto.tamanho + 1);
    if (data.texto != NULL && texto.texto != NULL) {
        strcat(data.texto, texto.texto);
        data.tamanho += texto.tamanho;
        resultado.codigo = data.texto;
        data.texto = NULL;
    }

    free(data.texto);
    free(texto.texto);
    return resultado;
}

void liberarAssembly(ResultadoAssembly *assembly) {
    if (assembly == NULL) {
        return;
    }

    free(assembly->codigo);
    assembly->codigo = NULL;
}
