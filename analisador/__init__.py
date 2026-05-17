# Trabalho realizado individualmente:
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Pacote do analisador semantico da Fase 3.

Este pacote deve separar analise lexica, analise sintatica, analise semantica,
arvore sintatica atribuida e geracao de Assembly.
"""

from analisador.entrada import prepararEntradaSemantica
from analisador.tabela_simbolos import construirTabelaSimbolos
from analisador.tipos import verificarTipos
from analisador.arvore_atribuida import gerarArvoreAtribuida
from analisador.assembly import gerarAssembly

__all__ = [
    "prepararEntradaSemantica",
    "construirTabelaSimbolos",
    "verificarTipos",
    "gerarArvoreAtribuida",
    "gerarAssembly",
]
