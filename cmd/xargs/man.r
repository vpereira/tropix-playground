.he 'XARGS (cmd)'TROPIX: Manual de Referência'XARGS (cmd)'
.fo 'Atualizado em 10.01.26'Versão 1.0.0'Pag. %'
.bp

.b NOME
.in 5
.wo "xargs -"
constrói e executa linhas de comando a partir da entrada padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
xargs [-ft] [[-x] -n <número>] [-s <tamanho>] [<utilitário> [<argumento> ...]]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "xargs" lê itens da entrada padrão, delimitados por
espaços, tabulações ou newlines, e executa o <utilitário>
especificado uma ou mais vezes com os itens como argumentos.

.sp
Se nenhum utilitário for especificado, "xargs" usa "echo" como padrão.

.sp
O comando "xargs" é frequentemente usado em conjunto com "find"
para processar listas de arquivos, mas pode ser usado com
qualquer comando que gere listas de itens.

.sp
As opções do comando são:

.in +3
.ip -f
Força a execução do utilitário mesmo que ele retorne
código de saída diferente de zero. Normalmente, "xargs"
para a execução após o primeiro erro.

.ip -n
Especifica o número máximo de argumentos retirados da entrada
padrão para cada invocação do utilitário. Se mais argumentos
forem lidos, o utilitário será invocado múltiplas vezes.

.ip -s
Especifica o tamanho máximo (em bytes) da linha de comando
que será passada ao utilitário. O padrão é ARG_MAX - 4096.

.ip -t
Imprime a linha de comando na saída de erro padrão antes
de executá-la (modo trace/verbose).

.ip -x
Usado com -n. Força "xargs" a terminar com erro se o
tamanho da linha especificado por -s for excedido.

.ep
.in -3

.in
.sp
.b EXEMPLOS
.in 5
.nf
	find . -name "*.tmp" | xargs rm
.fi
.in +3
Remove todos os arquivos .tmp encontrados.
.in -3

.sp
.nf
	echo arquivo1 arquivo2 arquivo3 | xargs cat
.fi
.in +3
Concatena os três arquivos especificados.
.in -3

.sp
.nf
	find . -name "*.c" | xargs -n 1 grep "main"
.fi
.in +3
Procura "main" em cada arquivo .c individualmente
(um arquivo por invocação do grep).
.in -3

.sp
.nf
	ls *.txt | xargs -t -n 2 diff
.fi
.in +3
Compara arquivos .txt dois a dois, mostrando os comandos
executados (modo trace).
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
O comando "xargs" limita o tamanho da linha de comando
para evitar o erro "Argument list too long" (E2BIG).

.sp
Aspas simples (') e duplas (") podem ser usadas na entrada
para preservar espaços dentro de argumentos.

.sp
Barra invertida (\) funciona como caractere de escape.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
find, echo, exec
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
