// Trabalho realizado individualmente:
// Daniel Campos Soares - daniSoares08
//
// Nome do grupo no Canvas: RA3_10

/* Preparacao da entrada para a analise semantica. */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analisador_semantico.h"

typedef struct {
    const Token *tokens;
    size_t quantidade;
    size_t posicao;
    EntradaSemantica *entrada;
} Parser;

void adicionar_erro(
    ErroAnalise *erros,
    size_t *quantidade,
    int linha,
    const char *elemento,
    const char *causa
) {
    ErroAnalise *erro;

    if (*quantidade >= MAX_ERROS) {
        return;
    }

    erro = &erros[*quantidade];
    erro->linha = linha;
    snprintf(erro->elemento, sizeof(erro->elemento), "%s", elemento ? elemento : "");
    snprintf(erro->causa, sizeof(erro->causa), "%s", causa ? causa : "");
    (*quantidade)++;
}

const char *nome_tipo_token(TipoToken tipo) {
    switch (tipo) {
        case TOKEN_ABRE_PAREN: return "ABRE_PAREN";
        case TOKEN_FECHA_PAREN: return "FECHA_PAREN";
        case TOKEN_NUMERO: return "NUMERO";
        case TOKEN_IDENTIFICADOR: return "IDENTIFICADOR";
        case TOKEN_OPERADOR_ARIT: return "OPERADOR_ARIT";
        case TOKEN_OPERADOR_REL: return "OPERADOR_REL";
        case TOKEN_RES: return "RES";
        case TOKEN_START: return "START";
        case TOKEN_END: return "END";
        case TOKEN_IF: return "IF";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_EOF: return "EOF";
        default: return "INVALIDO";
    }
}

const char *nome_tipo_no(TipoNo tipo) {
    switch (tipo) {
        case NO_PROGRAMA: return "PROGRAMA";
        case NO_NUMERO: return "NUMERO";
        case NO_VARIAVEL: return "VARIAVEL";
        case NO_ATRIBUICAO: return "ATRIBUICAO";
        case NO_RES: return "RES";
        case NO_BINARIO: return "BINARIO";
        case NO_IF: return "IF";
        case NO_WHILE: return "WHILE";
        default: return "INVALIDO";
    }
}

const char *nome_tipo_dado(TipoDado tipo) {
    switch (tipo) {
        case TIPO_INTEIRO: return "inteiro";
        case TIPO_REAL: return "real";
        case TIPO_BOOL: return "bool";
        case TIPO_ERRO: return "erro";
        default: return "desconhecido";
    }
}

int tipo_numerico(TipoDado tipo) {
    return tipo == TIPO_INTEIRO || tipo == TIPO_REAL;
}

NoAst *criar_no(TipoNo tipo, const char *valor, int linha) {
    NoAst *no = (NoAst *)calloc(1, sizeof(NoAst));

    if (no == NULL) {
        return NULL;
    }

    no->tipo = tipo;
    no->tipo_dado = TIPO_DESCONHECIDO;
    no->linha = linha;
    if (valor != NULL) {
        snprintf(no->valor, sizeof(no->valor), "%s", valor);
    }
    return no;
}

void adicionar_filho_programa(NoAst *programa, NoAst *filho) {
    NoAst *atual;

    if (programa == NULL || filho == NULL) {
        return;
    }

    if (programa->esquerda == NULL) {
        programa->esquerda = filho;
        return;
    }

    atual = programa->esquerda;
    while (atual->proximo != NULL) {
        atual = atual->proximo;
    }
    atual->proximo = filho;
}

void liberar_arvore(NoAst *no) {
    if (no == NULL) {
        return;
    }

    liberar_arvore(no->esquerda);
    liberar_arvore(no->direita);
    liberar_arvore(no->terceiro);
    liberar_arvore(no->proximo);
    free(no);
}

static char *ler_arquivo_inteiro(FILE *arquivo) {
    long tamanho;
    size_t lidos;
    char *conteudo;

    if (fseek(arquivo, 0, SEEK_END) != 0) {
        return NULL;
    }

    tamanho = ftell(arquivo);
    if (tamanho < 0) {
        return NULL;
    }

    if (fseek(arquivo, 0, SEEK_SET) != 0) {
        return NULL;
    }

    conteudo = (char *)malloc((size_t)tamanho + 1);
    if (conteudo == NULL) {
        return NULL;
    }

    lidos = fread(conteudo, 1, (size_t)tamanho, arquivo);
    conteudo[lidos] = '\0';
    return conteudo;
}

