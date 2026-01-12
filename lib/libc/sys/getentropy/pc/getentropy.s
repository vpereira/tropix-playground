|*
|****************************************************************
|*							*
|*			getentropy.s				*
|*							*
|*	Obtém bytes aleatórios do núcleo		*
|*							*
|*	Versão	4.6.0, de 12.01.26		*
|*		4.6.0, de 12.01.26		*
|*							*
|*	Módulo: getentropy				*
|*		libc/sys				*
|*	Categoria B					*
|*							*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*	Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 							*
|****************************************************************
|*

GETENTROPY	= 88
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "getentropy" *************************
|*
|*	int	getentropy (void *buf, int len);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_getentropy
_getentropy:
	movl	#GETENTROPY,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
