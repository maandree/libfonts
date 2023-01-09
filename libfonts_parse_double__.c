/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_parse_double__(double *outp, const char *value)
{
	char *end;
	int saved_errno = errno;
	errno = 0;
	*outp = strtod(value, &end);
	if (errno || *end || isblank(*value)) {
		errno = saved_errno;
		return 0;
	}
	errno = saved_errno;
	return 1;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
