/*
 ****************************************************************
 *								*
 *			id.c					*
 *								*
 *	Imprime informa��es de identifica��o do usu�rio	*
 *								*
 *	Vers�o	1.0.0, de 10.01.26				*
 *								*
 *	M�dulo: id						*
 *		Utilit�rios B�sicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright � 2026 NCE/UFRJ - tecle "man licen�a"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

/*
 ****************************************************************
 *	Vari�veis e Defini��es globais				*
 ****************************************************************
 */
const char	pgversion[] = "Vers�o:	1.0.0, de 10.01.26";

#define	elif	else if
#define NOSTR	(char *)NULL

entry int	Gflag;		/* Todos os grupos */
entry int	gflag;		/* Grupo efetivo/real */
entry int	nflag;		/* Imprime nome em vez de n�mero */
entry int	rflag;		/* Real em vez de efetivo */
entry int	uflag;		/* Usu�rio efetivo/real */

/*
 ******	Prot�tipos de fun��es ***********************************
 */
void		current (void);
void		user (PASSWD *);
void		sgroup (PASSWD *);
void		ugroup (PASSWD *);
PASSWD		*getuser (char *);
void		help (void);

/*
 ****************************************************************
 *	Imprime informa��es de identifica��o do usu�rio	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	GROUP		*gr;
	PASSWD		*pw = NOPASSWD;
	int		opt, id;

	/*
	 *	Analisa as op��es
	 */
	while ((opt = getopt (argc, argv, "GgnruH")) != EOF)
	{
		switch (opt)
		{
		    case 'G':
			Gflag++;
			break;

		    case 'g':
			gflag++;
			break;

		    case 'n':
			nflag++;
			break;

		    case 'r':
			rflag++;
			break;

		    case 'u':
			uflag++;
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

	/*
	 *	Obt�m informa��es do usu�rio especificado
	 */
	if (*argv != NOSTR)
		pw = getuser ((char *)*argv);

	/*
	 *	Verifica conflito de op��es
	 */
	if (Gflag + gflag + uflag > 1)
		help ();

	/*
	 *	Op��o -G: Imprime todos os grupos
	 */
	if (Gflag)
	{
		if (nflag)
			sgroup (pw);
		else
			ugroup (pw);
		return (0);
	}

	/*
	 *	Op��o -g: Imprime o grupo
	 */
	if (gflag)
	{
		if (pw != NOPASSWD)
			id = pw->pw_gid;
		else
			id = rflag ? getgid () : getegid ();

		if (nflag && (gr = getgrgid (id)) != NOGROUP)
		{
			printf ("%s\n", gr->gr_name);
			return (0);
		}

		printf ("%u\n", id);
		return (0);
	}

	/*
	 *	Op��o -u: Imprime o usu�rio
	 */
	if (uflag)
	{
		if (pw != NOPASSWD)
			id = pw->pw_uid;
		else
			id = rflag ? getuid () : geteuid ();

		if (nflag && (pw = getpwuid (id)) != NOPASSWD)
		{
			printf ("%s\n", pw->pw_name);
			return (0);
		}

		printf ("%u\n", id);
		return (0);
	}

	/*
	 *	Imprime informa��es completas
	 */
	if (pw != NOPASSWD)
		user (pw);
	else
		current ();

	return (0);

}	/* end main */

/*
 ****************************************************************
 *	Imprime informa��es do usu�rio corrente		*
 ****************************************************************
 */
void
current (void)
{
	GROUP		*gr;
	PASSWD		*pw;
	int		id, eid;
	int		first;
	char		**member;

	id = getuid ();
	printf ("uid=%u", id);

	if ((pw = getpwuid (id)) != NOPASSWD)
		printf ("(%s)", pw->pw_name);

	if ((eid = geteuid ()) != id)
	{
		printf (" euid=%u", eid);

		if ((pw = getpwuid (eid)) != NOPASSWD)
			printf ("(%s)", pw->pw_name);
	}

	id = getgid ();
	printf (" gid=%u", id);

	if ((gr = getgrgid (id)) != NOGROUP)
		printf ("(%s)", gr->gr_name);

	if ((eid = getegid ()) != id)
	{
		printf (" egid=%u", eid);

		if ((gr = getgrgid (eid)) != NOGROUP)
			printf ("(%s)", gr->gr_name);
	}

	/*
	 *	Imprime grupos suplementares
	 */
	if (pw != NOPASSWD)
	{
		first = 1;
		setgrent ();

		while ((gr = getgrent ()) != NOGROUP)
		{
			if (pw->pw_gid == gr->gr_gid)
				continue;

			if (gr->gr_mem == NULL)
				continue;

			for (member = gr->gr_mem; *member != NOSTR; member++)
			{
				if (strcmp (*member, pw->pw_name) == 0)
				{
					if (first)
					{
						printf (" groups=%u(%s)", gr->gr_gid, gr->gr_name);
						first = 0;
					}
					else
					{
						printf (",%u(%s)", gr->gr_gid, gr->gr_name);
					}
					break;
				}
			}
		}

		endgrent ();
	}

	printf ("\n");

}	/* end current */

/*
 ****************************************************************
 *	Imprime informa��es de um usu�rio espec�fico		*
 ****************************************************************
 */
void
user (PASSWD *pw)
{
	GROUP		*gr;
	int		first;
	char		**member;

	printf ("uid=%u(%s)", pw->pw_uid, pw->pw_name);
	printf (" gid=%u", pw->pw_gid);

	if ((gr = getgrgid (pw->pw_gid)) != NOGROUP)
		printf ("(%s)", gr->gr_name);

	/*
	 *	Imprime grupos suplementares
	 */
	first = 1;
	setgrent ();

	while ((gr = getgrent ()) != NOGROUP)
	{
		if (pw->pw_gid == gr->gr_gid)
			continue;

		if (gr->gr_mem == NULL)
			continue;

		for (member = gr->gr_mem; *member != NOSTR; member++)
		{
			if (strcmp (*member, pw->pw_name) == 0)
			{
				if (first)
				{
					printf (" groups=%u(%s)", gr->gr_gid, gr->gr_name);
					first = 0;
				}
				else
				{
					printf (",%u(%s)", gr->gr_gid, gr->gr_name);
				}
				break;
			}
		}
	}

	endgrent ();
	printf ("\n");

}	/* end user */

/*
 ****************************************************************
 *	Imprime nomes dos grupos				*
 ****************************************************************
 */
void
sgroup (PASSWD *pw)
{
	GROUP		*gr;
	char		*fmt;
	char		**member;

	if (pw != NOPASSWD)
	{
		fmt = "%s";
		setgrent ();

		while ((gr = getgrent ()) != NOGROUP)
		{
			if (gr->gr_mem == NULL)
				continue;

			for (member = gr->gr_mem; *member != NOSTR; member++)
			{
				if (strcmp (*member, pw->pw_name) == 0)
				{
					printf (fmt, gr->gr_name);
					fmt = " %s";
					break;
				}
			}
		}

		endgrent ();
	}

	printf ("\n");

}	/* end sgroup */

/*
 ****************************************************************
 *	Imprime IDs dos grupos					*
 ****************************************************************
 */
void
ugroup (PASSWD *pw)
{
	GROUP		*gr;
	char		*fmt;
	char		**member;

	if (pw != NOPASSWD)
	{
		fmt = "%u";
		setgrent ();

		while ((gr = getgrent ()) != NOGROUP)
		{
			if (gr->gr_mem == NULL)
				continue;

			for (member = gr->gr_mem; *member != NOSTR; member++)
			{
				if (strcmp (*member, pw->pw_name) == 0)
				{
					printf (fmt, gr->gr_gid);
					fmt = " %u";
					break;
				}
			}
		}

		endgrent ();
	}

	printf ("\n");

}	/* end ugroup */

/*
 ****************************************************************
 *	Obt�m informa��es do usu�rio				*
 ****************************************************************
 */
PASSWD *
getuser (char *name)
{
	PASSWD		*pw;
	int		uid;
	char		*ep;

	/*
	 *	Tenta obter por nome
	 */
	if ((pw = getpwnam (name)) != NOPASSWD)
		return (pw);

	/*
	 *	Tenta obter por UID
	 */
	uid = strtol (name, &ep, 10);

	if (*name != '\0' && *ep == '\0')
	{
		if ((pw = getpwuid (uid)) != NOPASSWD)
			return (pw);
	}

	fprintf (stderr, "id: %s: Usu�rio n�o encontrado\n", name);
	exit (1);

}	/* end getuser */

/*
 ****************************************************************
 *	Resumo de utiliza��o do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - imprime informa��es de identifica��o do usu�rio\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-Ggnru] [usu�rio]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOp��es:"
		"\t-G: Imprime todos os grupos do usu�rio\n"
		"\t-g: Imprime o grupo efetivo ou real\n"
		"\t-n: Imprime nome em vez de n�mero (com -G, -g ou -u)\n"
		"\t-r: Imprime o ID real em vez do efetivo (com -g ou -u)\n"
		"\t-u: Imprime o usu�rio efetivo ou real\n"
	);

	exit (2);

}	/* end help */
