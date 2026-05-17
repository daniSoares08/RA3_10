# Trabalho realizado individualmente:
# Daniel Campos Soares - daniSoares08
#
# Nome do grupo no Canvas: RA3_10

"""Modelos compartilhados pelo analisador semantico Python."""

from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum


TAMANHO_LEXEMA = 128
TAMANHO_CAUSA = 512
MAX_TOKENS = 1024
MAX_ERROS = 64
MAX_SIMBOLOS = 128
MAX_RESULTADOS = 256


class TipoToken(Enum):
    ABRE_PAREN = "ABRE_PAREN"
    FECHA_PAREN = "FECHA_PAREN"
    NUMERO = "NUMERO"
    IDENTIFICADOR = "IDENTIFICADOR"
    OPERADOR_ARIT = "OPERADOR_ARIT"
    OPERADOR_REL = "OPERADOR_REL"
    RES = "RES"
    START = "START"
    END = "END"
    IF = "IF"
    WHILE = "WHILE"
    EOF = "EOF"
    INVALIDO = "INVALIDO"


class TipoNo(Enum):
    PROGRAMA = "PROGRAMA"
    NUMERO = "NUMERO"
    VARIAVEL = "VARIAVEL"
    ATRIBUICAO = "ATRIBUICAO"
    RES = "RES"
    BINARIO = "BINARIO"
    IF = "IF"
    WHILE = "WHILE"
    INVALIDO = "INVALIDO"


class TipoDado(Enum):
    DESCONHECIDO = "desconhecido"
    INTEIRO = "inteiro"
    REAL = "real"
    BOOL = "bool"
    ERRO = "erro"


@dataclass
class Token:
    tipo: TipoToken
    lexema: str
    linha: int
    coluna: int


@dataclass
class ErroAnalise:
    linha: int
    elemento: str
    causa: str


@dataclass
class NoAst:
    tipo: TipoNo
    valor: str = ""
    linha: int = 0
    operador: str = ""
    tipo_dado: TipoDado = TipoDado.DESCONHECIDO
    esquerda: NoAst | None = None
    direita: NoAst | None = None
    terceiro: NoAst | None = None
    proximo: NoAst | None = None


@dataclass
class ArvoreSintatica:
    raiz: NoAst | None = None
    descricao: str = ""


@dataclass
class EntradaSemantica:
    arquivo: str = ""
    fonte: str = ""
    tokens: list[Token] = field(default_factory=list)
    arvore: ArvoreSintatica = field(default_factory=ArvoreSintatica)
    erros_lexicos: list[ErroAnalise] = field(default_factory=list)
    erros_sintaticos: list[ErroAnalise] = field(default_factory=list)


@dataclass
class Simbolo:
    identificador: str
    tipo: TipoDado
    escopo: str
    linha_definicao: int
    linha_ultimo_uso: int
    linhas_uso: list[int] = field(default_factory=list)


@dataclass
class ResultadoTabelaSimbolos:
    simbolos: list[Simbolo] = field(default_factory=list)
    resultados: list[TipoDado] = field(default_factory=list)
    erros_semanticos: list[ErroAnalise] = field(default_factory=list)


@dataclass
class ResultadoTipos:
    tipos: list[TipoDado] = field(default_factory=list)
    erros_semanticos: list[ErroAnalise] = field(default_factory=list)


@dataclass
class ArvoreAtribuida:
    raiz: NoAst | None = None
    descricao: str = ""


@dataclass
class ResultadoAssembly:
    codigo: str = ""


def adicionar_erro(
    erros: list[ErroAnalise],
    linha: int,
    elemento: object,
    causa: object,
) -> None:
    if len(erros) >= MAX_ERROS:
        return
    erros.append(
        ErroAnalise(
            linha=linha,
            elemento=str(elemento or ""),
            causa=str(causa or ""),
        )
    )


def criar_no(tipo: TipoNo, valor: str | None, linha: int) -> NoAst:
    return NoAst(tipo=tipo, valor=valor or "", linha=linha)


def adicionar_filho_programa(programa: NoAst | None, filho: NoAst | None) -> None:
    if programa is None or filho is None:
        return
    if programa.esquerda is None:
        programa.esquerda = filho
        return
    atual = programa.esquerda
    while atual.proximo is not None:
        atual = atual.proximo
    atual.proximo = filho


def nome_tipo_token(tipo: TipoToken) -> str:
    return tipo.value if isinstance(tipo, TipoToken) else TipoToken.INVALIDO.value


def nome_tipo_no(tipo: TipoNo) -> str:
    return tipo.value if isinstance(tipo, TipoNo) else TipoNo.INVALIDO.value


def nome_tipo_dado(tipo: TipoDado) -> str:
    return tipo.value if isinstance(tipo, TipoDado) else TipoDado.DESCONHECIDO.value


def tipo_numerico(tipo: TipoDado) -> bool:
    return tipo in (TipoDado.INTEIRO, TipoDado.REAL)
