# Trabalho realizado individualmente:
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Listagem dos caminhos de saida exigidos pelo enunciado."""

from pathlib import Path


def formatar_caminhos_saida():
    """Retorna os caminhos dos artefatos finais gerados.

    atualizar somente se eu alterar os nomes dos artefatos.
    """
    caminhos = [
        Path("resultados") / "arquivo_teste_final.md",
        Path("resultados") / "tabela_simbolos.md",
        Path("resultados") / "erros_semanticos.md",
        Path("resultados") / "arvore_sintatica_atribuida.md",
        Path("resultados") / "codigo_assembly_ultima_execucao.s",
    ]
    return "\n".join(f"- {caminho}" for caminho in caminhos)
