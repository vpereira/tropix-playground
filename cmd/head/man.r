NOME
	head - obtém o início de um arquivo

SINOPSE
	head [-b <n>[lbc]] [-n <n>] [-c <n>] [<arquivo>]

DESCRICAO
	O comando head imprime a parte inicial de um arquivo. Quando
	não é informado um arquivo, a entrada padrão é utilizada.

OPCOES
	-b  Define o tamanho a imprimir, a partir do início do arquivo.
	    O sufixo pode ser:
	      l  linhas (padrão)
	      b  blocos
	      c  caracteres

	-n  Atalho para numero de linhas.
	-c  Atalho para numero de caracteres.

EXEMPLOS
	head -n 5 arquivo.txt
	head -b 2b arquivo.txt
	head -c 100 arquivo.txt
