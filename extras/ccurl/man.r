.bp 1
.he 'CCURL (cmd)'TROPIX: Manual de Referencia'CCURL (cmd)'
.fo 'escrito em 09.01.26'Versao 1.0.0'Pag. %'

.b NOME
.in 5
ccurl - cliente HTTP simples

.in
.sp
.b SINTAXE
.in 5
ccurl [-v] [-o <arq>] <url>

.in
.sp
.b DESCRICAO
.in 5
O comando "ccurl" realiza uma requisicao HTTP/1.0 do tipo GET
e escreve o corpo da resposta na saida padrao ou em um arquivo.
O cliente usa a interface XTI (via "/dev/itntcp") para conexoes TCP.

.sp
Durante a transferencia, uma estatistica de progresso e exibida em stderr
periodicamente e ao final da copia. Com a opcao "-v", o comando imprime
informacoes de conexao e os cabecalhos HTTP.

.in
.sp
.b OPCOES
.in +3
.ip -v
Modo verboso.

.ip '-o <arq>'
Escreve a saida em <arq> em vez da saida padrao.
.ep
.in -3

.in
.sp
.b LIMITACOES
.in 5
HTTP apenas (sem HTTPS), sem autenticacao e sem redirecionamentos.

.in
.sp
.b EXEMPLOS
.in 7
.sp
ccurl http://example.com/index.html
.sp
.in +3
Baixa o conteudo e escreve na saida padrao.
.sp 2

.in -3
ccurl -o index.html http://example.com/index.html
.sp
.in +3
Salva o conteudo em "index.html".
.in -3

.in
.sp
.b
VEJA TAMBEM
.r
.in 5
(cmd): www

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
