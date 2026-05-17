# Trabalho realizado individualmente:
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Construcao e validacao inicial da tabela de simbolos."""

from __future__ import annotations

import re

from analisador.modelos import (
    MAX_RESULTADOS,
    MAX_SIMBOLOS,
    ArvoreSintatica,
    NoAst,
    ResultadoTabelaSimbolos,
    Simbolo,
    TipoDado,
    TipoNo,
    adicionar_erro,
    nome_tipo_dado,
    tipo_numerico,
)


def _buscar_simbolo(resultado: ResultadoTabelaSimbolos, nome: str) -> Simbolo | None:
    for simbolo in resultado.simbolos:
        if simbolo.identificador == nome:
            return simbolo
    return None


def _criar_simbolo(
    resultado: ResultadoTabelaSimbolos,
    nome: str,
    tipo: TipoDado,
    linha: int,
) -> Simbolo | None:
    if len(resultado.simbolos) >= MAX_SIMBOLOS:
        adicionar_erro(
            resultado.erros_semanticos,
            linha,
            nome,
            "Quantidade maxima de simbolos excedida.",
        )
        return None

    simbolo = Simbolo(
        identificador=nome,
        tipo=tipo,
        escopo="arquivo",
        linha_definicao=linha,
        linha_ultimo_uso=linha,
    )
    resultado.simbolos.append(simbolo)
    return simbolo


def _tipo_numero(valor: str) -> TipoDado:
    if "." in valor or "e" in valor or "E" in valor:
        return TipoDado.REAL
    return TipoDado.INTEIRO


def _tipo_operacao(
    operador: str,
    esquerdo: TipoDado,
    direito: TipoDado,
    linha: int,
    resultado: ResultadoTabelaSimbolos,
) -> TipoDado:
    if operador in {">", "<", "==", "!=", ">=", "<="}:
        if not tipo_numerico(esquerdo) or not tipo_numerico(direito):
            adicionar_erro(
                resultado.erros_semanticos,
                linha,
                operador,
                "Operador relacional exige operandos numericos.",
            )
            return TipoDado.ERRO
        return TipoDado.BOOL

    if operador in {"/", "%"}:
        if esquerdo != TipoDado.INTEIRO or direito != TipoDado.INTEIRO:
            adicionar_erro(
                resultado.erros_semanticos,
                linha,
                operador,
                f"Operador {operador} exige operandos inteiros.",
            )
            return TipoDado.ERRO
        return TipoDado.INTEIRO

    if operador == "|":
        if not tipo_numerico(esquerdo) or not tipo_numerico(direito):
            adicionar_erro(
                resultado.erros_semanticos,
                linha,
                operador,
                "Divisao real exige operandos numericos.",
            )
            return TipoDado.ERRO
        return TipoDado.REAL

    if operador in {"+", "-", "*", "^"}:
        if not tipo_numerico(esquerdo) or not tipo_numerico(direito):
            adicionar_erro(
                resultado.erros_semanticos,
                linha,
                operador,
                "Operador aritmetico exige operandos numericos.",
            )
            return TipoDado.ERRO
        if esquerdo == TipoDado.REAL or direito == TipoDado.REAL:
            return TipoDado.REAL
        return TipoDado.INTEIRO

    adicionar_erro(
        resultado.erros_semanticos,
        linha,
        operador,
        "Operador nao reconhecido.",
    )
    return TipoDado.ERRO


def _analisar_no(
    no: NoAst | None,
    resultado: ResultadoTabelaSimbolos,
    resultado_atual: int,
) -> TipoDado:
    if no is None:
        return TipoDado.DESCONHECIDO

    if no.tipo == TipoNo.NUMERO:
        no.tipo_dado = _tipo_numero(no.valor)
        return no.tipo_dado

    if no.tipo == TipoNo.VARIAVEL:
        simbolo = _buscar_simbolo(resultado, no.valor)
        if simbolo is None:
            adicionar_erro(
                resultado.erros_semanticos,
                no.linha,
                no.valor,
                "Variavel usada antes da definicao.",
            )
            no.tipo_dado = TipoDado.ERRO
            return TipoDado.ERRO
        if len(simbolo.linhas_uso) < MAX_RESULTADOS:
            simbolo.linhas_uso.append(no.linha)
        simbolo.linha_ultimo_uso = no.linha
        no.tipo_dado = simbolo.tipo
        return no.tipo_dado

    if no.tipo == TipoNo.ATRIBUICAO:
        tipo_valor = _analisar_no(no.esquerda, resultado, resultado_atual)
        simbolo = _buscar_simbolo(resultado, no.valor)
        if simbolo is None:
            _criar_simbolo(resultado, no.valor, tipo_valor, no.linha)
        elif simbolo.tipo != tipo_valor and tipo_valor != TipoDado.ERRO:
            adicionar_erro(
                resultado.erros_semanticos,
                no.linha,
                no.valor,
                "Redefinicao incompativel: variavel era "
                f"{nome_tipo_dado(simbolo.tipo)} e recebeu {nome_tipo_dado(tipo_valor)}.",
            )
            tipo_valor = TipoDado.ERRO
        no.tipo_dado = tipo_valor
        return no.tipo_dado

    if no.tipo == TipoNo.RES:
        if re.fullmatch(r"-?\d+", no.valor or "") is None:
            adicionar_erro(
                resultado.erros_semanticos,
                no.linha,
                no.valor,
                "RES exige N inteiro nao negativo.",
            )
            no.tipo_dado = TipoDado.ERRO
            return TipoDado.ERRO
        deslocamento = int(no.valor)
        if deslocamento < 0:
            adicionar_erro(
                resultado.erros_semanticos,
                no.linha,
                no.valor,
                "RES exige N inteiro nao negativo.",
            )
            no.tipo_dado = TipoDado.ERRO
            return TipoDado.ERRO
        if deslocamento == 0 or deslocamento > resultado_atual:
            adicionar_erro(
                resultado.erros_semanticos,
                no.linha,
                no.valor,
                "Referencia RES nao possui resultado anterior correspondente.",
            )
            no.tipo_dado = TipoDado.ERRO
            return TipoDado.ERRO
        no.tipo_dado = resultado.resultados[resultado_atual - deslocamento]
        return no.tipo_dado

    if no.tipo == TipoNo.BINARIO:
        esquerdo = _analisar_no(no.esquerda, resultado, resultado_atual)
        direito = _analisar_no(no.direita, resultado, resultado_atual)
        no.tipo_dado = _tipo_operacao(no.operador, esquerdo, direito, no.linha, resultado)
        return no.tipo_dado

    if no.tipo in (TipoNo.IF, TipoNo.WHILE):
        esquerdo = _analisar_no(no.esquerda, resultado, resultado_atual)
        direito = _analisar_no(no.direita, resultado, resultado_atual)
        if esquerdo not in (TipoDado.BOOL, TipoDado.ERRO):
            adicionar_erro(
                resultado.erros_semanticos,
                no.linha,
                "IF" if no.tipo == TipoNo.IF else "WHILE",
                "Condicao de decisao ou repeticao deve ser bool.",
            )
            no.tipo_dado = TipoDado.ERRO
            return TipoDado.ERRO
        no.tipo_dado = direito
        return no.tipo_dado

    no.tipo_dado = TipoDado.ERRO
    return TipoDado.ERRO


def construirTabelaSimbolos(arvore: ArvoreSintatica | None):
    """Percorre a arvore sintatica e registra simbolos da linguagem."""
    resultado = ResultadoTabelaSimbolos()
    if arvore is None or arvore.raiz is None:
        return resultado

    comando = arvore.raiz.esquerda
    while comando is not None:
        tipo = _analisar_no(comando, resultado, len(resultado.resultados))
        if len(resultado.resultados) < MAX_RESULTADOS:
            resultado.resultados.append(tipo)
        comando = comando.proximo

    return resultado
