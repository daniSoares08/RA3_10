# Integrantes do grupo (ordem alfabetica):
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Funcoes de teste especificas exigidas pela Secao 3.5 do Enunciado.

Cada funcao exercita o pipeline real (lexer, parser e analise semantica) sobre
codigos-fonte em memoria e verifica se os erros esperados sao (ou nao) gerados.
Estas funcoes complementam, mas nao substituem, os tres arquivos .txt obrigatorios.

Execucao isolada (sem menu interativo):

    python funcoes_teste_semantico.py
"""

from __future__ import annotations

from analisador.entrada import analisarFonte
from analisador.tabela_simbolos import construirTabelaSimbolos
from analisador.tipos import verificarTipos


def _analisar(fonte: str):
    """Roda lexer, parser e analise semantica completa sobre um fonte em memoria.

    Reproduz o mesmo pipeline do cli.py: construirTabelaSimbolos() cuida dos erros
    de declaracao e verificarTipos() dos erros de compatibilidade de tipos. Retorna
    (erros_lexicos, erros_sintaticos, erros_semanticos) com os dois conjuntos de
    erros semanticos ja combinados.
    """
    entrada = analisarFonte(fonte)
    tabela = construirTabelaSimbolos(entrada.arvore)
    tipos = verificarTipos(entrada.arvore, tabela)
    erros_semanticos = list(tabela.erros_semanticos) + list(tipos.erros_semanticos)
    return (
        entrada.erros_lexicos,
        entrada.erros_sintaticos,
        erros_semanticos,
    )


def _verificar(condicao: bool, descricao: str) -> bool:
    """Imprime o resultado de um caso e devolve o status booleano."""
    status = "OK" if condicao else "FALHOU"
    print(f"  [{status}] {descricao}")
    return condicao


def validar_erros_lexicos() -> bool:
    """Valida tokens invalidos e comentarios mal formados."""
    print("validar_erros_lexicos:")
    lex_invalido, _, _ = _analisar("(START)\n(1 @ 2)\n(END)\n")
    lex_comentario, _, _ = _analisar("(START)\n*{ comentario sem fechamento\n(END)\n")
    lex_valido, _, _ = _analisar("(START)\n(1 2 +)\n(END)\n")
    ok = True
    ok &= _verificar(len(lex_invalido) > 0, "token invalido '@' gera erro lexico")
    ok &= _verificar(len(lex_comentario) > 0, "comentario *{ sem }* gera erro lexico")
    ok &= _verificar(len(lex_valido) == 0, "programa valido nao gera erro lexico")
    return ok


def validar_erros_sintaticos() -> bool:
    """Valida expressoes mal formadas e estrutura START/END."""
    print("validar_erros_sintaticos:")
    _, sem_end, _ = _analisar("(START)\n(1 2 +)\n")
    _, rpn_invalida, _ = _analisar("(START)\n(1 2 3 4)\n(END)\n")
    _, valido, _ = _analisar("(START)\n(1 2 +)\n(END)\n")
    ok = True
    ok &= _verificar(len(sem_end) > 0, "programa sem (END) gera erro sintatico")
    ok &= _verificar(len(rpn_invalida) > 0, "RPN mal formada gera erro sintatico")
    ok &= _verificar(len(valido) == 0, "programa valido nao gera erro sintatico")
    return ok


def validar_erros_semanticos() -> bool:
    """Valida tipos incompativeis e uso incorreto de variaveis."""
    print("validar_erros_semanticos:")
    _, _, nao_declarada = _analisar("(START)\n(X)\n(END)\n")
    _, _, bool_em_soma = _analisar("(START)\n(TRUE 1 +)\n(END)\n")
    _, _, and_invalido = _analisar("(START)\n(1 TRUE AND)\n(END)\n")
    _, _, not_invalido = _analisar("(START)\n(3 NOT)\n(END)\n")
    _, _, logico_valido = _analisar("(START)\n(TRUE FALSE OR)\n(FALSE NOT)\n(END)\n")
    _, _, valido = _analisar("(START)\n(1 2 +)\n(END)\n")
    ok = True
    ok &= _verificar(len(nao_declarada) > 0, "variavel usada antes da definicao gera erro")
    ok &= _verificar(len(bool_em_soma) > 0, "literal logico em soma gera erro semantico")
    ok &= _verificar(len(and_invalido) > 0, "AND com operando nao logico gera erro semantico")
    ok &= _verificar(len(not_invalido) > 0, "NOT com operando nao logico gera erro semantico")
    ok &= _verificar(len(logico_valido) == 0, "operadores logicos sobre bool sao validos")
    ok &= _verificar(len(valido) == 0, "expressao numerica valida nao gera erro")
    return ok


def validar_estruturas_controle_validas() -> bool:
    """Valida decisoes e lacos semanticamente corretos."""
    print("validar_estruturas_controle_validas:")
    _, sin_if, sem_if = _analisar("(START)\n((1 2 <) (3 4 +) IF)\n(END)\n")
    _, sin_while, sem_while = _analisar("(START)\n((TRUE) (1 1 +) WHILE)\n(END)\n")
    _, sin_log, sem_log = _analisar(
        "(START)\n(((1 2 <) (3 4 >) OR) (5 6 +) IF)\n((FALSE NOT) (1 1 +) WHILE)\n(END)\n"
    )
    ok = True
    ok &= _verificar(
        len(sin_if) == 0 and len(sem_if) == 0,
        "IF com condicao relacional bool e valido",
    )
    ok &= _verificar(
        len(sin_while) == 0 and len(sem_while) == 0,
        "WHILE com condicao literal logico e valido",
    )
    ok &= _verificar(
        len(sin_log) == 0 and len(sem_log) == 0,
        "IF/WHILE com condicao de operador logico (OR/NOT) e valido",
    )
    return ok


def validar_erros_semanticos_estruturas_controle() -> bool:
    """Valida condicoes invalidas e variaveis incorretas em lacos."""
    print("validar_erros_semanticos_estruturas_controle:")
    _, _, if_nao_bool = _analisar("(START)\n((1 2 +) (3 4 +) IF)\n(END)\n")
    _, _, while_nao_bool = _analisar("(START)\n((5 6 *) (1 1 +) WHILE)\n(END)\n")
    _, _, var_nao_declarada = _analisar("(START)\n((Y 0 >) (1 1 +) WHILE)\n(END)\n")
    ok = True
    ok &= _verificar(len(if_nao_bool) > 0, "IF com condicao nao bool gera erro semantico")
    ok &= _verificar(len(while_nao_bool) > 0, "WHILE com condicao nao bool gera erro semantico")
    ok &= _verificar(len(var_nao_declarada) > 0, "variavel nao declarada em laco gera erro")
    return ok


def validar_erros_tipos_expressoes_aninhadas() -> bool:
    """Valida incompatibilidades em expressoes aninhadas."""
    print("validar_erros_tipos_expressoes_aninhadas:")
    _, _, mod_com_real = _analisar("(START)\n((1.5 2.0 |) 3 %)\n(END)\n")
    _, _, bool_aninhado = _analisar("(START)\n((TRUE) (1 2 +) +)\n(END)\n")
    _, _, valido = _analisar("(START)\n((1 2 +) (3 4 *) +)\n(END)\n")
    ok = True
    ok &= _verificar(len(mod_com_real) > 0, "'%' sobre resultado real aninhado gera erro")
    ok &= _verificar(len(bool_aninhado) > 0, "soma com bool aninhado gera erro semantico")
    ok &= _verificar(len(valido) == 0, "aninhamento numerico compativel e valido")
    return ok


def validar_casos_extremos() -> bool:
    """Valida aninhamento profundo e expressoes vazias."""
    print("validar_casos_extremos:")
    lex_fundo, sin_fundo, sem_fundo = _analisar(
        "(START)\n((((1 2 +) 3 +) 4 +) 5 +)\n(END)\n"
    )
    _, sin_vazia, _ = _analisar("(START)\n()\n(END)\n")
    ok = True
    ok &= _verificar(
        len(lex_fundo) == 0 and len(sin_fundo) == 0 and len(sem_fundo) == 0,
        "aninhamento profundo valido nao gera erros",
    )
    ok &= _verificar(len(sin_vazia) > 0, "expressao vazia '()' gera erro sintatico")
    return ok


def executar_todos() -> bool:
    """Executa todas as funcoes de teste e devolve True se todas passarem."""
    funcoes = [
        validar_erros_lexicos,
        validar_erros_sintaticos,
        validar_erros_semanticos,
        validar_estruturas_controle_validas,
        validar_erros_semanticos_estruturas_controle,
        validar_erros_tipos_expressoes_aninhadas,
        validar_casos_extremos,
    ]
    todos_ok = True
    for funcao in funcoes:
        todos_ok &= funcao()
    print()
    print("RESULTADO:", "todas as funcoes de teste passaram" if todos_ok else "ha falhas")
    return todos_ok


if __name__ == "__main__":
    raise SystemExit(0 if executar_todos() else 1)
