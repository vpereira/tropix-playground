/*
 ****************************************************************
 *						*
 *			find.c				*
 *						*
 *	find simples para o TROPIX			*
 *						*
 *	Versao	1.0.0, de 12.01.26		*
 *						*
 *	Modulo: find					*
 *		Utilitarios Basicos			*
 *		Categoria B				*
 *						*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *	Copyright © 2026 NCE/UFRJ - tecle "man licenca"	*
 *						*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifndef MODE_T
typedef long mode_t;
#define MODE_T
#endif

#define MAX_PATH	1024
#define MAX_EXEC_ARGS	64

#define SECSPERDAY	(24 * 60 * 60)

#define WIFEXITED(st)	(((st) & 0xFF) == 0)
#define WEXITSTATUS(st)	(((st) >> 8) & 0xFF)
#define WIFSIGNALED(st)	(((st) & 0xFF) != 0 && ((st) & 0xFF) != 0x3F)

#define CMP_EQ	0
#define CMP_LT	-1
#define CMP_GT	1

typedef struct {
	char	*name;
	char	type;
	int	mtime_cmp;
	long	mtime_days;
	int	size_cmp;
	long	size_bytes;
	int	maxdepth;
	int	mindepth;
	int	do_print;
	int	do_delete;
	int	do_exec;
	char	*exec_argv[MAX_EXEC_ARGS];
	int	exec_argc;
} OPTIONS;

static void	error(const char *fmt, ...);
static int	match_pattern(const char *pat, const char *str);
static int	parse_cmp_num(const char *s, int *cmp, long *val, int is_size);
static int	match_criteria(const OPTIONS *opt, const char *name,
			const struct stat *st, time_t now);
static int	run_exec(const OPTIONS *opt, const char *path);
static void	walk(const OPTIONS *opt, const char *path, int depth);

static void
error(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "find: ");
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

static int
match_pattern(const char *pat, const char *str)
{
	while (*pat) {
		if (*pat == '*') {
			pat++;
			if (!*pat)
				return 1;
			while (*str) {
				if (match_pattern(pat, str))
					return 1;
				str++;
			}
			return 0;
		}
		if (*pat == '?') {
			if (!*str)
				return 0;
			pat++;
			str++;
			continue;
		}
		if (*pat != *str)
			return 0;
		pat++;
		str++;
	}
	return *str == '\0';
}

static int
parse_cmp_num(const char *s, int *cmp, long *val, int is_size)
{
	long v;
	char *end;
	int c = CMP_EQ;

	if (*s == '+') {
		c = CMP_GT;
		s++;
	} else if (*s == '-') {
		c = CMP_LT;
		s++;
	}

	v = strtol((char *)s, &end, 10);
	if (end == s)
		return -1;

	if (is_size && *end) {
		switch (*end) {
		case 'c':
			break;
		case 'k':
			v *= 1024;
			break;
		case 'm':
			v *= 1024 * 1024;
			break;
		case 'b':
			v *= 512;
			break;
		default:
			return -1;
		}
		end++;
	}

	if (*end != '\0')
		return -1;

	*cmp = c;
	*val = v;
	return 0;
}

static int
match_criteria(const OPTIONS *opt, const char *name,
	const struct stat *st, time_t now)
{
	long diff;

	if (opt->name && !match_pattern(opt->name, name))
		return 0;

	if (opt->type) {
		mode_t t = st->st_mode & S_IFMT;
		switch (opt->type) {
		case 'f':
			if (t != S_IFREG)
				return 0;
			break;
		case 'd':
			if (t != S_IFDIR)
				return 0;
			break;
		case 'l':
			if (t != S_IFLNK)
				return 0;
			break;
		case 'b':
			if (t != S_IFBLK)
				return 0;
			break;
		case 'c':
			if (t != S_IFCHR)
				return 0;
			break;
		case 'p':
			if (t != S_IFIFO)
				return 0;
			break;
		default:
			return 0;
		}
	}

	if (opt->mtime_cmp != CMP_EQ || opt->mtime_days >= 0) {
		diff = (now - st->st_mtime + SECSPERDAY - 1) / SECSPERDAY;
		if (opt->mtime_cmp == CMP_LT && diff >= opt->mtime_days)
			return 0;
		if (opt->mtime_cmp == CMP_GT && diff <= opt->mtime_days)
			return 0;
		if (opt->mtime_cmp == CMP_EQ && diff != opt->mtime_days)
			return 0;
	}

	if (opt->size_cmp != CMP_EQ || opt->size_bytes >= 0) {
		if (opt->size_cmp == CMP_LT && st->st_size >= opt->size_bytes)
			return 0;
		if (opt->size_cmp == CMP_GT && st->st_size <= opt->size_bytes)
			return 0;
		if (opt->size_cmp == CMP_EQ && st->st_size != opt->size_bytes)
			return 0;
	}

	return 1;
}

static int
run_exec(const OPTIONS *opt, const char *path)
{
	char *argv[MAX_EXEC_ARGS + 2];
	int i, j, has_brace;
	int status;
	int pid;

	has_brace = 0;
	j = 0;
	for (i = 0; i < opt->exec_argc && j < MAX_EXEC_ARGS; i++) {
		if (strcmp(opt->exec_argv[i], "{}") == 0) {
			argv[j++] = (char *)path;
			has_brace = 1;
		} else {
			argv[j++] = opt->exec_argv[i];
		}
	}
	if (!has_brace && j < MAX_EXEC_ARGS)
		argv[j++] = (char *)path;
	argv[j] = NULL;

	switch (pid = fork()) {
	case -1:
		fprintf(stderr, "find: fork falhou\n");
		return -1;
	case 0:
		execvp(argv[0], (const char **)argv);
		fprintf(stderr, "find: exec falhou: %s\n", argv[0]);
		_exit(127);
	}

	while (wait(&status) != pid)
		;

	if (!WIFEXITED(status) || WIFSIGNALED(status))
		return -1;

	return WEXITSTATUS(status);
}

static void
walk(const OPTIONS *opt, const char *path, int depth)
{
	struct stat st;
	char namebuf[MAX_PATH];
	const char *name;
	int is_dir;
	int matched;
	DIR *dir;
	struct dirent *de;

	if (lstat(path, &st) < 0) {
		fprintf(stderr, "find: nao consegui ler %s\n", path);
		return;
	}

	name = strrchr(path, '/');
	name = name ? name + 1 : path;

	if (opt->maxdepth >= 0 && depth > opt->maxdepth)
		return;

	matched = match_criteria(opt, name, &st, time(NULL));
	if (depth < opt->mindepth)
		matched = 0;

	is_dir = ((st.st_mode & S_IFMT) == S_IFDIR);

	if (matched) {
		if (opt->do_print)
			printf("%s\n", path);
		if (opt->do_exec)
			run_exec(opt, path);
		if (opt->do_delete && !is_dir) {
			if (unlink(path) < 0)
				fprintf(stderr, "find: nao consegui remover %s\n", path);
		}
	}

	if (is_dir) {
		if (opt->maxdepth < 0 || depth < opt->maxdepth) {
			dir = opendir(path);
			if (!dir) {
				fprintf(stderr, "find: nao consegui abrir %s\n", path);
				return;
			}
			while ((de = readdir(dir)) != NULL) {
				if (strcmp(de->d_name, ".") == 0 ||
				    strcmp(de->d_name, "..") == 0)
					continue;
				snprintf(namebuf, sizeof(namebuf), "%s/%s", path, de->d_name);
				walk(opt, namebuf, depth + 1);
			}
			closedir(dir);
		}

		if (opt->do_delete && matched) {
			if (rmdir(path) < 0)
				fprintf(stderr, "find: nao consegui remover %s\n", path);
		}
	}
}

int
main(int argc, char **argv)
{
	OPTIONS opt;
	char *paths[32];
	int npaths = 0;
	int i;

	memset(&opt, 0, sizeof(opt));
	opt.maxdepth = -1;
	opt.mindepth = 0;
	opt.mtime_days = -1;
	opt.size_bytes = -1;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-')
			break;
		if (npaths < (int)(sizeof(paths) / sizeof(paths[0])))
			paths[npaths++] = argv[i];
	}

	if (npaths == 0)
		paths[npaths++] = ".";

	for (; i < argc; i++) {
		if (strcmp(argv[i], "-name") == 0) {
			if (++i >= argc)
				error("falta argumento para -name");
			opt.name = argv[i];
			continue;
		}
		if (strcmp(argv[i], "-type") == 0) {
			if (++i >= argc)
				error("falta argumento para -type");
			opt.type = argv[i][0];
			continue;
		}
		if (strcmp(argv[i], "-mtime") == 0) {
			if (++i >= argc)
				error("falta argumento para -mtime");
			if (parse_cmp_num(argv[i], &opt.mtime_cmp,
					&opt.mtime_days, 0) < 0)
				error("valor invalido para -mtime");
			continue;
		}
		if (strcmp(argv[i], "-size") == 0) {
			if (++i >= argc)
				error("falta argumento para -size");
			if (parse_cmp_num(argv[i], &opt.size_cmp,
					&opt.size_bytes, 1) < 0)
				error("valor invalido para -size");
			continue;
		}
		if (strcmp(argv[i], "-maxdepth") == 0) {
			if (++i >= argc)
				error("falta argumento para -maxdepth");
			opt.maxdepth = atoi(argv[i]);
			continue;
		}
		if (strcmp(argv[i], "-mindepth") == 0) {
			if (++i >= argc)
				error("falta argumento para -mindepth");
			opt.mindepth = atoi(argv[i]);
			continue;
		}
		if (strcmp(argv[i], "-print") == 0) {
			opt.do_print = 1;
			continue;
		}
		if (strcmp(argv[i], "-delete") == 0) {
			opt.do_delete = 1;
			continue;
		}
		if (strcmp(argv[i], "-exec") == 0) {
			int j = 0;
			opt.do_exec = 1;
			opt.exec_argc = 0;
			for (i = i + 1; i < argc; i++) {
				if (strcmp(argv[i], ";") == 0)
					break;
				if (j < MAX_EXEC_ARGS)
					opt.exec_argv[j++] = argv[i];
			}
			if (i >= argc || strcmp(argv[i], ";") != 0)
				error("falta ';' em -exec");
			opt.exec_argc = j;
			continue;
		}
		error("opcao invalida: %s", argv[i]);
	}

	if (!opt.do_print && !opt.do_exec && !opt.do_delete)
		opt.do_print = 1;

	for (i = 0; i < npaths; i++)
		walk(&opt, paths[i], 0);

	return 0;
}
