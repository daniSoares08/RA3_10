# Integrantes do grupo (ordem alfabetica):
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Preparacao da entrada para a analise semantica."""

from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path

from analisador.modelos import (
    MAX_TOKENS,
    ArvoreSintatica,
    EntradaSemantica,
    NoAst,
    TipoNo,
    TipoToken,
    Token,
    adicionar_erro,
    adicionar_filho_programa,
    criar_no,
    nome_tipo_token,
)


PROJETO_RAIZ = Path(__file__).resolve().parents[1]


@dataclass
class Parser:
    tokens: list[Token]
    entrada: EntradaSemantica
    posicao: int = 0


def _char(fonte: str, indice: int) -> str:
    if indice < len(fonte):
        return fonte[indice]
    return "\0"


def _adicionar_token(
    entrada: EntradaSemantica,
    tipo: TipoToken,
    lexema: str,
    linha: int,
    coluna: int,
) -> None:
    if len(entrada.tokens) >= MAX_TOKENS:
        adicionar_erro(
            entrada.erros_lexicos,
            linha,
            "tokens",
            "Quantidade maxima de tokens excedida.",
        )
        return
    entrada.tokens.append(Token(tipo=tipo, lexema=lexema[:127], linha=linha, coluna=coluna))


def _numero_valido(lexema: str) -> bool:
    return re.fullmatch(r"-?\d+(\.\d+)?([eE][+-]?\d+)?", lexema) is not None


def _registrar_comentario(
    entrada: EntradaSemantica,
    lexema: str,
    linha: int,
    coluna: int,
) -> None:
    """Registra um comentario como token COMENTARIO fora do vetor do parser.

    A Secao 2.1 do enunciado pede que o lexer trate comentarios como tokens de
    tipo "comentario" e os descarte do vetor usado pelo analisador sintatico.
    Por isso eles entram em `entrada.comentarios`, e nunca em `entrada.tokens`.
    """
    entrada.comentarios.append(
        Token(tipo=TipoToken.COMENTARIO, lexema=lexema[:127], linha=linha, coluna=coluna)
    )


def _eh_operador_arit(c: str) -> bool:
    return c in {"+", "-", "*", "|", "/", "%", "^"}


def _tipo_identificador(lexema: str) -> TipoToken:
    palavras = {
        "RES": TipoToken.RES,
        "START": TipoToken.START,
        "END": TipoToken.END,
        "IF": TipoToken.IF,
        "WHILE": TipoToken.WHILE,
        "TRUE": TipoToken.LOGICO,
        "FALSE": TipoToken.LOGICO,
        "AND": TipoToken.OPERADOR_LOGICO,
        "OR": TipoToken.OPERADOR_LOGICO,
        "NOT": TipoToken.OPERADOR_LOGICO,
    }
    return palavras.get(lexema, TipoToken.IDENTIFICADOR)


def _tokenizar(entrada: EntradaSemantica) -> int:
    fonte = entrada.fonte
    i = 0
    linha = 1
    coluna = 1

    while i < len(fonte):
        c = fonte[i]

        if c == "\r":
            i += 1
            continue
        if c == "\n":
            linha += 1
            coluna = 1
            i += 1
            continue
        if c.isspace():
            coluna += 1
            i += 1
            continue

        if c == "*" and _char(fonte, i + 1) == "{":
            linha_inicio = linha
            coluna_inicio = coluna
            inicio = i
            i += 2
            coluna += 2
            while i < len(fonte) and not (fonte[i] == "}" and _char(fonte, i + 1) == "*"):
                if fonte[i] == "\n":
                    linha += 1
                    coluna = 1
                    i += 1
                else:
                    coluna += 1
                    i += 1
            if i >= len(fonte):
                adicionar_erro(
                    entrada.erros_lexicos,
                    linha_inicio,
                    "*{",
                    "Comentario iniciado por *{ sem fechamento }*.",
                )
                return 1
            i += 2
            coluna += 2
            # Comentario reconhecido como token COMENTARIO e descartado do vetor
            # do parser (Secao 2.1): registrado a parte em entrada.comentarios.
            _registrar_comentario(entrada, fonte[inicio:i], linha_inicio, coluna_inicio)
            continue

        if c == "(":
            _adicionar_token(entrada, TipoToken.ABRE_PAREN, "(", linha, coluna)
            i += 1
            coluna += 1
            continue
        if c == ")":
            _adicionar_token(entrada, TipoToken.FECHA_PAREN, ")", linha, coluna)
            i += 1
            coluna += 1
            continue

        if c.isdigit() or (c == "-" and _char(fonte, i + 1).isdigit()):
            inicio = i
            coluna_inicio = coluna
            pontos = 0
            expoente = 0

            if c == "-":
                i += 1
                coluna += 1
            while i < len(fonte):
                atual = fonte[i]
                anterior = fonte[i - 1] if i > 0 else "\0"
                if not (atual.isdigit() or atual in ".eE+-"):
                    break
                if atual == ".":
                    pontos += 1
                    if pontos > 1:
                        break
                elif atual in "eE":
                    expoente += 1
                    if expoente > 1:
                        break
                    i += 1
                    coluna += 1
                    if i < len(fonte) and fonte[i] in "+-":
                        i += 1
                        coluna += 1
                    continue
                elif atual in "+-" and anterior not in "eE":
                    break
                i += 1
                coluna += 1
            lexema = fonte[inicio:i]
            if not _numero_valido(lexema):
                adicionar_erro(
                    entrada.erros_lexicos,
                    linha,
                    lexema,
                    "Literal numerico mal formado.",
                )
                return 1
            _adicionar_token(entrada, TipoToken.NUMERO, lexema, linha, coluna_inicio)
            continue

        if "A" <= c <= "Z":
            inicio = i
            coluna_inicio = coluna
            while i < len(fonte) and "A" <= fonte[i] <= "Z":
                i += 1
                coluna += 1
            if i < len(fonte) and (fonte[i].isalnum() or fonte[i] == "_"):
                fim = i + 1
                while fim < len(fonte) and (fonte[fim].isalnum() or fonte[fim] == "_"):
                    fim += 1
                adicionar_erro(
                    entrada.erros_lexicos,
                    linha,
                    fonte[inicio:fim],
                    "Identificador deve conter apenas letras latinas maiusculas.",
                )
                return 1
            lexema = fonte[inicio:i]
            _adicionar_token(
                entrada,
                _tipo_identificador(lexema),
                lexema,
                linha,
                coluna_inicio,
            )
            continue

        dois = fonte[i : i + 2]
        if dois in {"==", "!=", ">=", "<="}:
            _adicionar_token(entrada, TipoToken.OPERADOR_REL, dois, linha, coluna)
            i += 2
            coluna += 2
            continue
        if c in {">", "<"}:
            _adicionar_token(entrada, TipoToken.OPERADOR_REL, c, linha, coluna)
            i += 1
            coluna += 1
            continue
        if _eh_operador_arit(c):
            _adicionar_token(entrada, TipoToken.OPERADOR_ARIT, c, linha, coluna)
            i += 1
            coluna += 1
            continue

        adicionar_erro(entrada.erros_lexicos, linha, c, "Lexema invalido.")
        return 1

    _adicionar_token(entrada, TipoToken.EOF, "$", linha, coluna)
    return 1 if entrada.erros_lexicos else 0


def _atual(parser: Parser) -> Token:
    if parser.posicao >= len(parser.tokens):
        return parser.tokens[-1]
    return parser.tokens[parser.posicao]


def _aceitar(parser: Parser, tipo: TipoToken) -> bool:
    if _atual(parser).tipo == tipo:
        parser.posicao += 1
        return True
    return False


def _erro_sintatico(parser: Parser, esperado: str) -> None:
    token = _atual(parser)
    causa = f"Esperado {esperado}, encontrado {nome_tipo_token(token.tipo)}."
    adicionar_erro(parser.entrada.erros_sintaticos, token.linha, token.lexema, causa)


def _parse_item(parser: Parser) -> NoAst:
    token = _atual(parser)

    if token.tipo == TipoToken.ABRE_PAREN:
        return _parse_parenteses(parser)
    if token.tipo == TipoToken.NUMERO:
        parser.posicao += 1
        return criar_no(TipoNo.NUMERO, token.lexema, token.linha)
    if token.tipo == TipoToken.LOGICO:
        parser.posicao += 1
        return criar_no(TipoNo.LOGICO, token.lexema, token.linha)
    if token.tipo == TipoToken.IDENTIFICADOR:
        parser.posicao += 1
        return criar_no(TipoNo.VARIAVEL, token.lexema, token.linha)
    if token.tipo in (
        TipoToken.OPERADOR_ARIT,
        TipoToken.OPERADOR_REL,
        TipoToken.OPERADOR_LOGICO,
    ):
        parser.posicao += 1
        no = criar_no(TipoNo.BINARIO, None, token.linha)
        no.operador = token.lexema
        return no
    if token.tipo == TipoToken.RES:
        parser.posicao += 1
        return criar_no(TipoNo.RES, token.lexema, token.linha)
    if token.tipo == TipoToken.IF:
        parser.posicao += 1
        return criar_no(TipoNo.IF, token.lexema, token.linha)
    if token.tipo == TipoToken.WHILE:
        parser.posicao += 1
        return criar_no(TipoNo.WHILE, token.lexema, token.linha)

    _erro_sintatico(
        parser,
        "expressao, identificador, numero, literal logico, operador, "
        "AND/OR/NOT, IF, WHILE ou RES",
    )
    parser.posicao += 1
    return criar_no(TipoNo.INVALIDO, token.lexema, token.linha)


def _item_operador(no: NoAst | None) -> bool:
    return (
        no is not None
        and no.tipo == TipoNo.BINARIO
        and no.operador != ""
        and no.esquerda is None
        and no.direita is None
    )


def _montar_expressao(parser: Parser, itens: list[NoAst], linha: int) -> NoAst:
    if len(itens) == 1:
        return itens[0]

    # Operador logico unario: (expressao NOT).
    if len(itens) == 2 and _item_operador(itens[1]) and itens[1].operador == "NOT":
        resultado = criar_no(TipoNo.UNARIO, None, itens[1].linha)
        resultado.operador = "NOT"
        resultado.esquerda = itens[0]
        return resultado

    if len(itens) == 2 and itens[0].tipo == TipoNo.NUMERO and itens[1].tipo == TipoNo.RES:
        return criar_no(TipoNo.RES, itens[0].valor, itens[0].linha)

    # Comando especial do enunciado: (V MEM), onde MEM e o nome da memoria.
    if len(itens) == 2 and itens[1].tipo == TipoNo.VARIAVEL:
        resultado = criar_no(TipoNo.ATRIBUICAO, itens[1].valor, itens[1].linha)
        resultado.esquerda = itens[0]
        return resultado

    if len(itens) == 3 and _item_operador(itens[2]):
        resultado = itens[2]
        resultado.esquerda = itens[0]
        resultado.direita = itens[1]
        return resultado

    if len(itens) == 3 and itens[2].tipo in (TipoNo.IF, TipoNo.WHILE):
        resultado = criar_no(itens[2].tipo, itens[2].valor, itens[2].linha)
        resultado.esquerda = itens[0]
        resultado.direita = itens[1]
        return resultado

    adicionar_erro(
        parser.entrada.erros_sintaticos,
        linha,
        "expressao",
        "Expressao RPN nao corresponde aos formatos da Fase 2.",
    )
    return criar_no(TipoNo.INVALIDO, "expressao", linha)


def _parse_parenteses(parser: Parser) -> NoAst:
    abre = _atual(parser)
    itens: list[NoAst] = []

    if not _aceitar(parser, TipoToken.ABRE_PAREN):
        _erro_sintatico(parser, "'('")
        return criar_no(TipoNo.INVALIDO, "(", _atual(parser).linha)

    while _atual(parser).tipo not in (TipoToken.FECHA_PAREN, TipoToken.EOF):
        if len(itens) >= 8:
            _erro_sintatico(parser, "expressao RPN com ate 3 itens relevantes")
            break
        itens.append(_parse_item(parser))

    if not _aceitar(parser, TipoToken.FECHA_PAREN):
        _erro_sintatico(parser, "')'")
        return criar_no(TipoNo.INVALIDO, ")", abre.linha)

    return _montar_expressao(parser, itens, abre.linha)


def _parse_marcador(parser: Parser, esperado: TipoToken, nome: str) -> bool:
    if not _aceitar(parser, TipoToken.ABRE_PAREN):
        _erro_sintatico(parser, "'('")
        return False
    if not _aceitar(parser, esperado):
        _erro_sintatico(parser, nome)
        return False
    if not _aceitar(parser, TipoToken.FECHA_PAREN):
        _erro_sintatico(parser, "')'")
        return False
    return True


def _parse_programa(entrada: EntradaSemantica) -> None:
    parser = Parser(tokens=entrada.tokens, entrada=entrada)
    programa = criar_no(TipoNo.PROGRAMA, "PROGRAMA", 1)
    entrada.arvore = ArvoreSintatica(raiz=programa, descricao="Programa RA3_10")
    encontrou_end = False

    if not _parse_marcador(parser, TipoToken.START, "START"):
        return

    while _atual(parser).tipo != TipoToken.EOF:
        if (
            _atual(parser).tipo == TipoToken.ABRE_PAREN
            and parser.posicao + 1 < len(parser.tokens)
            and parser.tokens[parser.posicao + 1].tipo == TipoToken.END
        ):
            _parse_marcador(parser, TipoToken.END, "END")
            encontrou_end = True
            break

        if _atual(parser).tipo != TipoToken.ABRE_PAREN:
            _erro_sintatico(parser, "'(' iniciando comando ou (END)")
            parser.posicao += 1
            continue

        adicionar_filho_programa(programa, _parse_parenteses(parser))

    if not encontrou_end:
        adicionar_erro(
            entrada.erros_sintaticos,
            _atual(parser).linha,
            "$",
            "Programa deve terminar com (END).",
        )
    elif _atual(parser).tipo != TipoToken.EOF:
        adicionar_erro(
            entrada.erros_sintaticos,
            _atual(parser).linha,
            _atual(parser).lexema,
            "Nenhum token deve aparecer apos (END).",
        )


def _resolver_caminho(arquivo: str) -> Path:
    caminho = Path(arquivo)
    if caminho.exists():
        return caminho
    caminho_raiz = PROJETO_RAIZ / caminho
    return caminho_raiz


def analisarFonte(fonte, arquivo="<memoria>"):
    """Tokeniza e analisa sintaticamente um codigo-fonte ja carregado em memoria.

    Usada por prepararEntradaSemantica() e pelas funcoes de teste, que precisam
    exercitar o lexer e o parser sem depender de um arquivo em disco.
    """
    entrada = EntradaSemantica(arquivo=str(arquivo or ""))
    entrada.fonte = str(fonte or "")
    if _tokenizar(entrada) == 0:
        _parse_programa(entrada)
    return entrada


def prepararEntradaSemantica(arquivo):
    """Carrega a entrada e prepara tokens/arvore para a etapa semantica."""
    caminho = _resolver_caminho(str(arquivo or ""))

    if not caminho.exists():
        entrada = EntradaSemantica(arquivo=str(arquivo or ""))
        adicionar_erro(
            entrada.erros_lexicos,
            0,
            arquivo,
            "Arquivo de teste nao encontrado.",
        )
        return entrada

    try:
        fonte = caminho.read_text(encoding="utf-8")
    except OSError:
        entrada = EntradaSemantica(arquivo=str(arquivo or ""))
        adicionar_erro(
            entrada.erros_lexicos,
            0,
            arquivo,
            "Nao foi possivel ler o arquivo de teste.",
        )
        return entrada

    return analisarFonte(fonte, arquivo)
