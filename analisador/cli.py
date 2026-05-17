# Trabalho realizado individualmente:
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Interface de linha de comando sem menu interativo."""

from __future__ import annotations

import sys

from analisador.arquivo_saida import formatar_caminhos_saida
from analisador.arvore_atribuida import gerarArvoreAtribuida
from analisador.artefatos import salvarArtefatosUltimaExecucao
from analisador.assembly import gerarAssembly
from analisador.entrada import prepararEntradaSemantica
from analisador.modelos import ErroAnalise, adicionar_erro
from analisador.tabela_simbolos import construirTabelaSimbolos
from analisador.tipos import verificarTipos


def _copiar_erros(*listas: list[ErroAnalise]) -> list[ErroAnalise]:
    erros: list[ErroAnalise] = []
    for lista in listas:
        erros.extend(lista)
    return erros


def main(argv=None):
    """Coordena a execucao completa do compilador da Fase 3."""
    argumentos = sys.argv[1:] if argv is None else argv
    if len(argumentos) != 1:
        print("Uso: python AnalisadorSemantico.py <arquivo_teste>")
        return 2

    arquivo_teste = argumentos[0]
    entrada = prepararEntradaSemantica(arquivo_teste)
    tabela = construirTabelaSimbolos(entrada.arvore)
    tipos = verificarTipos(entrada.arvore, tabela)
    arvore_atribuida = gerarArvoreAtribuida(entrada.arvore, tabela, tipos)

    erros = _copiar_erros(
        entrada.erros_lexicos,
        entrada.erros_sintaticos,
        tabela.erros_semanticos,
        tipos.erros_semanticos,
    )

    assembly = ""
    if not erros:
        assembly = gerarAssembly(arvore_atribuida)

    status = salvarArtefatosUltimaExecucao(
        arquivo_teste,
        tabela,
        erros,
        arvore_atribuida,
        assembly,
    )
    if status != 0:
        adicionar_erro(
            erros,
            0,
            "resultados",
            "Nao foi possivel salvar todos os artefatos obrigatorios.",
        )

    print(f"Arquivo analisado: {arquivo_teste}")
    print(f"Resultado da analise lexica: {len(entrada.erros_lexicos)} erro(s)")
    print(f"Resultado da analise sintatica: {len(entrada.erros_sintaticos)} erro(s)")
    print(f"Resultado da analise semantica: {len(erros)} erro(s) total(is)")
    print("Lista de erros encontrados:")
    if not erros:
        print("- nenhum erro registrado")
    else:
        for erro in erros:
            print(f'- linha {erro.linha}, elemento "{erro.elemento}": {erro.causa}')
    print("Caminhos dos arquivos de saida gerados:")
    print(formatar_caminhos_saida())
    return 1 if erros else 0
