.he 'WHOAMI (cmd)'TROPIX: Manual de Referência'WHOAMI (cmd)'
.fo 'Atualizado em 10.01.26'Versão 1.0.0'Pag. %'
.bp

.b NOME
.in 5
.wo "whoami -"
imprime o nome do usuário efetivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
whoami
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "whoami" imprime o nome do usuário efetivo
(effective user) do processo corrente.

.sp
Este comando é útil quando se quer verificar qual
identidade está sendo usada pelo processo,
especialmente depois de trocar de usuário com "su"
ou executar comandos com permissões especiais.

.sp
O usuário efetivo é aquele determinado pela
chamada ao sistema "geteuid".

.in
.sp
.b OBSERVAÇÃO
.in 5
Para ver informações completas sobre o usuário,
incluindo o usuário real, grupos, etc., use
o comando "id" (veja id(cmd)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
id, who, users
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
