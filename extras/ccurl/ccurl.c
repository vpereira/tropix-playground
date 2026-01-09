/*
 ****************************************************************
 *								*
 *			ccurl.c					*
 *								*
 *	Simple HTTP client (curl-like) without HTTPS support	*
 *								*
 *	Version	1.0.0, 2026-01-09				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Definitions and global variables			*
 ****************************************************************
 */
#define HTTP_PORT	80
#define BUFFER_SIZE	4096

#define	EVER	;;
#define	elif	else if

int	vflag = 0;	/* Verbose */
int	oflag = 0;	/* Output to file */
char	*output_file = NULL;
long	content_length = -1;	/* Content-Length from headers */

/*
 ****************************************************************
 *	Parse URL to extract host, port, and path		*
 ****************************************************************
 */
int
parse_url(const char *url, char *host, int *port, char *path)
{
	const char *p = url;
	char *h = host;

	/* Skip http:// if present */
	if (strncmp(p, "http://", 7) == 0)
		p += 7;

	/* Extract hostname */
	while (*p && *p != ':' && *p != '/')
		*h++ = *p++;
	*h = '\0';

	if (host[0] == '\0')
		return -1;

	/* Extract port if present */
	*port = HTTP_PORT;
	if (*p == ':')
	{
		p++;
		*port = atoi(p);
		while (*p && *p != '/')
			p++;
	}

	/* Extract path */
	if (*p == '/')
		strcpy(path, p);
	else
		strcpy(path, "/");

	return 0;
}

/*
 ****************************************************************
 *	Main function						*
 ****************************************************************
 */
