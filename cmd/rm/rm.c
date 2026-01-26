/*
 ****************************************************************
 *								*
 *			rm.c					*
 *								*
 *	Remove arquivos						*
 *								*
 *	Versão	1.0.0, de 13.05.86				*
 *		4.3.0, de 25.01.26				*
 *								*
 *	Módulo: rm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002-2026 NCE/UFRJ			*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.3.0, de 25.01.26";

#define	elif	else if

entry int	vflag;		/* Verbose */
entry int	fflag;		/* opção '-f' */
entry int	rflag;		/* opção '-r' ou '-R' (recursivo) */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

const char	**color_vector;	/* Para as cores dos modos */

/*
 ******	Protótipos de funções ***********************************
 */
int		do_remove (const char *);
int		remove_file (const char *, STAT *);
int		remove_dir (const char *, const STAT *);
int		remove_dir_contents (const char *, const STAT *);
void		help (void);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, ret = 0;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "firRvNH")) != EOF)
	{
		switch (opt)
		{
		    case 'f':			/* Força */
			fflag++;
			break;

		    case 'i':			/* Interativo */
			fflag = 0;
			break;

		    case 'r':			/* Recursivo */
		    case 'R':
			rflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;

	/*
	 *	Prepara o terminal para consultas
	 */
	if (!fflag)
	{
		if ((askyesno_stdin = fopen ("/dev/tty", "r")) == NOFILE)
			error ("$*Não consegui abrir \"/dev/tty\"");

		askyesno_set (1);	/* Permite o modo "de força" */
	}

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stderr)), (void *)NULL, &color_vector);

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
		{ error ("Os argumentos supérfluos serão ignorados"); *argv = NOSTR; }

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR)		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
			ret += do_remove (area);
	}
	else				/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
			ret += do_remove (*argv);
	}

	return (ret);

}	/* end main */

/*
 ****************************************************************
 *	Analisa a remoção					*
 ****************************************************************
 */
int
do_remove (const char *file_nm)
{
	STAT		s;
	int		code, save_errno;

	/*
	 *	Obtém o estado do arquivo
	 */
	code = lstat (file_nm, &s);
	save_errno = errno;

	/*
	 *	Se não existe e -f, ignora silenciosamente (POSIX)
	 */
	if (code < 0)
	{
		if (fflag)
			return (0);

		errno = save_errno;
		error ("*Não consegui obter o estado de \"%s\"", file_nm);
		return (1);
	}

	/*
	 *	Se é diretório
	 */
	if (S_ISDIR (s.st_mode))
	{
		if (!rflag)
		{
			error ("\"%s\" é um diretório (use -r para remover)", file_nm);
			return (1);
		}

		return (remove_dir_contents (file_nm, &s));
	}

	/*
	 *	Arquivo regular ou link
	 */
	return (remove_file (file_nm, &s));

}	/* end do_remove */

/*
 ****************************************************************
 *	Remove um arquivo					*
 ****************************************************************
 */
int
remove_file (const char *file_nm, STAT *sp)
{
	/*
	 *	Modo interativo ou verbose
	 */
	if (!fflag || vflag)
	{
		fprintf (stderr, "%c", modetostr (sp->st_mode)[0]);

		if (S_ISREG (sp->st_mode))
			fprintf (stderr, " %d", sp->st_size);

		fprintf
		(	stderr,
			" %s%s%s",
			color_vector[(sp->st_mode & S_IFMT) >> 12],
			file_nm,
			color_vector[MC_DEF]
		);

		if (!fflag)
		{
			fprintf (stderr, "? (n): ");

			if (askyesno () <= 0)
				return (0);
		}
		else
		{
			fprintf (stderr, "\n");
		}
	}

	/*
	 *	Verifica permissão de escrita
	 */
	if (!fflag && !S_ISLNK (sp->st_mode) && access (file_nm, W_OK) < 0)
	{
		fprintf
		(	stderr,
			"%s: \"%s\" não tem permissão de escrita. Remove? (n): ",
			pgname, file_nm
		);

		if (askyesno () <= 0)
			return (0);
	}

	/*
	 *	Remove
	 */
	if (unlink (file_nm) < 0)
	{
		error ("*Não consegui remover \"%s\"", file_nm);
		return (1);
	}

	if (sp->st_nlink-- > 1)
		error ("Ainda há %d elo(s) físico(s) para \"%s\"", sp->st_nlink, file_nm);

	return (0);

}	/* end remove_file */

