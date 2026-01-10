.bp
.he 'FIND (cmd)'TROPIX: Manual de Referência'FIND (cmd)'
.fo 'Atualizado em 12.01.26'Versão 1.0.0'Pag. %'

.b NOME
.in 5
.wo "find -"
localiza arquivos e diretórios
.br

.in
.sp
.b SINTAXE
.in 5
.(l
find [<caminho> ...] [opções]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "find" percorre árvores de diretórios e aplica filtros
simples. Se nenhuma ação for especificada, imprime o caminho completo.

.sp
As opções suportadas são:

.in +3
.ip "-name <padrão>" 18
Seleciona pelo nome (suporta '*' e '?').

.ip "-type <c>" 18
Seleciona por tipo: f, d, l, b, c, p.

.ip "-mtime <n>" 18
Seleciona por dias desde a última modificação. Aceita +n, -n ou n.

.ip "-size <n>" 18
Seleciona por tamanho. Aceita +n, -n ou n e os sufixos c, b, k, m.

.ip "-maxdepth <n>" 18
Limita a profundidade de busca.

.ip "-mindepth <n>" 18
Ignora níveis menores que <n>.

.ip -print 18
Imprime o caminho (padrão).

.ip -delete 18
Remove os itens selecionados. Diretórios são removidos ao final.

.ip "-exec <cmd> ... ;" 18
Executa um comando por item. Use "{}" como substituição do caminho.

.ep
.in -3

.in
.sp
.b EXEMPLOS
.in 7
.sp
find . -name "*.c"
.sp
.in +3
Lista arquivos .c na árvore.
.sp 2

.in -3
find /etc -type f -mtime +7 -print
.sp
.in +3
Lista arquivos modificados há mais de 7 dias.
.sp 2

.in -3
find . -name "*.o" -delete
.sp
.in +3
Remove objetos na árvore.
.in -3

.in
.sp
.b LIMITAÇÕES
.in 5
Esta versão não implementa expressões complexas (-o, -a, !, -prune).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ls, rm
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
