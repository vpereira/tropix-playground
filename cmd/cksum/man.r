.he 'CKSUM (cmd)'TROPIX: Manual de Referência'CKSUM (cmd)'
.fo 'Atualizado em 10.01.26'Versão 1.0.0'Pag. %'
.bp

.b NOME
.in 5
.wo "cksum -"
calcula checksums e contagem de bytes de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
cksum [-o 1|2] [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "cksum" calcula e imprime um checksum CRC e a contagem
de bytes para cada <arquivo> dado, ou da entrada padrão
se nenhum arquivo for especificado.

.sp
O checksum CRC é calculado usando o polinômio do padrão POSIX
1003.2, garantindo portabilidade entre diferentes sistemas.

.sp
Para cada arquivo processado, "cksum" imprime o valor do checksum,
o número de bytes, e o nome do arquivo (se fornecido).

.sp
As opções do comando são:

.in +3
.ip "-o 1"
Calcula o checksum usando o algoritmo histórico "sum"
(16 bits), compatível com o comando tradicional Unix "sum".

.ip "-o 2"
Calcula o checksum usando um algoritmo alternativo
(32 bits), compatível com versões antigas do BSD.

.ep
.in -3

.in
.sp
.b EXEMPLOS
.in 5
.nf
	cksum arquivo.txt
.fi
.in +3
Calcula o checksum CRC de "arquivo.txt".
.in -3

.sp
.nf
	cksum -o 1 *.c
.fi
.in +3
Calcula o checksum usando o algoritmo sum (16 bits)
para todos os arquivos .c do diretório corrente.
.in -3

.sp
.nf
	cat arquivo | cksum
.fi
.in +3
Calcula o checksum da entrada padrão.
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
O algoritmo CRC padrão produz um valor de 32 bits que pode ser usado
para detectar erros de transmissão ou corrupção de dados.

.sp
Os algoritmos alternativos (-o 1 e -o 2) são fornecidos para
compatibilidade com versões históricas do Unix, mas o CRC padrão
é recomendado para uso geral por ser mais robusto.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sum, md5, wc
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
