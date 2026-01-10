/*
 ****************************************************************
 *								*
 *			mkdir.c					*
 *								*
 *	Cria diretórios						*
 *								*
 *	Versão	1.0.0, de 08.04.86				*
 *		4.2.0, de 10 05.02				*
 *		4.2.1, de 10 01.26				*
 *								*
 *	Módulo: mkdir						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.1, de 10.01.26";

#define	elif	else if

#define		MODO	0777	/* Modo do diretório criado */

entry int	fflag;		/* Cria os subdiretórios */
entry int	pflag;		/* Cria os subdiretórios (modo POSIX, não falha se existir) */
entry int	vflag;		/* Verbose */
entry int	dflag;		/* Debug */

/*
 ******	Protótipos de funções ***********************************
 */
int		ver_dir (char *);
void		help (void);

/*
 ****************************************************************
 *	Cria diretórios						*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, exit_code = 0;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "fpvdH")) != EOF)
	{
		switch (opt)
		{
		    case 'f':			/* Cria os subdiretórios */
			fflag++;
			break;

		    case 'p':			/* Cria os subdiretórios (POSIX) */
			pflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'd':			/* Debug */
			dflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica uso correto do programa
	 */
	if (argc < 1)
		help ();

	/*
	 *	Cria os diretórios
	 */
	for (/* vazio */; *argv != NOSTR; argv++)
	{
		if ((fflag || pflag) && ver_dir ((char *)*argv) < 0)
		{
			/* Se for modo POSIX e erro foi EEXIST, ignora */
			if (pflag && errno == EEXIST)
				continue;
			exit_code++;
			continue;
		}

		if (mkdir (*argv, MODO) < 0)
		{
			/* Se for modo POSIX e diretório já existe, não é erro */
			if (pflag && errno == EEXIST)
				continue;

			error ("*Não consegui criar o diretório \"%s\"", *argv);
			exit_code++;
		}
	}

	return (exit_code);

}	/* end main */

/*
 ****************************************************************
 *	Cria os diretórios intermediários faltando		*
 ****************************************************************
 */
int
ver_dir (char *file_nm)
{
	char		*cp;

	for (cp = file_nm + 1; *cp != '\0'; cp++)
	{
		if (*cp == '/')
		{
			*cp = '\0';

			if (access (file_nm, F_OK) < 0 && mkdir (file_nm, MODO) < 0)
			{
				error ("*Não consegui criar o diretório \"%s\"", file_nm);

				*cp = '/'; return (-1);
			}

			*cp = '/';
		}
	}

	return (0);

}	/* end ver_dir */

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
		"%s - cria diretórios\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-f|-p] <diretório> ...\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-f: Cria os diretórios intermediários (se necessário)\n"
		"\t-p: Cria os diretórios intermediários, não falha se já existir (POSIX)\n"
	);

	exit (2);

}	/* end help */