static void adicionar_token(
    EntradaSemantica *entrada,
    TipoToken tipo,
    const char *inicio,
    size_t tamanho,
    int linha,
    int coluna
) {
    Token *token;
    size_t copia;

    if (entrada->quantidade_tokens >= MAX_TOKENS) {
        adicionar_erro(
            entrada->erros_lexicos,
            &entrada->quantidade_erros_lexicos,
            linha,
            "tokens",
            "Quantidade maxima de tokens excedida."
        );
        return;
    }

    token = &entrada->tokens[entrada->quantidade_tokens++];
    memset(token, 0, sizeof(*token));
    token->tipo = tipo;
    token->linha = linha;
    token->coluna = coluna;
    copia = tamanho < sizeof(token->lexema) - 1 ? tamanho : sizeof(token->lexema) - 1;
    if (inicio != NULL && copia > 0) {
        memcpy(token->lexema, inicio, copia);
    }
    token->lexema[copia] = '\0';
}

static int eh_operador_arit(char c) {
    return c == '+' || c == '-' || c == '*' || c == '|' || c == '/' || c == '%' || c == '^';
}

static TipoToken tipo_identificador(const char *inicio, size_t tamanho) {
    if (tamanho == 3 && strncmp(inicio, "RES", 3) == 0) {
        return TOKEN_RES;
    }
    if (tamanho == 5 && strncmp(inicio, "START", 5) == 0) {
        return TOKEN_START;
    }
    if (tamanho == 3 && strncmp(inicio, "END", 3) == 0) {
        return TOKEN_END;
    }
    if (tamanho == 2 && strncmp(inicio, "IF", 2) == 0) {
        return TOKEN_IF;
    }
    if (tamanho == 5 && strncmp(inicio, "WHILE", 5) == 0) {
        return TOKEN_WHILE;
    }
    return TOKEN_IDENTIFICADOR;
}

