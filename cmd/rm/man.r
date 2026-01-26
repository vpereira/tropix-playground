.bp
.he 'RM (cmd)'TROPIX: Manual de Referência'RM (cmd)'
.fo 'Atualizado em 25.01.26'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "rm -"
remove arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
rm [-firRvN] [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Para cada nome de <arquivo> dado, o comando "rm" remove a entrada
do seu respectivo diretório (elo físico).
No entando, o arquivo só é efetivamente removido se este elo físico era o último.
Se não era o caso, o arquivo é conservado,
mas não pode mais ser acessado pelo nome removido.

.sp
Se o nome do <arquivo> for o de um elo simbólico, este elo simbólico é removido,
e não o arquivo referenciado.

.sp
A remoção de um arquivo requer permissão de
escrita em seu diretório, mas não são necessárias
permissões de leitura nem de escrita no próprio arquivo.

.sp
Para maior segurança, o comando "rm" é normalmente interativo,
isto é, ele pede a confirmação do usuário para cada remoção.
Além disto, o usuário é normalmente informado caso o arquivo não possua
permissão de escrita (veja a opção "-f", abaixo).

.sp
As opções do comando são:

.in +3

.ip -f
Remove o arquivo sem pedir confirmação do usuário,
mesmo que o arquivo não possua permissão de escrita.
Esta opção é compatível com POSIX: arquivos inexistentes são
silenciosamente ignorados (sem mensagem de erro).

.sp
Um outro método de evitar as consultas é a de responder com letras
maiúsculas: com a resposta "S" ou "Y" todas as perguntas seguintes
serão automaticamente respondidas afirmativamente, e (analogamente)
com a resposta "N" serão respondidadas negativamente
(veja "askyesno" (libc)).

.ip -i
Pede confirmação do usuário para a remoção de cada arquivo
(modo normal).

.ip "-r, -R"
Remove diretórios e seus conteúdos recursivamente.
Quando um diretório é especificado, todo o seu conteúdo
(arquivos e subdiretórios) é removido antes do próprio diretório.
Esta opção é compatível com POSIX.

.ip -v
Lista os nomes dos <arquivo>s sendo removidos.
Isto é útil em conjunto com as opções "-f" e "-N".

.ip -N
Lê os nomes dos arquivos da entrada padrão.
Esta opção é implícita se não forem dados <arquivo>s.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Com a opção "-r" ou "-R", o comando "rm" pode remover diretórios e árvores,
comportamento equivalente ao antigo comando "rmtree".
Sem estas opções, "rm" recusa remover diretórios.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
rmdir, rmtree
.br
.wo "(sys): "
unlink, rmdir
.br
.wo "(libc):"
askyesno
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
