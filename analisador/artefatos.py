# Trabalho realizado individualmente:
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Persistencia dos artefatos obrigatorios da ultima execucao."""

from __future__ import annotations

from pathlib import Path

from analisador.modelos import (
    ArvoreAtribuida,
    ErroAnalise,
    NoAst,
    ResultadoTabelaSimbolos,
    nome_tipo_dado,
    nome_tipo_no,
)


PROJETO_RAIZ = Path(__file__).resolve().parents[1]
RESULTADOS_DIR = PROJETO_RAIZ / "resultados"


def _escrever_texto(caminho: Path, texto: str) -> int:
    try:
        caminho.write_text(texto or "", encoding="utf-8")
    except OSError:
        return 1
    return 0


def _escrever_tabela(tabela_simbolos: ResultadoTabelaSimbolos) -> int:
    linhas = [
        "# Tabela de simbolos\n\n",
        "| Identificador | Tipo | Escopo | Linha definicao | Linhas de uso |\n",
        "|---|---|---|---:|---|\n",
    ]
    for simbolo in tabela_simbolos.simbolos:
        usos = "-" if not simbolo.linhas_uso else ", ".join(str(uso) for uso in simbolo.linhas_uso)
        linhas.append(
            f"| {simbolo.identificador} | {nome_tipo_dado(simbolo.tipo)} | "
            f"{simbolo.escopo} | {simbolo.linha_definicao} | {usos} |\n"
        )
    return _escrever_texto(RESULTADOS_DIR / "tabela_simbolos.md", "".join(linhas))


def _escrever_erros(erros: list[ErroAnalise]) -> int:
    linhas = ["# Erros semanticos\n\n"]
    if not erros:
        linhas.append("Nenhum erro semantico registrado.\n")
    else:
        for erro in erros:
            linhas.append(
                f"- linha {erro.linha}, elemento `{erro.elemento}`: {erro.causa}\n"
            )
    return _escrever_texto(RESULTADOS_DIR / "erros_semanticos.md", "".join(linhas))


def _escrever_no(no: NoAst | None, nivel: int, linhas: list[str]) -> None:
    if no is None:
        return
    linhas.append(
        f"{'  ' * nivel}- {nome_tipo_no(no.tipo)} valor=`{no.valor}` "
        f"operador=`{no.operador}` tipo={nome_tipo_dado(no.tipo_dado)} "
        f"linha={no.linha}\n"
    )
    _escrever_no(no.esquerda, nivel + 1, linhas)
    _escrever_no(no.direita, nivel + 1, linhas)
    _escrever_no(no.terceiro, nivel + 1, linhas)
    _escrever_no(no.proximo, nivel, linhas)


def _escrever_arvore(arvore_atribuida: ArvoreAtribuida) -> int:
    linhas = [
        "# Arvore sintatica atribuida\n\n",
        f"{arvore_atribuida.descricao}\n\n",
    ]
    _escrever_no(arvore_atribuida.raiz, 0, linhas)
    return _escrever_texto(
        RESULTADOS_DIR / "arvore_sintatica_atribuida.md",
        "".join(linhas),
    )


def salvarArtefatosUltimaExecucao(
    arquivo_teste,
    tabela_simbolos: ResultadoTabelaSimbolos,
    erros_semanticos: list[ErroAnalise],
    arvore_atribuida: ArvoreAtribuida,
    assembly,
):
    """Salva os artefatos finais exigidos pelo enunciado."""
    falhou = 0
    try:
        RESULTADOS_DIR.mkdir(exist_ok=True)
    except OSError:
        return 1

    falhou |= _escrever_texto(
        RESULTADOS_DIR / "arquivo_teste_final.md",
        f"# Arquivo de teste final\n\n{arquivo_teste}\n",
    )
    falhou |= _escrever_tabela(tabela_simbolos)
    falhou |= _escrever_erros(erros_semanticos)
    falhou |= _escrever_arvore(arvore_atribuida)
    falhou |= _escrever_texto(
        RESULTADOS_DIR / "codigo_assembly_ultima_execucao.s",
        assembly or "@ GERADO: nenhum Assembly disponivel nesta execucao.\n",
    )
    falhou |= _escrever_texto(
        RESULTADOS_DIR / "relatorio_execucao.md",
        "# Relatorio final de execucao\n\n"
        f"- Arquivo analisado: `{arquivo_teste}`\n"
        f"- Simbolos registrados: {len(tabela_simbolos.simbolos)}\n"
        f"- Erros registrados: {len(erros_semanticos)}\n",
    )
    return falhou
