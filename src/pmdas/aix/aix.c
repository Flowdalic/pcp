/*
 * AIX PMDA
 *
 * Collect performance data from the AIX kernel using libperfstat for
 * the most part.
 *
 * Copyright (c) 2004 Silicon Graphics, Inc.  All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 * 
 * Contact information: Silicon Graphics, Inc., 1500 Crittenden Lane,
 * Mountain View, CA 94043, USA, or: http://www.sgi.com
 */

#include <time.h>
#include "common.h"

static int	_isDSO = 1;
static char	mypath[MAXPATHLEN];

/*
 * wrapper for pmdaFetch which primes the methods ready for
 * the next fetch
 * ... real callback is fetch_callback()
 */
static int
aix_fetch(int numpmid, pmID pmidlist[], pmResult **resp, pmdaExt *pmda)
{
    int		i;

    // TODO: this should only fetch metrics from "pmidlist"
    for (i = 0; i < methodtab_sz; i++) {
	methodtab[i].m_prefetch();
    }

    return pmdaFetch(numpmid, pmidlist, resp, pmda);
}

/*
 * callback provided to pmdaFetch
 */
static int
aix_fetch_callback(pmdaMetric *mdesc, unsigned int inst, pmAtomValue *atom)
{
    metricdesc_t	*mdp;

    mdp = (metricdesc_t *)mdesc->m_user;
    return methodtab[mdp->md_method].m_fetch(mdesc, inst, atom);
}

/*
 * Initialise the agent (both daemon and DSO).
 */
void 
aix_init(pmdaInterface *dp)
{
    if (_isDSO) {
	snprintf(mypath, sizeof(mypath),
		"%s/aix/help", pmGetConfig("PCP_PMDAS_DIR"));
	pmdaDSO(dp, PMDA_INTERFACE_3, "AIX DSO", mypath);
    }

    if (dp->status != 0)
	return;

    dp->version.two.fetch = aix_fetch;
    pmdaSetFetchCallBack(dp, aix_fetch_callback);
    init_data(dp->domain);
    pmdaInit(dp, indomtab, indomtab_sz, metrictab, metrictab_sz);
}

static void
usage(void)
{
    fprintf(stderr, "Usage: %s [options]\n\n", pmProgname);
    fputs("Options:\n"
	  "  -d domain    use domain (numeric) for metrics domain of PMDA\n"
	  "  -l logfile   write log into logfile rather than using default log name\n",
	      stderr);		
    exit(1);
}

/*
 * Set up the agent if running as a daemon.
 */
int
main(int argc, char **argv)
{
    int			err = 0;
    pmdaInterface	desc;

    _isDSO = 0;
    __pmSetProgname(argv[0]);

    snprintf(mypath, sizeof(mypath),
		"%s/aix/help", pmGetConfig("PCP_PMDAS_DIR"));
    pmdaDaemon(&desc, PMDA_INTERFACE_3, pmProgname, AIX, "aix.log", mypath);

    if (pmdaGetOpt(argc, argv, "D:d:l:?", &desc, &err) != EOF)
	err++;
    if (err)
	usage();

    pmdaOpenLog(&desc);
    aix_init(&desc);
    pmdaConnect(&desc);
    pmdaMain(&desc);

    exit(0);
}