static int tokenizar(EntradaSemantica *entrada) {
    const char *fonte = entrada->fonte;
    size_t i = 0;
    int linha = 1;
    int coluna = 1;

    while (fonte[i] != '\0') {
        char c = fonte[i];

        if (c == '\r') {
            i++;
            continue;
        }
        if (c == '\n') {
            linha++;
            coluna = 1;
            i++;
            continue;
        }
        if (isspace((unsigned char)c)) {
            coluna++;
            i++;
            continue;
        }
        if (c == '*' && fonte[i + 1] == '{') {
            int linha_inicio = linha;
            i += 2;
            coluna += 2;
            while (fonte[i] != '\0' && !(fonte[i] == '}' && fonte[i + 1] == '*')) {
                if (fonte[i] == '\n') {
                    linha++;
                    coluna = 1;
                    i++;
                } else {
                    coluna++;
                    i++;
                }
            }
            if (fonte[i] == '\0') {
                adicionar_erro(
                    entrada->erros_lexicos,
                    &entrada->quantidade_erros_lexicos,
                    linha_inicio,
                    "*{",
                    "Comentario iniciado por *{ sem fechamento }*."
                );
                return 1;
            }
            i += 2;
            coluna += 2;
            continue;
        }
        if (c == '(') {
            adicionar_token(entrada, TOKEN_ABRE_PAREN, "(", 1, linha, coluna);
            i++;
            coluna++;
            continue;
        }
        if (c == ')') {
            adicionar_token(entrada, TOKEN_FECHA_PAREN, ")", 1, linha, coluna);
            i++;
            coluna++;
            continue;
        }
        if (isdigit((unsigned char)c) || (c == '-' && isdigit((unsigned char)fonte[i + 1]))) {
            size_t inicio = i;
            int coluna_inicio = coluna;
            int pontos = 0;
            int expoente = 0;

            if (c == '-') {
                i++;
                coluna++;
            }
            while (isdigit((unsigned char)fonte[i]) || fonte[i] == '.' || fonte[i] == 'e' || fonte[i] == 'E' || fonte[i] == '+' || fonte[i] == '-') {
                if (fonte[i] == '.') {
                    pontos++;
                    if (pontos > 1) {
                        break;
                    }
                } else if (fonte[i] == 'e' || fonte[i] == 'E') {
                    expoente++;
                    if (expoente > 1) {
                        break;
                    }
                    i++;
                    coluna++;
                    if (fonte[i] == '+' || fonte[i] == '-') {
                        i++;
                        coluna++;
                    }
                    continue;
                } else if ((fonte[i] == '+' || fonte[i] == '-') && !(fonte[i - 1] == 'e' || fonte[i - 1] == 'E')) {
                    break;
                }
                i++;
                coluna++;
            }
            adicionar_token(entrada, TOKEN_NUMERO, fonte + inicio, i - inicio, linha, coluna_inicio);
            continue;
        }
        if (c >= 'A' && c <= 'Z') {
            size_t inicio = i;
            int coluna_inicio = coluna;
            while (fonte[i] >= 'A' && fonte[i] <= 'Z') {
                i++;
                coluna++;
            }
            if (isalnum((unsigned char)fonte[i]) || fonte[i] == '_') {
                adicionar_erro(
                    entrada->erros_lexicos,
                    &entrada->quantidade_erros_lexicos,
                    linha,
                    fonte + inicio,
                    "Identificador deve conter apenas letras latinas maiusculas."
                );
                return 1;
            }
            adicionar_token(
                entrada,
                tipo_identificador(fonte + inicio, i - inicio),
                fonte + inicio,
                i - inicio,
                linha,
                coluna_inicio
            );
            continue;
        }
        if ((c == '=' && fonte[i + 1] == '=') || (c == '!' && fonte[i + 1] == '=') ||
            (c == '>' && fonte[i + 1] == '=') || (c == '<' && fonte[i + 1] == '=')) {
            adicionar_token(entrada, TOKEN_OPERADOR_REL, fonte + i, 2, linha, coluna);
            i += 2;
            coluna += 2;
            continue;
        }
        if (c == '>' || c == '<') {
            adicionar_token(entrada, TOKEN_OPERADOR_REL, fonte + i, 1, linha, coluna);
            i++;
            coluna++;
            continue;
        }
        if (eh_operador_arit(c)) {
            adicionar_token(entrada, TOKEN_OPERADOR_ARIT, fonte + i, 1, linha, coluna);
            i++;
            coluna++;
            continue;
        }

        {
            char elemento[2] = {c, '\0'};
            adicionar_erro(
                entrada->erros_lexicos,
                &entrada->quantidade_erros_lexicos,
                linha,
                elemento,
                "Lexema invalido."
            );
        }
        return 1;
    }

    adicionar_token(entrada, TOKEN_EOF, "$", 1, linha, coluna);
    return entrada->quantidade_erros_lexicos > 0 ? 1 : 0;
}

static const Token *atual(Parser *parser) {
    if (parser->posicao >= parser->quantidade) {
        return &parser->tokens[parser->quantidade - 1];
    }
    return &parser->tokens[parser->posicao];
}

static int aceitar(Parser *parser, TipoToken tipo) {
    if (atual(parser)->tipo == tipo) {
        parser->posicao++;
        return 1;
    }
    return 0;
}

static void erro_sintatico(Parser *parser, const char *esperado) {
    const Token *token = atual(parser);
    char causa[TAMANHO_CAUSA];

    snprintf(
        causa,
        sizeof(causa),
        "Esperado %s, encontrado %s.",
        esperado,
        nome_tipo_token(token->tipo)
    );
    adicionar_erro(
        parser->entrada->erros_sintaticos,
        &parser->entrada->quantidade_erros_sintaticos,
        token->linha,
        token->lexema,
        causa
    );
}

static NoAst *parse_parenteses(Parser *parser);

typedef struct {
    NoAst *itens[8];
    size_t quantidade;
} ListaItens;

static void liberar_itens(ListaItens *lista) {
    size_t i;
    for (i = 0; i < lista->quantidade; i++) {
        liberar_arvore(lista->itens[i]);
    }
}

