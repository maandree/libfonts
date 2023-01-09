/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


char *
libfonts_confsplit__(char *line)
{
	size_t p, q;

	p = 0;
	while (line[p] && line[p] != '=')
		p += 1;
	if (!line[p])
		return NULL;

	q = p;
	while (q && isblank(line[q - 1]))
	       q -= 1;
	if (!q)
		return NULL;
	line[q] = '\0';

	p++;
	while (isblank(line[p]))
		p++;
	if (!line[p])
		return NULL;

	return &line[p];
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
