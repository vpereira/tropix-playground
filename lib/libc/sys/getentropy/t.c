#include <sys/syscall.h>

#include <stdio.h>

main ()
{
	unsigned char	buf[16];
	int			i;

	if (getentropy (buf, sizeof (buf)) < 0)
	{
		printf ("erro em getentropy\n");
		return (1);
	}

	for (i = 0; i < (int)sizeof (buf); i++)
		printf ("%02x", buf[i] & 0xFF);

	printf ("\n");
	return (0);
}