static NoAst *parse_item(Parser *parser) {
    const Token *token = atual(parser);
    NoAst *no;

    if (token->tipo == TOKEN_ABRE_PAREN) {
        return parse_parenteses(parser);
    }
    if (token->tipo == TOKEN_NUMERO) {
        parser->posicao++;
        return criar_no(NO_NUMERO, token->lexema, token->linha);
    }
    if (token->tipo == TOKEN_IDENTIFICADOR) {
        parser->posicao++;
        return criar_no(NO_VARIAVEL, token->lexema, token->linha);
    }
    if (token->tipo == TOKEN_OPERADOR_ARIT || token->tipo == TOKEN_OPERADOR_REL) {
        parser->posicao++;
        no = criar_no(NO_BINARIO, NULL, token->linha);
        if (no != NULL) {
            snprintf(no->operador, sizeof(no->operador), "%s", token->lexema);
        }
        return no;
    }
    if (token->tipo == TOKEN_RES) {
        parser->posicao++;
        return criar_no(NO_RES, token->lexema, token->linha);
    }
    if (token->tipo == TOKEN_IF) {
        parser->posicao++;
        return criar_no(NO_IF, token->lexema, token->linha);
    }
    if (token->tipo == TOKEN_WHILE) {
        parser->posicao++;
        return criar_no(NO_WHILE, token->lexema, token->linha);
    }

    erro_sintatico(parser, "expressao, identificador, numero, operador, IF, WHILE ou RES");
    parser->posicao++;
    return criar_no(NO_INVALIDO, token->lexema, token->linha);
}

static int item_mem_keyword(const NoAst *no) {
    return no != NULL && no->tipo == NO_VARIAVEL && strcmp(no->valor, "MEM") == 0;
}

static int item_operador(const NoAst *no) {
    return no != NULL && no->tipo == NO_BINARIO && no->operador[0] != '\0' &&
        no->esquerda == NULL && no->direita == NULL;
}

static NoAst *montar_expressao(Parser *parser, ListaItens *lista, int linha) {
    NoAst *resultado;

    if (lista->quantidade == 1) {
        resultado = lista->itens[0];
        lista->itens[0] = NULL;
        return resultado;
    }

    if (lista->quantidade == 2 && lista->itens[0]->tipo == NO_NUMERO && lista->itens[1]->tipo == NO_RES) {
        resultado = criar_no(NO_RES, lista->itens[0]->valor, lista->itens[0]->linha);
        liberar_itens(lista);
        return resultado;
    }

    if (lista->quantidade == 3 && item_operador(lista->itens[2])) {
        resultado = lista->itens[2];
        resultado->esquerda = lista->itens[0];
        resultado->direita = lista->itens[1];
        lista->itens[0] = NULL;
        lista->itens[1] = NULL;
        lista->itens[2] = NULL;
        return resultado;
    }

    if (lista->quantidade == 3 && item_mem_keyword(lista->itens[2]) && lista->itens[1]->tipo == NO_VARIAVEL) {
        resultado = criar_no(NO_ATRIBUICAO, lista->itens[1]->valor, lista->itens[1]->linha);
        resultado->esquerda = lista->itens[0];
        lista->itens[0] = NULL;
        liberar_itens(lista);
        return resultado;
    }

    if (lista->quantidade == 3 && (lista->itens[2]->tipo == NO_IF || lista->itens[2]->tipo == NO_WHILE)) {
        resultado = criar_no(lista->itens[2]->tipo, lista->itens[2]->valor, lista->itens[2]->linha);
        resultado->esquerda = lista->itens[0];
        resultado->direita = lista->itens[1];
        lista->itens[0] = NULL;
        lista->itens[1] = NULL;
        liberar_itens(lista);
        return resultado;
    }

    adicionar_erro(
        parser->entrada->erros_sintaticos,
        &parser->entrada->quantidade_erros_sintaticos,
        linha,
        "expressao",
        "Expressao RPN nao corresponde aos formatos da Fase 2."
    );
    liberar_itens(lista);
    return criar_no(NO_INVALIDO, "expressao", linha);
}

static NoAst *parse_parenteses(Parser *parser) {
    const Token *abre = atual(parser);
    ListaItens lista;
    memset(&lista, 0, sizeof(lista));

    if (!aceitar(parser, TOKEN_ABRE_PAREN)) {
        erro_sintatico(parser, "'('");
        return criar_no(NO_INVALIDO, "(", atual(parser)->linha);
    }

    while (atual(parser)->tipo != TOKEN_FECHA_PAREN && atual(parser)->tipo != TOKEN_EOF) {
        if (lista.quantidade >= sizeof(lista.itens) / sizeof(lista.itens[0])) {
            erro_sintatico(parser, "expressao RPN com ate 3 itens relevantes");
            break;
        }
        lista.itens[lista.quantidade++] = parse_item(parser);
    }

    if (!aceitar(parser, TOKEN_FECHA_PAREN)) {
        erro_sintatico(parser, "')'");
        liberar_itens(&lista);
        return criar_no(NO_INVALIDO, ")", abre->linha);
    }

    return montar_expressao(parser, &lista, abre->linha);
}

