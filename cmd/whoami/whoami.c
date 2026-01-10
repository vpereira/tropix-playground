/*
 ****************************************************************
 *								*
 *			whoami.c				*
 *								*
 *	Imprime o nome do usuário efetivo			*
 *								*
 *	Versão	1.0.0, de 10.01.26				*
 *								*
 *	Módulo: whoami						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2026 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] = "Versão:	1.0.0, de 10.01.26";

#define NOSTR (char *)NULL

/*
 ****************************************************************
 *	Imprime o nome do usuário efetivo			*
 ****************************************************************
 */
int
main (void)
{
	PASSWD		*pw;
	int		uid;

	uid = geteuid ();

	if ((pw = getpwuid (uid)) == NOPASSWD)
	{
		fprintf
		(	stderr,
			"whoami: Não há login associado com o UID %u\n",
			uid
		);
		exit (1);
	}

	printf ("%s\n", pw->pw_name);

	return (0);

}	/* end main */
