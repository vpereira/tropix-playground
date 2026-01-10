.he 'MKDIR (cmd)'TROPIX: Manual de Refer�ncia'MKDIR (cmd)'
.fo 'Atualizado em 10.05.02'Vers�o 4.2.0'Pag. %'
.bp

.b NOME
.in 5
.wo "mkdir -"
cria diret�rios
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mkdir [-f|-p] <diret�rio> ...
.)l

.in
.sp
.b DESCRI��O
.in 5
O comando "mkdir" cria os <diret�rio>s dados.

.sp
As entradas "." (o pr�prio diret�rio) e ".." (o pai do diret�rio)
s�o criadas automaticamente.

.sp
A cria��o de um diret�rio requer a permiss�o de escrita no pai
do diret�rio que se deseja criar.

.sp
As op��es do comando s�o:

.in +3
.ip -f
Cria os diret�rios intermedi�rios (se necess�rio).

.sp
Normalmente, "mkdir" espera que os diret�rios intermedi�rios
j� existam (por exemplo, no comando "mkdir alpha/beta",
o comando espera que "alpha" j� exista).

.sp
Com esta op��o,
"mkdir" cria tamb�m estes diret�rios intermedi�rios faltando.

.sp
Se o diret�rio final j� existir, o comando retornar� erro.

.ip -p
Cria os diret�rios intermedi�rios (se necess�rio), modo POSIX.

.sp
Esta op��o funciona como "-f", mas com comportamento POSIX:
se o diret�rio final j� existir, o comando n�o retorna erro
e termina com c�digo de sa�da 0 (sucesso).

.sp
Esta � a op��o padr�o em sistemas Unix/POSIX e �til em scripts,
permitindo que "mkdir -p" seja executado de forma idempotente.

.ep
.in -3

.in
.sp
.b
VEJA TAMB�M
.r
.in 5
.wo "(sys): "
mkdir
.br
.wo "(cmd): "
rm, rmdir
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
