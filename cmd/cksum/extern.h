/*
 ****************************************************************
 *	extern.h - Protótipos para cksum			*
 ****************************************************************
 */

int	crc (int, unsigned long *, unsigned long *);
void	pcrc (char *, unsigned long, unsigned long);
void	psum1 (char *, unsigned long, unsigned long);
void	psum2 (char *, unsigned long, unsigned long);
int	csum1 (int, unsigned long *, unsigned long *);
int	csum2 (int, unsigned long *, unsigned long *);
