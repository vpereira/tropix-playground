.bp
.he 'TOP (cmd)'TROPIX: Manual de Referencia'TOP (cmd)'
.fo 'Atualizado em 25.01.26'Versao 1.0.0'Pag. %'

.b NOME
.in 5
.wo "top  -"
lista processos em tela cheia
.br

.in
.sp
.b SINTAXE
.in 5
.(l
top [-ax] [-d <seg>]
.)l

.in
.sp
.b DESCRICAO
.in 5
O comando "top" mostra uma lista simples de processos em modo
interativo, atualizando a tela periodicamente. A lista e ordenada
por tempo de CPU total (utime + stime).

.sp
Teclas:
.in +3
.ip q 6
Sai do programa.
.ip r 6
Atualiza imediatamente.
.ip ? 6
Mostra ajuda curta no rodape.
.in -3

.in
.sp
.b OPCOES
.in 5
.ip -a 6
Lista todos os processos.
.ip -x 6
Inclui processos sem TTY associado.
.ip -d 6
Define o intervalo de atualizacao, em segundos (padrao 2).

.in
.sp
.b VEJA TAMBEM
.in 5
.wo "(cmd):  "
ps