static int parse_marcador(Parser *parser, TipoToken esperado, const char *nome) {
    if (!aceitar(parser, TOKEN_ABRE_PAREN)) {
        erro_sintatico(parser, "'('");
        return 0;
    }
    if (!aceitar(parser, esperado)) {
        erro_sintatico(parser, nome);
        return 0;
    }
    if (!aceitar(parser, TOKEN_FECHA_PAREN)) {
        erro_sintatico(parser, "')'");
        return 0;
    }
    return 1;
}

static void parse_programa(EntradaSemantica *entrada) {
    Parser parser;
    NoAst *programa;
    int encontrou_end = 0;

    parser.tokens = entrada->tokens;
    parser.quantidade = entrada->quantidade_tokens;
    parser.posicao = 0;
    parser.entrada = entrada;

    programa = criar_no(NO_PROGRAMA, "PROGRAMA", 1);
    entrada->arvore.raiz = programa;
    snprintf(entrada->arvore.descricao, sizeof(entrada->arvore.descricao), "Programa RA3_10");

    if (!parse_marcador(&parser, TOKEN_START, "START")) {
        return;
    }

    while (atual(&parser)->tipo != TOKEN_EOF) {
        if (atual(&parser)->tipo == TOKEN_ABRE_PAREN &&
            parser.posicao + 1 < parser.quantidade &&
            parser.tokens[parser.posicao + 1].tipo == TOKEN_END) {
            parse_marcador(&parser, TOKEN_END, "END");
            encontrou_end = 1;
            break;
        }

        if (atual(&parser)->tipo != TOKEN_ABRE_PAREN) {
            erro_sintatico(&parser, "'(' iniciando comando ou (END)");
            parser.posicao++;
            continue;
        }

        adicionar_filho_programa(programa, parse_parenteses(&parser));
    }

    if (!encontrou_end) {
        adicionar_erro(
            entrada->erros_sintaticos,
            &entrada->quantidade_erros_sintaticos,
            atual(&parser)->linha,
            "$",
            "Programa deve terminar com (END)."
        );
    }
}

EntradaSemantica prepararEntradaSemantica(const char *arquivo) {
    /*
     * Carrega a entrada e prepara tokens/arvore para a etapa semantica.
     *
     * implementar conforme regras:
     * - Ler o arquivo de entrada recebido por argumento de linha de comando.
     * - Adaptar/reaproveitar o analisador lexico da Fase 1.
     * - Reconhecer comentarios iniciados por "*{" e terminados por "}*".
     * - Descartar comentarios do vetor de tokens usado pelo parser.
     * - Garantir comentarios em linha inteira, fim de linha e entre expressoes.
     * - Validar se o programa comeca com "(START)" e termina com "(END)".
     * - Acionar/reaproveitar o analisador sintatico LL(1) da Fase 2.
     * - Produzir a arvore sintatica inicial.
     * - Reportar erros lexicos e sintaticos antes da etapa semantica.
     */
    EntradaSemantica entrada;
    FILE *ponteiro_arquivo;

    memset(&entrada, 0, sizeof(entrada));
    snprintf(entrada.arquivo, sizeof(entrada.arquivo), "%s", arquivo ? arquivo : "");

    ponteiro_arquivo = fopen(arquivo, "rb");
    if (ponteiro_arquivo == NULL) {
        adicionar_erro(
            entrada.erros_lexicos,
            &entrada.quantidade_erros_lexicos,
            0,
            arquivo,
            "Arquivo de teste nao encontrado."
        );
        return entrada;
    }

    entrada.fonte = ler_arquivo_inteiro(ponteiro_arquivo);
    fclose(ponteiro_arquivo);

    if (entrada.fonte == NULL) {
        adicionar_erro(
            entrada.erros_lexicos,
            &entrada.quantidade_erros_lexicos,
            0,
            arquivo,
            "Nao foi possivel ler o arquivo de teste."
        );
        return entrada;
    }

    if (tokenizar(&entrada) == 0) {
        parse_programa(&entrada);
    }

    return entrada;
}

void liberarEntradaSemantica(EntradaSemantica *entrada) {
    if (entrada == NULL) {
        return;
    }

    free(entrada->fonte);
    entrada->fonte = NULL;
    liberar_arvore(entrada->arvore.raiz);
    entrada->arvore.raiz = NULL;
}