int
main(int argc, char *argv[])
{
	int		opt;
	const char	*url = NULL;
	char		host[256];
	int		port;
	char		path[1024];
	int		tcp_fd;
	T_BIND		bind;
	INADDR		addr;
	T_CALL		call;
	const char	*tcp_dev = "/dev/itntcp";
	long		serv_addr;
	char		buf[BUFFER_SIZE];
	int		n;
	FILE		*tcp_in, *tcp_out;
	FILE		*out = stdout;
	int		header_done = 0;

	/* Parse options */
	while ((opt = getopt(argc, argv, "vo:")) != EOF)
	{
		switch (opt)
		{
		    case 'v':
			vflag = 1;
			break;

		    case 'o':
			oflag = 1;
			output_file = optarg;
			break;

		    default:
			fprintf(stderr, "Usage: %s [-v] [-o file] <url>\n", argv[0]);
			return 1;
		}
	}

	if (optind >= argc)
	{
		fprintf(stderr, "Usage: %s [-v] [-o file] <url>\n", argv[0]);
		return 1;
	}

	url = argv[optind];

	/* Parse URL */
	if (parse_url(url, host, &port, path) < 0)
	{
		fprintf(stderr, "Error: Invalid URL\n");
		return 1;
	}

	if (vflag)
		fprintf(stderr, "Host: %s, Port: %d, Path: %s\n", host, port, path);

	/* Open TCP endpoint */
	if ((tcp_fd = t_open(tcp_dev, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		fprintf(stderr, "Error: Cannot open %s\n", tcp_dev);
		return 1;
	}

	/* Get server address */
	if ((serv_addr = t_node_to_addr(tcp_fd, host, (long *)NULL)) == -1)
	{
		fprintf(stderr, "Error: Cannot resolve host %s\n", host);
		t_close(tcp_fd);
		return 1;
	}

	if (vflag)
		fprintf(stderr, "Server address: %08lX\n", serv_addr);

	/* Bind to local address */
	bind.addr.len = sizeof(INADDR);
	bind.addr.maxlen = sizeof(INADDR);
	bind.addr.buf = &addr;
	bind.qlen = 0;

	addr.a_port = 0;

	if (t_bind(tcp_fd, &bind, &bind) < 0)
	{
		fprintf(stderr, "Error: Cannot bind\n");
		t_close(tcp_fd);
		return 1;
	}

	/* Connect to server */
	addr.a_port = port;
	addr.a_addr = serv_addr;

	call.addr.len = sizeof(INADDR);
	call.addr.maxlen = sizeof(INADDR);
	call.addr.buf = &addr;

	if (t_connect(tcp_fd, &call, &call) < 0)
	{
		fprintf(stderr, "Error: Cannot connect to %s:%d\n", host, port);
		t_close(tcp_fd);
		return 1;
	}

	if (vflag)
		fprintf(stderr, "Connected to %s:%d\n", host, port);

	/* Open FILE streams */
	if ((tcp_in = fdopen(tcp_fd, "r")) == NULL)
	{
		fprintf(stderr, "Error: Cannot create input stream\n");
		t_close(tcp_fd);
		return 1;
	}

	if ((tcp_out = fdopen(tcp_fd, "w")) == NULL)
	{
		fprintf(stderr, "Error: Cannot create output stream\n");
		fclose(tcp_in);
		return 1;
	}

	/* Send HTTP request */
	fprintf(tcp_out, "GET %s HTTP/1.0\r\n", path);
	fprintf(tcp_out, "Host: %s\r\n", host);
	fprintf(tcp_out, "Connection: close\r\n");
	fprintf(tcp_out, "\r\n");
	fflush(tcp_out);
	t_push(tcp_fd);

	if (vflag)
		fprintf(stderr, "Request sent\n");

	/* Open output file if specified */
	if (oflag)
	{
		if ((out = fopen(output_file, "w")) == NULL)
		{
			fprintf(stderr, "Error: Cannot open output file %s\n", output_file);
			fclose(tcp_in);
			fclose(tcp_out);
			return 1;
		}
	}

	/* Skip HTTP headers line by line */
	while (fgets(buf, sizeof(buf), tcp_in) != NULL)
	{
		if (vflag)
			fprintf(stderr, "Header: %s", buf);

		/* Parse Content-Length */
		if (strncmp(buf, "Content-Length: ", 16) == 0)
			content_length = atol(buf + 16);

		/* Empty line marks end of headers */
		if (buf[0] == '\r' || buf[0] == '\n' ||
		    (buf[0] == '\r' && buf[1] == '\n'))
		{
			header_done = 1;
			break;
		}
	}

	if (!header_done)
	{
		fprintf(stderr, "Error: No response from server\n");
		if (oflag)
			fclose(out);
		fclose(tcp_in);
		fclose(tcp_out);
		return 1;
	}

	/* Read and write body */
	{
		long total = 0;
		long last_shown = 0;
		int pct;

		while ((n = fread(buf, 1, sizeof(buf), tcp_in)) > 0)
		{
			if (oflag)
			{
				if (fwrite(buf, 1, n, out) != n)
				{
					fprintf(stderr, "\nError writing to file\n");
					break;
				}
			}
			else
			{
				fwrite(buf, 1, n, stdout);
			}

			total += n;

			/* Show progress every 100KB */
			if (total - last_shown >= 102400)
			{
				if (content_length > 0)
				{
					pct = (int)((total * 100) / content_length);
					fprintf(stderr, "\rDownloaded: %ld / %ld bytes (%d%%) ",
						total, content_length, pct);
				}
				else
				{
					fprintf(stderr, "\rDownloaded: %ld bytes ", total);
				}
				fflush(stderr);
				last_shown = total;
			}
		}

		/* Final progress update */
		if (content_length > 0)
		{
			pct = (int)((total * 100) / content_length);
			fprintf(stderr, "\rDownloaded: %ld / %ld bytes (%d%%) ",
				total, content_length, pct);
		}
		else
		{
			fprintf(stderr, "\rDownloaded: %ld bytes ", total);
		}
		fprintf(stderr, "\n");
		fflush(stderr);
	}

	if (vflag)
		fprintf(stderr, "Download complete\n");

	/* Cleanup */
	if (oflag)
		fclose(out);

	t_sndrel(tcp_fd);
	t_rcvrel(tcp_fd);
	fclose(tcp_in);
	fclose(tcp_out);

	return 0;
}
