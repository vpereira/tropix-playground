.bp
.he 'GETENTROPY (sys)'TROPIX: Manual de Referência'GETENTROPY (sys)'
.fo 'Atualizado em 12.01.26'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "getentropy -"
Obtém bytes aleatórios do núcleo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	getentropy (void *buf, int len);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "getentropy" copia "len" bytes para a área
apontada por "buf". Os bytes são gerados pelo gerador de números
pseudo-aleatórios do núcleo, o mesmo usado por /dev/urandom.

.sp
O tamanho máximo por chamada é de 256 bytes.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve zero.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b ERROS
.in 5
EINVAL	"len" é menor que zero ou maior que 256.
.br
EFAULT	"buf" aponta para uma área inválida.
.br

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
read
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
