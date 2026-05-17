# Trabalho realizado individualmente:
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Geracao da arvore sintatica atribuida."""

from __future__ import annotations

from analisador.modelos import (
    ArvoreAtribuida,
    ArvoreSintatica,
    ResultadoTabelaSimbolos,
    ResultadoTipos,
)


def gerarArvoreAtribuida(
    arvore: ArvoreSintatica | None,
    tabelaSimbolos: ResultadoTabelaSimbolos | None,
    tipos: ResultadoTipos | None,
):
    """Produz a arvore sintatica aumentada/atribuida."""
    quantidade_simbolos = len(tabelaSimbolos.simbolos) if tabelaSimbolos is not None else 0
    quantidade_tipos = len(tipos.tipos) if tipos is not None else 0
    return ArvoreAtribuida(
        raiz=arvore.raiz if arvore is not None else None,
        descricao=(
            f"Arvore atribuida com {quantidade_simbolos} simbolo(s) "
            f"e {quantidade_tipos} tipo(s) anotado(s)."
        ),
    )
