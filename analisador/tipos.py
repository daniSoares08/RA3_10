# Trabalho realizado individualmente:
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Validacao e inferencia de tipos da Fase 3."""

from __future__ import annotations

from analisador.modelos import (
    MAX_RESULTADOS,
    ArvoreSintatica,
    NoAst,
    ResultadoTabelaSimbolos,
    ResultadoTipos,
)


def _coletar_tipos(no: NoAst | None, resultado: ResultadoTipos) -> None:
    if no is None:
        return
    if len(resultado.tipos) < MAX_RESULTADOS:
        resultado.tipos.append(no.tipo_dado)
    _coletar_tipos(no.esquerda, resultado)
    _coletar_tipos(no.direita, resultado)
    _coletar_tipos(no.terceiro, resultado)
    _coletar_tipos(no.proximo, resultado)


def verificarTipos(
    arvore: ArvoreSintatica | None,
    tabelaSimbolos: ResultadoTabelaSimbolos | None,
):
    """Valida tipos em expressoes, comandos especiais, decisoes e lacos.

    A validacao principal ocorre durante construirTabelaSimbolos(), como na
    versao em C. Esta etapa consolida os tipos anotados na arvore.
    """
    resultado = ResultadoTipos()
    if arvore is not None:
        _coletar_tipos(arvore.raiz, resultado)
    _ = tabelaSimbolos
    return resultado
