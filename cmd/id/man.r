.he 'ID (cmd)'TROPIX: Manual de Referência'ID (cmd)'
.fo 'Atualizado em 10.01.26'Versão 1.0.0'Pag. %'
.bp

.b NOME
.in 5
.wo "id -"
imprime informações de identificação do usuário
.br

.in
.sp
.b SINTAXE
.in 5
.(l
id [opções]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "id" imprime informações sobre a identificação
do usuário, incluindo UID (user ID) e GID (group ID).

.sp
Por padrão, "id" mostra as informações do processo corrente,
incluindo usuário efetivo (effective), usuário real (real),
grupo efetivo e grupo real.

.sp
As opções do comando são:

.in +3
.ip -g
Imprime apenas o GID do grupo efetivo (ou real, se usado com -r).

.ip -n
Usado com -g ou -u para imprimir nomes em vez de números.

.ip -r
Usado com -g ou -u para imprimir o ID real em vez do efetivo.

.ip -u
Imprime apenas o UID do usuário efetivo (ou real, se usado com -r).

.ep
.in -3

.in
.sp
.b EXEMPLOS
.in 5
.nf
	id
.fi
.in +3
Mostra informações completas do usuário corrente.
.in -3

.sp
.nf
	id -un
.fi
.in +3
Imprime o nome do usuário efetivo.
.in -3

.sp
.nf
	id -gn
.fi
.in +3
Imprime o nome do grupo efetivo.
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
O usuário "efetivo" (effective) é usado para verificação de permissões
de acesso, enquanto o usuário "real" é o que iniciou o processo.
Normalmente são iguais, exceto em programas SUID ou após "su".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
getuid, getgid
.br
.wo "(cmd): "
whoami, who
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
