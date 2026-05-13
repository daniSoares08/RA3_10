// Trabalho realizado individualmente:
// Daniel Campos Soares - daniSoares08
//
// Nome do grupo no Canvas: RA3_10

#ifndef ANALISADOR_SEMANTICO_H
#define ANALISADOR_SEMANTICO_H

#include <stddef.h>

#define TAMANHO_CAMINHO 512
#define TAMANHO_LEXEMA 128
#define TAMANHO_CAUSA 512
#define MAX_TOKENS 1024
#define MAX_ERROS 64
#define MAX_SIMBOLOS 128
#define MAX_RESULTADOS 256

typedef enum {
    TOKEN_ABRE_PAREN,
    TOKEN_FECHA_PAREN,
    TOKEN_NUMERO,
    TOKEN_IDENTIFICADOR,
    TOKEN_OPERADOR_ARIT,
    TOKEN_OPERADOR_REL,
    TOKEN_RES,
    TOKEN_START,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_WHILE,
    TOKEN_EOF,
    TOKEN_INVALIDO
} TipoToken;

typedef enum {
    NO_PROGRAMA,
    NO_NUMERO,
    NO_VARIAVEL,
    NO_ATRIBUICAO,
    NO_RES,
    NO_BINARIO,
    NO_IF,
    NO_WHILE,
    NO_INVALIDO
} TipoNo;

typedef enum {
    TIPO_DESCONHECIDO,
    TIPO_INTEIRO,
    TIPO_REAL,
    TIPO_BOOL,
    TIPO_ERRO
} TipoDado;

typedef struct {
    TipoToken tipo;
    char lexema[TAMANHO_LEXEMA];
    int linha;
    int coluna;
} Token;

typedef struct {
    int linha;
    char elemento[TAMANHO_LEXEMA];
    char causa[TAMANHO_CAUSA];
} ErroAnalise;

typedef struct NoAst {
    TipoNo tipo;
    TipoDado tipo_dado;
    char valor[TAMANHO_LEXEMA];
    char operador[TAMANHO_LEXEMA];
    int linha;
    struct NoAst *esquerda;
    struct NoAst *direita;
    struct NoAst *terceiro;
    struct NoAst *proximo;
} NoAst;

typedef struct {
    NoAst *raiz;
    char descricao[TAMANHO_CAUSA];
} ArvoreSintatica;

typedef struct {
    char arquivo[TAMANHO_CAMINHO];
    char *fonte;
    Token tokens[MAX_TOKENS];
    size_t quantidade_tokens;
    ArvoreSintatica arvore;
    ErroAnalise erros_lexicos[MAX_ERROS];
    size_t quantidade_erros_lexicos;
    ErroAnalise erros_sintaticos[MAX_ERROS];
    size_t quantidade_erros_sintaticos;
} EntradaSemantica;

typedef struct {
    char identificador[TAMANHO_LEXEMA];
    TipoDado tipo;
    char escopo[TAMANHO_LEXEMA];
    int linha_definicao;
    int linha_ultimo_uso;
    int linhas_uso[MAX_RESULTADOS];
    size_t quantidade_usos;
} Simbolo;

typedef struct {
    Simbolo simbolos[MAX_SIMBOLOS];
    size_t quantidade_simbolos;
    TipoDado resultados[MAX_RESULTADOS];
    size_t quantidade_resultados;
    ErroAnalise erros_semanticos[MAX_ERROS];
    size_t quantidade_erros_semanticos;
} ResultadoTabelaSimbolos;

typedef struct {
    TipoDado tipos[MAX_RESULTADOS];
    size_t quantidade_tipos;
    ErroAnalise erros_semanticos[MAX_ERROS];
    size_t quantidade_erros_semanticos;
} ResultadoTipos;

typedef struct {
    NoAst *raiz;
    char descricao[TAMANHO_CAUSA];
} ArvoreAtribuida;

typedef struct {
    char *codigo;
} ResultadoAssembly;

EntradaSemantica prepararEntradaSemantica(const char *arquivo);
void liberarEntradaSemantica(EntradaSemantica *entrada);

ResultadoTabelaSimbolos construirTabelaSimbolos(const ArvoreSintatica *arvore);
ResultadoTipos verificarTipos(
    const ArvoreSintatica *arvore,
    const ResultadoTabelaSimbolos *tabelaSimbolos
);
ArvoreAtribuida gerarArvoreAtribuida(
    const ArvoreSintatica *arvore,
    const ResultadoTabelaSimbolos *tabelaSimbolos,
    const ResultadoTipos *tipos
);
ResultadoAssembly gerarAssembly(const ArvoreAtribuida *arvoreAtribuida);
void liberarAssembly(ResultadoAssembly *assembly);

int salvarArtefatosUltimaExecucao(
    const char *arquivo_teste,
    const ResultadoTabelaSimbolos *tabela_simbolos,
    const ErroAnalise *erros_semanticos,
    size_t quantidade_erros_semanticos,
    const ArvoreAtribuida *arvore_atribuida,
    const ResultadoAssembly *assembly
);

void formatar_caminhos_saida(void);

void validar_erros_lexicos(void);
void validar_erros_sintaticos(void);
void validar_erros_semanticos(void);
void validar_estruturas_controle_validas(void);
void validar_erros_semanticos_estruturas_controle(void);
void validar_erros_tipos_expressoes_aninhadas(void);
void validar_casos_extremos(void);

NoAst *criar_no(TipoNo tipo, const char *valor, int linha);
void adicionar_filho_programa(NoAst *programa, NoAst *filho);
void liberar_arvore(NoAst *no);

void adicionar_erro(
    ErroAnalise *erros,
    size_t *quantidade,
    int linha,
    const char *elemento,
    const char *causa
);

const char *nome_tipo_dado(TipoDado tipo);
const char *nome_tipo_no(TipoNo tipo);
const char *nome_tipo_token(TipoToken tipo);
int tipo_numerico(TipoDado tipo);

#endif
