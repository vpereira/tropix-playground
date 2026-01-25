/*
 ****************************************************************
 *								*
 *			top.c					*
 *								*
 *	Lista de processos em tela cheia			*
 *								*
 *	Versao	1.0.0, de 25.01.26				*
 *								*
 *	Modulo: top						*
 *		Utilitario especial				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *	Copyright (c) 2026 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/region.h>
#include <sys/syscall.h>
#include <sys/uerror.h>
#include <sys/signal.h>
#include <sys/uproc.h>
#include <sys/mmu.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <pwd.h>

/*
 ****************************************************************
 *	Variaveis e Definicoes globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versao:	1.0.0, de 25.01.26";

entry int	exit_code = 0;	/* Codigo de retorno */

entry int	aflag;		/* Todos os processos */
entry int	xflag;		/* Lista processos sem TTY */

/*
 ******	Tabela de terminais *************************************
 */
const char	notty[] = "   "; /* Simbolo de "sem tty" */

typedef struct
{
	char	t_ttyid[16];	/* Nome da linha */
	dev_t	t_dev;		/* Dispositivo */

}	TTYTB;

extern const TTYTB	ttytb[];

typedef struct
{
	long	pid;
	char	user[16];
	char	tty[16];
	char	state;
	int	pri;
	int	nice;
	time_t	utime;
	time_t	stime;
	long	total;
	char	cmd[16];

}	TOPPROC;

/*
 ******	Protótipos **********************************************
 */
static int	map_uproc (const UPROC *kup, UPROC *up);
static const char	*get_tty_nm (const UPROC *up);
static void	format_time (char *buf, int bufsz, time_t ticks, int hz);
static int	cmp_proc (const void *a, const void *b);
static void	print_help (void);


/*
 ****************************************************************
 *	Lista de processos					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	int		intervalo = 2;
	int		ajuda = 0;
	int		i, c, cnt;
	long		ps_pid;
	int		ps_uid;
	const char	*ps_user_nm;
	const UPROCV	*uvp, *procv, *last_procv, *end_procv;
	int		procv_delta;
	SCB		scb;
	TOPPROC	*vec = NOVOID;
	int		max_proc = 0;

	/*
	 *	Verifica usuario efetivo
	 */
	if (geteuid () != 0)
	{
		fprintf (stderr, "Nao sou o SUPERUSUARIO!\n");
		return (1);
	}

	/*
	 *	Analisa opcoes
	 */
	while ((opt = getopt (argc, argv, "axd:H")) != EOF)
	{
		switch (opt)
		{
		    case 'a':
			aflag++;
			break;

		    case 'x':
			xflag++;
			aflag++;
			break;

		    case 'd':
			intervalo = atoi (optarg);
			if (intervalo < 1)
				intervalo = 1;
			break;

		    case 'H':
			print_help ();

		    default:
			print_help ();
		}
	}

	/*
	 *	Prepara parametros
	 */
	ps_user_nm = getenv ("USER");
	ps_uid	   = getuid ();
	ps_pid     = getpid ();

	/*
	 *	Obtem o "System Control Block"
	 */
	if ((int)getscb (&scb) == -1)
	{
		fprintf (stderr, "Nao consegui obter o SCB\n");
		return (1);
	}

	/*
	 *	Mapeia a tabela de processos
	 */
	if ((int)(procv = uvp = phys (scb.y_uproc, scb.y_nproc * sizeof (UPROCV), O_KERNEL|O_RDONLY)) == -1)
	{
		fprintf (stderr, "Nao consegui mapear a tabela de processos\n");
		return (1);
	}

	procv_delta = (int)uvp - (int)scb.y_uproc;
	last_procv  = (UPROCV *)((int)scb.y_lastuproc + procv_delta);
	end_procv   = (UPROCV *)((int)scb.y_enduproc  + procv_delta);

	/*
	 *	Reserva vetor
	 */
	max_proc = scb.y_nproc;
	vec = (TOPPROC *)malloc (max_proc * sizeof (TOPPROC));
	if (vec == NOVOID)
	{
		fprintf (stderr, "Memoria esgotada\n");
		phys (procv, 0, 0);
		return (1);
	}

	/*
	 *	Inicia curses
	 */
	if (incurses ("top") == ERR)
	{
		fprintf (stderr, "Nao consegui entrar em Curses\n");
		phys (procv, 0, 0);
		free (vec);
		return (1);
	}

	wgeton (stdwin, G_NODELAY);
	wgetoff (stdwin, G_ECHO);

	for (;;)
	{
		UPROC		up;
		const char	*tty_nm;
		const char	*user_nm;
		TOPPROC		*tp;
		int		nlin = 0;

		/*
		 *	Monta a lista
		 */
		cnt = 0;
		for (uvp = procv; uvp < end_procv; uvp++)
		{
			if (uvp->u_uproc == NOUPROC)
				continue;

			if (map_uproc (uvp->u_uproc, &up) != 0)
				continue;

			if (up.u_state == SNULL)
				continue;

			if (up.u_tgrp == 0 && !xflag)
				continue;

			if (up.u_euid != ps_uid && up.u_ruid != ps_uid)
			{
				if (!aflag || ps_uid != 0)
					continue;
			}

			if (!aflag && ps_uid == 0)
			{
				user_nm = pwcache (up.u_euid);
				if (user_nm == NOSTR || ps_user_nm == NOSTR)
					continue;
				if (strcmp (user_nm, ps_user_nm) != 0)
					continue;
			}

			if (cnt >= max_proc)
				break;

			tp = &vec[cnt++];

			tp->pid = up.u_pid;
			user_nm = pwcache (up.u_euid);
			if (user_nm == NOSTR)
				user_nm = "???";
			strncpy (tp->user, user_nm, sizeof (tp->user) - 1);
			tp->user[sizeof (tp->user) - 1] = '\0';

			tty_nm = get_tty_nm (&up);
			strncpy (tp->tty, tty_nm, sizeof (tp->tty) - 1);
			tp->tty[sizeof (tp->tty) - 1] = '\0';

			tp->state = "0rRsS0Z"[up.u_state];
			tp->pri = up.u_pri;
			tp->nice = up.u_nice;
			tp->utime = up.u_utime;
			tp->stime = up.u_stime;
			tp->total = up.u_utime + up.u_stime;

			strncpy (tp->cmd, up.u_pgname, sizeof (tp->cmd) - 1);
			tp->cmd[sizeof (tp->cmd) - 1] = '\0';
		}

		qsort (vec, cnt, sizeof (TOPPROC), cmp_proc);

		werase (stdwin);

		wprintw (stdwin, "TROPIX top  (proc: %d, intervalo: %d s)\n", cnt, intervalo);
		wprintw (stdwin, " PID  USER         TTY  S  PRI  NI  UTIME  STIME  CMD\n");

		for (i = 0; i < cnt; i++)
		{
			char	utbuf[8], stbuf[8];

			if (nlin >= LINES - 3)
				break;

			format_time (utbuf, sizeof (utbuf), vec[i].utime, scb.y_hz);
			format_time (stbuf, sizeof (stbuf), vec[i].stime, scb.y_hz);

			wprintw
			(	stdwin,
				"%5ld %-12.12s %-3.3s  %c %4d %3d %6s %6s  %-14.14s\n",
				vec[i].pid,
				vec[i].user,
				vec[i].tty,
				vec[i].state,
				vec[i].pri,
				vec[i].nice,
				utbuf,
				stbuf,
				vec[i].cmd
			);
			nlin++;
		}

		if (ajuda)
		{
			wprintw (stdwin, "Teclas: q sair, r atualizar, ? ajuda\n");
			ajuda = 0;
		}
		else
		{
			wprintw (stdwin, "q sair, r atualizar, ? ajuda\n");
		}

		wrefresh (stdwin);

		/*
		 *	Espera e trata teclas
		 */
		for (i = 0; i < intervalo; i++)
		{
			c = wgetch (stdwin);
			if (c == 'q' || c == 'Q')
				goto fim;
			if (c == 'r' || c == 'R')
				break;
			if (c == '?')
				ajuda = 1;
			sleep (1);
		}
	}

