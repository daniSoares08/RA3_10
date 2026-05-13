CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -Iinclude
TARGET ?= AnalisadorSemantico

SOURCES = \
	AnalisadorSemantico.c \
	src/arquivo_saida.c \
	src/artefatos.c \
	src/arvore_atribuida.c \
	src/assembly.c \
	src/entrada.c \
	src/funcoes_teste_semantico.c \
	src/tabela_simbolos.c \
	src/tipos.c

OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -f $(TARGET) $(OBJECTS)
