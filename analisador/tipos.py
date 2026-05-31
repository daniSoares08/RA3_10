# Integrantes do grupo (ordem alfabetica):
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Validacao e inferencia de tipos da Fase 3 (Secao 7.3 do enunciado)."""

from __future__ import annotations

from analisador.modelos import (
    MAX_RESULTADOS,
    ArvoreSintatica,
    NoAst,
    ResultadoTabelaSimbolos,
    ResultadoTipos,
    TipoDado,
    TipoNo,
    adicionar_erro,
    avaliar_operacao,
    avaliar_operacao_unaria,
)


def _tipo_no(no: NoAst | None) -> TipoDado:
    if no is None:
        return TipoDado.DESCONHECIDO
    return no.tipo_dado


def _validar_no(no: NoAst | None, resultado: ResultadoTipos) -> None:
    if no is None:
        return

    # Pos-ordem: valida os operandos antes do operador, de modo que erros
    # internos sejam reportados antes dos externos (como na versao anterior).
    _validar_no(no.esquerda, resultado)
    _validar_no(no.direita, resultado)
    _validar_no(no.terceiro, resultado)

    if no.tipo == TipoNo.BINARIO:
        _, mensagem = avaliar_operacao(
            no.operador, _tipo_no(no.esquerda), _tipo_no(no.direita)
        )
        if mensagem is not None:
            adicionar_erro(resultado.erros_semanticos, no.linha, no.operador, mensagem)

    elif no.tipo == TipoNo.UNARIO:
        _, mensagem = avaliar_operacao_unaria(no.operador, _tipo_no(no.esquerda))
        if mensagem is not None:
            adicionar_erro(resultado.erros_semanticos, no.linha, no.operador, mensagem)

    elif no.tipo in (TipoNo.IF, TipoNo.WHILE):
        condicao = _tipo_no(no.esquerda)
        if condicao not in (TipoDado.BOOL, TipoDado.ERRO):
            adicionar_erro(
                resultado.erros_semanticos,
                no.linha,
                "IF" if no.tipo == TipoNo.IF else "WHILE",
                "Condicao de decisao ou repeticao deve ser bool.",
            )

    if len(resultado.tipos) < MAX_RESULTADOS:
        resultado.tipos.append(no.tipo_dado)

    _validar_no(no.proximo, resultado)


def verificarTipos(
    arvore: ArvoreSintatica | None,
    tabelaSimbolos: ResultadoTabelaSimbolos | None,
):
    """Valida tipos em expressoes, comandos especiais, decisoes e lacos.

    A inferencia/anotacao de tipos e feita por construirTabelaSimbolos(), que
    tambem cuida das verificacoes de declaracao (uso antes da definicao,
    redefinicao incompativel e RES). Esta etapa percorre a arvore ja anotada e
    reporta os erros de compatibilidade de tipos das operacoes (aritmeticas,
    relacionais, logicas AND/OR/NOT e de divisao) e das condicoes de IF/WHILE,
    usando as mesmas regras formais documentadas em calculo de sequentes.
    """
    resultado = ResultadoTipos()
    if arvore is not None:
        _validar_no(arvore.raiz, resultado)
    _ = tabelaSimbolos
    return resultado
