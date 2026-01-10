/*
 ****************************************************************
 *								*
 *			cksum.c					*
 *								*
 *	Calcula checksums e contagem de bytes			*
 *								*
 *	Versão	1.0.0, de 10.01.26				*
 *								*
 *	Módulo: cksum						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2026 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "extern.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] = "Versão:	1.0.0, de 10.01.26";

#define	elif	else if
/*
 ******	Protótipos de funções ***********************************
 */
void	help (void);

/*
 ****************************************************************
 *	Calcula checksums e contagem de bytes			*
 ****************************************************************
 */
int
main (int argc, char **argv)
{
	unsigned long		len, val;
	int		ch, fd, rval;
	char		*fn;
	int		(*cfncn) (int, unsigned long *, unsigned long *);
	void		(*pfncn) (char *, unsigned long, unsigned long);

	cfncn = crc;
	pfncn = pcrc;

	while ((ch = getopt (argc, argv, "o:H")) != EOF)
	{
		switch (ch)
		{
		    case 'o':
			if (*optarg == '1')
			{
				cfncn = csum1;
				pfncn = psum1;
			}
			else if (*optarg == '2')
			{
				cfncn = csum2;
				pfncn = psum2;
			}
			else
			{
				fprintf
				(	stderr,
					"cksum: Argumento inválido para opção -o\n"
				);
				help ();
			}
			break;

		    case 'H':
			help ();

		    case '?':
		    default:
			help ();
		}
	}

	argc -= optind;
	argv += optind;

	fd = 0;
	fn = "stdin";
	rval = 0;

	do
	{
		if (*argv)
		{
			fn = *argv++;

			if ((fd = open (fn, O_RDONLY, 0)) < 0)
			{
				fprintf
				(	stderr,
					"cksum: %s: %s\n",
					fn, strerror (errno)
				);
				rval = 1;
				continue;
			}
		}

		if (cfncn (fd, &val, &len))
		{
			fprintf
			(	stderr,
				"cksum: %s: %s\n",
				fn, strerror (errno)
			);
			rval = 1;
		}
		else
		{
			pfncn (fn, val, len);
		}

		close (fd);

	} while (*argv);

	return (rval);

}	/* end main */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - calcula checksums e contagem de bytes\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-o 1|2] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-o: Seleciona algoritmo (1=sum1, 2=sum2, padrão=CRC)\n"
	);

	exit (2);

}	/* end help */
