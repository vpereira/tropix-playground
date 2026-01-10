/*
 ****************************************************************
 *								*
 *			xargs.c					*
 *								*
 *	Constrói e executa linhas de comando a partir da	*
 *	entrada padrão						*
 *								*
 *	Versão	1.0.0, de 10.01.26				*
 *								*
 *	Módulo: xargs						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2026 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] = "Versão:	1.0.0, de 10.01.26";

#define	elif	else if
#define NOSTR	(char *)NULL

#define DEFAULT_CMD	"/bin/echo"
#define MAX_ARGS	5000
#define MAX_LINE	10240

/*
 * wait(2) status decoding (see lib/libc/sys/wait/man.r)
 */
#define	WIFEXITED(st)	(((st) & 0xFF) == 0)
#define	WEXITSTATUS(st)	(((st) >> 8) & 0xFF)
#define	WIFSIGNALED(st)	(((st) & 0xFF) != 0 && ((st) & 0xFF) != 0x3F)

entry int	fflag;		/* Não termina se comando falhar */
entry int	tflag;		/* Imprime comando antes de executar */

/*
 ******	Protótipos de funções ***********************************
 */
void		run (char **);
void		error (const char *, ...);
void		help (void);

/*
 ****************************************************************
 *	Constrói e executa linhas de comando			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, ch;
	char		*p, *bbp, *ebp, **bxp, **exp, **xp;
	int		cnt, indouble, insingle, nargs, nflag, nline, xflag;
	char		**av, *argp;

	/*
	 *	Limites padrão de argumentos e tamanho de linha
	 */
	nargs = MAX_ARGS;
	nline = MAX_LINE;
	nflag = xflag = 0;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "fn:s:txH")) != EOF)
	{
		switch (opt)
		{
		    case 'f':
			fflag = 1;
			break;

		    case 'n':
			nflag = 1;
			if ((nargs = atoi (optarg)) <= 0)
				error ("Número de argumentos inválido");
			break;

		    case 's':
			if ((nline = atoi (optarg)) <= 0)
				error ("Tamanho de linha inválido");
			break;

		    case 't':
			tflag = 1;
			break;

		    case 'x':
			xflag = 1;
			break;

		    case 'H':
			help ();

		    default:
			putc ('\n', stderr);
			help ();
		}
	}

	argc -= optind;
	argv += optind;

	if (xflag && !nflag)
		help ();

	/*
	 *	Aloca espaço para os ponteiros de argumentos
	 */
	av = bxp = malloc ((1 + argc + nargs + 1) * sizeof (char *));
	
	if (av == NOSTR)
		error ("%s", strerror (errno));

	/*
	 *	Obtém o comando a executar (echo se não especificado)
	 */
	if (*argv == NOSTR)
	{
		cnt = strlen (*bxp++ = DEFAULT_CMD);
	}
	else
	{
		cnt = 0;
		do
		{
			cnt += strlen (*bxp++ = (char *)*argv) + 1;
		}
		while (*++argv != NOSTR);
	}

	/*
	 *	Configura ponteiros para os argumentos da entrada
	 */
	exp = (xp = bxp) + nargs;

	/*
	 *	Aloca espaço para o buffer de argumentos lidos
	 */
	nline -= cnt;
	
	if (nline <= 0)
		error ("Espaço insuficiente para o comando");

	bbp = malloc (nline + 1);
	
	if (bbp == NOSTR)
		error ("%s", strerror (errno));

	ebp = (argp = p = bbp) + nline - 1;

	/*
	 *	Lê e processa a entrada
	 */
	for (insingle = indouble = 0; ; )
	{
		switch (ch = getchar ())
		{
		    case EOF:
			/* Sem argumentos desde a última execução */
			if (p == bbp)
				exit (0);

			/* Nada desde o fim do último argumento */
			if (argp == p)
			{
				*xp = NOSTR;
				run (av);
				exit (0);
			}
			goto arg1;

		    case ' ':
		    case '\t':
			/* Aspas escapam espaços e tabs */
			if (insingle || indouble)
				goto addch;
			goto arg2;

		    case '\n':
			/* Linhas vazias são ignoradas */
			if (argp == p)
				continue;

			/* Aspas não escapam newlines */
arg1:			if (insingle || indouble)
				error ("Aspas não terminadas");

arg2:			*p = '\0';
			*xp++ = argp;

			/*
			 *	Executa comando se buffer cheio, max args, ou EOF
			 */
			if (xp == exp || p == ebp || ch == EOF)
			{
				if (xflag && xp != exp && p == ebp)
					error ("Espaço insuficiente para argumentos");

				*xp = NOSTR;
				run (av);

				if (ch == EOF)
					exit (0);

				p = bbp;
				xp = bxp;
			}
			else
			{
				++p;
			}

			argp = p;
			break;

		    case '\'':
			if (indouble)
				goto addch;
			insingle = !insingle;
			break;

		    case '"':
			if (insingle)
				goto addch;
			indouble = !indouble;
			break;

		    case '\\':
			/* Backslash escapa qualquer caractere */
			if (!insingle && !indouble && (ch = getchar ()) == EOF)
				error ("Backslash no fim do arquivo");
			/* FALLTHROUGH */

		    default:
addch:			if (p < ebp)
			{
				*p++ = ch;
				break;
			}

			/* Se apenas um argumento, buffer insuficiente */
			if (bxp == xp)
				error ("Espaço insuficiente para argumento");

			/* Se xflag, falha */
			if (xflag)
				error ("Espaço insuficiente para argumentos");

			*xp = NOSTR;
			run (av);
			xp = bxp;
			cnt = ebp - argp;
			memmove (bbp, argp, cnt);
			p = (argp = bbp) + cnt;
			*p++ = ch;
			break;
		}
	}

	return (0);

}	/* end main */

