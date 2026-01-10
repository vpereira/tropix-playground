/*
 ****************************************************************
 *								*
 *			rng.c					*
 *								*
 *	Driver simples para /dev/urandom			*
 *								*
 *	Versao	1.0.0, de 12.01.26				*
 *								*
 *	Modulo: Nucleo						*
 *		Nucleo do TROPIX para PC				*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 *	Minor 0 => /dev/urandom
 */

static LOCK		rng_lock;
static unsigned long	rng_state = 0x6d2b79f5UL;

static unsigned long
rng_next (void)
{
	unsigned long	x;

	x = rng_state;
	if (x == 0)
		x = 0x1;

	x ^= (x << 13);
	x ^= (x >> 17);
	x ^= (x << 5);

	rng_state = x;
	return (x);
}

void
rng_seed (unsigned long v)
{
	SPINLOCK (&rng_lock);
	rng_state ^= v + 0x9e3779b9UL + (rng_state << 6) + (rng_state >> 2);
	SPINFREE (&rng_lock);
}

static void
rng_fill (char *buf, int count)
{
	unsigned long	x;
	int		i;

	while (count > 0)
	{
		SPINLOCK (&rng_lock);
		rng_state ^= (unsigned long)time;
		x = rng_next ();
		SPINFREE (&rng_lock);

		for (i = 0; i < 4 && count > 0; i++, count--)
		{
			*buf++ = (char)(x & 0xFF);
			x >>= 8;
		}
	}
}

int
rngopen (dev_t dev, int flag)
{
	return (0);
}

int
rngclose (dev_t dev, int flag)
{
	return (0);
}

void
rngread (IOREQ *iop)
{
	char		tmp[64];
	char		*area;
	int		count;
	int		n;

	if (MINOR (iop->io_dev) != 0)
	{
		u.u_error = EINVAL;
		return;
	}

	area = (char *)iop->io_area;
	count = iop->io_count;

	while (count > 0)
	{
		n = (count > (int)sizeof (tmp)) ? (int)sizeof (tmp) : count;
		rng_fill (tmp, n);

		if (unimove (area, tmp, n, SU) < 0)
		{
			u.u_error = EINVAL;
			return;
		}

		area  += n;
		count -= n;
	}

	iop->io_count = 0;
}

void
rngwrite (IOREQ *iop)
{
	iop->io_count = 0;
}
