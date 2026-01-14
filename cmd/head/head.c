/*
 ****************************************************************
 *								*
 *			head.c					*
 *								*
 *	Obtém a parte inicial de um arquivo			*
 *								*
 *	Versão	1.0.0, de 08.03.25				*
 *								*
 *	Módulo: head						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright (c) 2025 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

const char	pgversion[] = "Versão: 1.0.0, de 08.03.25";

#define	DEF_CHAR 'l'		/* Normalmente linhas */
#define	DEF_OFF	10		/* Normalmente 10 */

#define	ASZ	(4*1024)	/* Tamanho da area de leitura */

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	STDOUT	1

entry int	exit_code = 0;	/* Código de retorno */

/*
 ****** Prototipos de funcoes ***********************************
 */
void		help (void);
void		print_head (FILE *fp, off_t count, int mode);

/*
 ****************************************************************
 *	Imprime o início de um arquivo				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt;
	register FILE		*fp;
	register off_t		count = DEF_OFF;
	int			mode = DEF_CHAR;
	const char		*str, *b_str = NOSTR;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "b:n:c:H")) != EOF)
	{
		switch (opt)
		{
		    case 'b':			/* Definição do tamanho */
			b_str = optarg;
			break;

		    case 'n':			/* Número de linhas */
			mode = 'l';
			count = strtol (optarg, &str, 0);
			if (*str != '\0' || count < 0)
				help ();
			break;

		    case 'c':			/* Número de caracteres */
			mode = 'c';
			count = strtol (optarg, &str, 0);
			if (*str != '\0' || count < 0)
				help ();
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();
		}	/* end switch */
	}	/* end while */

	/*
	 *	Verifica o número de argumentos
	 */
	argv += optind;
	argc -= optind;

	if (argc > 1)
		help ();

	/*
	 *	Analisa a opção "-b"
	 */
	if (b_str)
	{
		count = strtol (b_str, &str, 0);

		if (count < 0)
			goto bad;

		if (str[0] != '\0')
		{
			mode = str[0];

			if (str[1] != '\0')
			{
			    bad:
				fprintf
				(	stderr,
					"Quantidade inválida: \"%s\"\n\n",
					b_str
				);
				help ();
			}
		}
	}

	if (mode == 'b')
		count *= BLSZ;
	elif (mode != 'l' && mode != 'c')
		help ();

	/*
	 *	Abre o arquivo
	 */
	if (argv[0] != NOSTR)
	{
		if ((fp = fopen (argv[0], "r")) == NOFILE)
		{
			fprintf
			(	stderr,
				"%s: Não consegui abrir \"%s\" (%s)\n",
				pgname, argv[0], strerror (errno)
			);
			exit (1);
		}
	}
	else
	{
		fp = stdin;
	}

	/*
	 *	Imprime o início
	 */
	print_head (fp, count, mode);

	return (exit_code);
}	/* end main */

/*
 ****************************************************************
 *	Lê e imprime o início do arquivo			*
 ****************************************************************
 */
void
print_head (FILE *fp, off_t count, int mode)
{
	char		area[ASZ];
	off_t		remaining = count;
	int		n;

	if (count <= 0)
		return;

	if (mode == 'l')
	{
		off_t	lines = 0;

		while ((n = read (fileno (fp), area, sizeof (area))) > 0)
		{
			int i;

			for (i = 0; i < n; i++)
			{
				if (area[i] == '\n')
				{
					lines++;
					if (lines >= count)
					{
						write (STDOUT, area, i + 1);
						return;
					}
				}
			}

			write (STDOUT, area, n);
		}

		return;
	}

	/* mode == 'c' or 'b' (already converted to bytes) */
	while (remaining > 0)
	{
		off_t want = remaining > (off_t)sizeof (area) ? (off_t)sizeof (area) : remaining;

		n = read (fileno (fp), area, (int)want);
		if (n <= 0)
			break;

		write (STDOUT, area, n);
		remaining -= n;
	}
}	/* end print_head */

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
		"%s - obtém o início de um arquivo\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-b <n>[lbc]] [-n <n>] [-c <n>] [<arquivo>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-b: Imprime <n> linhas (\"l\"), blocos (\"b\") ou\n"
		"\t    caracteres (\"c\") a partir do início do <arquivo>.\n"
		"\t    (se a opção não for dada, assume-se " XSTR (DEF_OFF) " linhas)\n"
		"\t-n: Atalho para número de linhas.\n"
		"\t-c: Atalho para número de caracteres.\n"
		"\nObs.:\tSe o <arquivo> não for dado, será lida a entrada padrão\n"
	);

	exit (2);
}	/* end help */