fim:
	outcurses ();
	phys (procv, 0, 0);
	free (vec);
	return (exit_code);
}

/*
 ****************************************************************
 *	Mapeia e copia uma UPROC				*
 ****************************************************************
 */
static int
map_uproc (const UPROC *kup, UPROC *up)
{
	const UPROC	*mp;

	if ((int)(mp = phys (kup, UPROCSZ, O_KERNEL|O_RDONLY)) == -1)
		return (-1);

	memmove (up, mp, sizeof (UPROC));
	phys (mp, 0, 0);
	return (0);
}

/*
 ****************************************************************
 *	Obtém o nome do TTY					*
 ****************************************************************
 */
static const char *
get_tty_nm (const UPROC *up)
{
	const TTYTB	*dp;

	if (up == NOUPROC)
		return (notty);

	if (up->u_tty == NOTTY)
		return (notty);

	for (dp = ttytb; dp->t_ttyid[0] != 0; dp++)
	{
		if (dp->t_dev == up->u_ttydev)
			return (dp->t_ttyid);
	}

	return (notty);
}

/*
 ****************************************************************
 *	Formata tempo (ticks -> mm:ss)				*
 ****************************************************************
 */
static void
format_time (char *buf, int bufsz, time_t ticks, int hz)
{
	time_t		tm;

	tm = (ticks + (hz >> 1)) / hz;
	snprintf (buf, bufsz, "%2d:%02d", (int)(tm / 60), (int)(tm % 60));
}

/*
 ****************************************************************
 *	Ordena por tempo total					*
 ****************************************************************
 */
static int
cmp_proc (const void *a, const void *b)
{
	const TOPPROC	*pa = (const TOPPROC *)a;
	const TOPPROC	*pb = (const TOPPROC *)b;

	if (pa->total < pb->total)
		return (1);
	if (pa->total > pb->total)
		return (-1);
	if (pa->pid < pb->pid)
		return (-1);
	if (pa->pid > pb->pid)
		return (1);
	return (0);
}

/*
 ****************************************************************
 *	Resumo de uso						*
 ****************************************************************
 */
static void
print_help (void)
{
	fprintf
	(	stderr,
		"%s - lista processos em tela cheia\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-ax] [-d <seg>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpcoes:\n"
		"\t-a: todos os processos\n"
		"\t-x: inclui processos sem TTY\n"
		"\t-d: intervalo de atualizacao (segundos)\n"
		"\t-H: mostra esta ajuda\n"
	);
	exit (1);
}
