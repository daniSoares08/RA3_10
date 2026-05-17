# Trabalho realizado individualmente:
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Geracao de Assembly para Cpulator-ARMv7 DEC1-SOC(v16.1)."""

from __future__ import annotations

from analisador.modelos import ArvoreAtribuida, NoAst, TipoNo


def _coletar_variaveis(no: NoAst | None, nomes: list[str]) -> None:
    if no is None:
        return
    if no.tipo in (TipoNo.ATRIBUICAO, TipoNo.VARIAVEL) and no.valor and no.valor not in nomes:
        nomes.append(no.valor)
    _coletar_variaveis(no.esquerda, nomes)
    _coletar_variaveis(no.direita, nomes)
    _coletar_variaveis(no.terceiro, nomes)
    _coletar_variaveis(no.proximo, nomes)


def _emitir_rotinas(texto: list[str]) -> None:
    texto.append(
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
    )


def _emitir_divisao_inteira(texto: list[str]) -> None:
    texto.append(
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
    )


def _emitir_operador(op: str, texto: list[str], estado: dict[str, int]) -> None:
    texto.append("    bl pop_d1\n")
    texto.append("    bl pop_d0\n")

    if op == "+":
        texto.append("    vadd.f64 d0, d0, d1\n    bl push_d0\n")
    elif op == "-":
        texto.append("    vsub.f64 d0, d0, d1\n    bl push_d0\n")
    elif op == "*":
        texto.append("    vmul.f64 d0, d0, d1\n    bl push_d0\n")
    elif op == "|":
        texto.append("    vdiv.f64 d0, d0, d1\n    bl push_d0\n")
    elif op in {"/", "%"}:
        label = estado["label"]
        texto.append(
            "    vcvt.s32.f64 s0, d0\n"
            "    vcvt.s32.f64 s1, d1\n"
            "    vmov r0, s0\n"
            "    vmov r1, s1\n"
            f"    cmp r1, #0\n"
            f"    beq int_div_zero_{label}\n"
            "    bl dividir_inteiros_assinados\n"
        )
        if op == "/":
            texto.append("    vmov s0, r2\n")
        else:
            texto.append("    vmov s0, r3\n")
        texto.append(
            "    vcvt.f64.s32 d0, s0\n"
            "    bl push_d0\n"
            f"    b int_div_fim_{label}\n"
            f"int_div_zero_{label}:\n"
            "    ldr r0, =const_zero_double\n"
            "    vldr d0, [r0]\n"
            "    bl push_d0\n"
            f"int_div_fim_{label}:\n"
        )
        estado["label"] += 1
    elif op == "^":
        label = estado["label"]
        estado["label"] += 1
        texto.append(
            "    vcvt.s32.f64 s0, d1\n"
            "    vmov r0, s0\n"
            "    ldr r1, =temp_pow_base\n"
            "    vstr d0, [r1]\n"
            "    ldr r1, =const_one_double\n"
            "    vldr d0, [r1]\n"
            f"pow_loop_{label}:\n"
            "    cmp r0, #0\n"
            f"    beq pow_fim_{label}\n"
            "    ldr r1, =temp_pow_base\n"
            "    vldr d2, [r1]\n"
            "    vmul.f64 d0, d0, d2\n"
            "    sub r0, r0, #1\n"
            f"    b pow_loop_{label}\n"
            f"pow_fim_{label}:\n"
            "    bl push_d0\n"
        )
    else:
        label = estado["label"]
        estado["label"] += 1
        texto.append("    vcmp.f64 d0, d1\n    vmrs APSR_nzcv, FPSCR\n")
        if op == ">":
            texto.append(f"    bgt cmp_true_{label}\n")
        elif op == "<":
            texto.append(f"    blt cmp_true_{label}\n")
        elif op == "==":
            texto.append(f"    beq cmp_true_{label}\n")
        elif op == "!=":
            texto.append(f"    bne cmp_true_{label}\n")
        elif op == ">=":
            texto.append(f"    bge cmp_true_{label}\n")
        elif op == "<=":
            texto.append(f"    ble cmp_true_{label}\n")
        texto.append(
            "    mov r0, #0\n"
            f"    b cmp_done_{label}\n"
            f"cmp_true_{label}:\n"
            "    mov r0, #1\n"
            f"cmp_done_{label}:\n"
            "    bl bool_from_r0\n"
            "    bl push_d0\n"
        )


def _emitir_no(no: NoAst | None, data: list[str], texto: list[str], estado: dict[str, int]) -> None:
    if no is None:
        return

    if no.tipo == TipoNo.NUMERO:
        constante = estado["constante"]
        data.append(f"const_{constante}: .double {no.valor}\n")
        texto.append(
            f"    ldr r0, =const_{constante}\n"
            "    vldr d0, [r0]\n"
            "    bl push_d0\n"
        )
        estado["constante"] += 1
    elif no.tipo == TipoNo.VARIAVEL:
        texto.append(f"    ldr r0, =mem_{no.valor}\n    vldr d0, [r0]\n    bl push_d0\n")
    elif no.tipo == TipoNo.ATRIBUICAO:
        _emitir_no(no.esquerda, data, texto, estado)
        texto.append(
            "    bl pop_d0\n"
            f"    ldr r0, =mem_{no.valor}\n"
            "    vstr d0, [r0]\n"
            "    bl push_d0\n"
        )
    elif no.tipo == TipoNo.RES:
        texto.append(
            f"    @ RES {no.valor}: valor recuperado dos resultados anteriores deve ser ligado ao historico\n"
        )
        texto.append("    ldr r0, =const_zero_double\n    vldr d0, [r0]\n    bl push_d0\n")
    elif no.tipo == TipoNo.BINARIO:
        _emitir_no(no.esquerda, data, texto, estado)
        _emitir_no(no.direita, data, texto, estado)
        _emitir_operador(no.operador, texto, estado)
    elif no.tipo == TipoNo.IF:
        label = estado["label"]
        estado["label"] += 1
        _emitir_no(no.esquerda, data, texto, estado)
        texto.append(
            "    bl pop_d0\n"
            "    ldr r0, =const_zero_double\n"
            "    vldr d1, [r0]\n"
            "    vcmp.f64 d0, d1\n"
            "    vmrs APSR_nzcv, FPSCR\n"
            f"    beq if_fim_{label}\n"
        )
        _emitir_no(no.direita, data, texto, estado)
        texto.append(f"if_fim_{label}:\n")
    elif no.tipo == TipoNo.WHILE:
        label = estado["label"]
        estado["label"] += 1
        texto.append(f"while_ini_{label}:\n")
        _emitir_no(no.esquerda, data, texto, estado)
        texto.append(
            "    bl pop_d0\n"
            "    ldr r0, =const_zero_double\n"
            "    vldr d1, [r0]\n"
            "    vcmp.f64 d0, d1\n"
            "    vmrs APSR_nzcv, FPSCR\n"
            f"    beq while_fim_{label}\n"
        )
        _emitir_no(no.direita, data, texto, estado)
        texto.append(f"    b while_ini_{label}\nwhile_fim_{label}:\n")
    else:
        texto.append("    @ no invalido ignorado na geracao de Assembly\n")


def gerarAssembly(arvoreAtribuida: ArvoreAtribuida | None):
    """Gera Assembly a partir da arvore sintatica atribuida."""
    if arvoreAtribuida is None or arvoreAtribuida.raiz is None:
        return ""

    variaveis: list[str] = []
    _coletar_variaveis(arvoreAtribuida.raiz, variaveis)

    data = [
        ".data\n.align 3\n",
        "const_zero_double: .double 0.0\n",
        "const_one_double: .double 1.0\n",
        "temp_pow_base: .double 0.0\n",
    ]
    for nome in variaveis:
        data.append(f"mem_{nome}: .double 0.0\n")
    data.append("pilha_topo: .word 0\n")
    data.append("pilha_valores: .space 4096\n\n")

    texto = [
        ".syntax unified\n"
        ".fpu vfpv3\n"
        ".global _start\n"
        ".text\n"
        "_start:\n"
        "    ldr r0, =pilha_topo\n"
        "    mov r1, #0\n"
        "    str r1, [r0]\n"
    ]

    estado = {"constante": 0, "label": 0}
    comando = arvoreAtribuida.raiz.esquerda
    while comando is not None:
        _emitir_no(comando, data, texto, estado)
        comando = comando.proximo

    texto.append("fim:\n    b fim\n")
    _emitir_rotinas(texto)
    _emitir_divisao_inteira(texto)

    return "".join(data) + "".join(texto)