/*
 ****************************************************************
 *	Remove o conteúdo de um diretório recursivamente	*
 ****************************************************************
 */
int
remove_dir_contents (const char *dir_path, const STAT *sp)
{
	DIR		*dir_fp;
	const DIRENT	*dp;
	STAT		s;
	char		*child_path;
	int		ret = 0;
	int		dir_len;

	/*
	 *	Modo interativo
	 */
	if (!fflag)
	{
		fprintf
		(	stderr,
			"Diretório \"%s%s%s\", entra? (n): ",
			color_vector[S_IFDIR >> 12],
			dir_path,
			color_vector[MC_DEF]
		);

		if (askyesno () <= 0)
			return (0);
	}
	elif (vflag)
	{
		fprintf
		(	stderr,
			"Diretório \"%s%s%s\":\n",
			color_vector[S_IFDIR >> 12],
			dir_path,
			color_vector[MC_DEF]
		);
	}

	/*
	 *	Abre o diretório
	 */
	if ((dir_fp = inopendir (sp->st_dev, sp->st_ino)) == NODIR)
	{
		error ("*Não consegui abrir o diretório \"%s\"", dir_path);
		return (1);
	}

	dir_len = strlen (dir_path);

	/*
	 *	Lê e processa cada entrada
	 */
	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		/* Pula "." e ".." */
		if (dp->d_name[0] == '.')
		{
			if   (dp->d_name[1] == '\0')
				continue;
			elif (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
				continue;
		}

		/*
		 *	Monta o caminho completo
		 */
		child_path = alloca (dir_len + dp->d_namlen + 2);
		strcpy (child_path, dir_path);
		strcat (child_path, "/");
		strcat (child_path, dp->d_name);

		/*
		 *	Obtém o estado
		 */
		if (instat (sp->st_dev, dp->d_ino, &s) < 0)
		{
			error ("*Não consegui obter o estado de \"%s\"", child_path);
			ret++;
			continue;
		}

		/*
		 *	Processa arquivo ou diretório
		 */
		if (S_ISDIR (s.st_mode))
			ret += remove_dir_contents (child_path, &s);
		else
			ret += remove_file (child_path, &s);
	}

	closedir (dir_fp);

	/*
	 *	Remove o diretório se ficou vazio
	 */
	if (ret == 0)
		ret = remove_dir (dir_path, sp);

	return (ret);

}	/* end remove_dir_contents */

/*
 ****************************************************************
 *	Remove um diretório vazio				*
 ****************************************************************
 */
int
remove_dir (const char *dir_path, const STAT *sp)
{
	/*
	 *	Modo interativo
	 */
	if (!fflag)
	{
		fprintf
		(	stderr,
			"Diretório \"%s%s%s\", remove? (n): ",
			color_vector[S_IFDIR >> 12],
			dir_path,
			color_vector[MC_DEF]
		);

		if (askyesno () <= 0)
			return (1);
	}
	elif (vflag)
	{
		fprintf
		(	stderr,
			"Removendo diretório \"%s%s%s\"\n",
			color_vector[S_IFDIR >> 12],
			dir_path,
			color_vector[MC_DEF]
		);
	}

	/*
	 *	Remove
	 */
	if (rmdir (dir_path) < 0)
	{
		error ("*Não consegui remover o diretório \"%s\"", dir_path);
		return (1);
	}

	return (0);

}	/* end remove_dir */

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
		"%s - Remove arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-firRvN] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-f: Remove sem pedir confirmação (POSIX: ignora arquivos inexistentes)\n"
		"\t-i: Pede confirmação do usuário para cada remoção (modo normal)\n"
		"\t-r, -R: Remove diretórios e seus conteúdos recursivamente\n"
		"\t-v: Verboso\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados <arquivo>s\n"
	);

	exit (2);

}	/* end help */
