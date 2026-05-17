PYTHON ?= python

PYTHON_SOURCES = \
	AnalisadorSemantico.py \
	funcoes_teste_semantico.py \
	analisador/__init__.py \
	analisador/arquivo_saida.py \
	analisador/artefatos.py \
	analisador/arvore_atribuida.py \
	analisador/assembly.py \
	analisador/cli.py \
	analisador/entrada.py \
	analisador/modelos.py \
	analisador/tabela_simbolos.py \
	analisador/tipos.py

.PHONY: all check test run clean

all: check

check:
	$(PYTHON) -m py_compile $(PYTHON_SOURCES)

run:
	$(PYTHON) AnalisadorSemantico.py teste1_valido.txt

test: check
	$(PYTHON) AnalisadorSemantico.py teste1_valido.txt
	-$(PYTHON) AnalisadorSemantico.py teste2_erros_semanticos.txt
	$(PYTHON) AnalisadorSemantico.py teste3_integracao.txt

clean:
	$(PYTHON) -c "import pathlib, shutil; [shutil.rmtree(p) for p in pathlib.Path('.').rglob('__pycache__')]"