/*
 ****************************************************************
 *	Executa o comando com os argumentos			*
 ****************************************************************
 */
void
run (char **argv)
{
	char	**p;
	int	pid;
	int	noinvoke;
	int	status;

	/*
	 *	Imprime comando se -t
	 */
	if (tflag)
	{
		fprintf (stderr, "%s", *argv);
		
		for (p = argv + 1; *p != NOSTR; ++p)
			fprintf (stderr, " %s", *p);
		
		fprintf (stderr, "\n");
		fflush (stderr);
	}

	/*
	 *	Executa o comando
	 */
	noinvoke = 0;

	switch (pid = fork ())
	{
	    case -1:
		error ("fork: %s", strerror (errno));

	    case 0:
		execvp (argv[0], argv);
		fprintf (stderr, "xargs: %s: %s\n", argv[0], strerror (errno));
		noinvoke = 1;
		_exit (1);
	}

	/*
	 *	Aguarda término do processo filho
	 */
	while (wait (&status) != pid)
		continue;

	/*
	 *	Verifica status de saída
	 */
	if (noinvoke || !WIFEXITED (status) || WIFSIGNALED (status))
		exit (127);

	if (!fflag && WEXITSTATUS (status))
		exit (WEXITSTATUS (status));

}	/* end run */

/*
 ****************************************************************
 *	Imprime mensagem de erro e termina			*
 ****************************************************************
 */
void
error (const char *fmt, ...)
{
	va_list	ap;

	va_start (ap, fmt);
	fprintf (stderr, "xargs: ");
	vfprintf (stderr, fmt, ap);
	va_end (ap);
	fprintf (stderr, "\n");
	exit (1);

}	/* end error */

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
		"%s - constrói e executa linhas de comando\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-ft] [[-x] -n número] [-s tamanho] [comando [arg ...]]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-f: Não termina se comando falhar\n"
		"\t-n: Máximo de argumentos por execução\n"
		"\t-s: Tamanho máximo da linha de comando\n"
		"\t-t: Imprime comando antes de executar\n"
		"\t-x: Termina se tamanho excedido (requer -n)\n"
	);

	exit (2);

}	/* end help */
