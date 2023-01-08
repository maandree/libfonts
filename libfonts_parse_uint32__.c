/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_parse_uint32__(uint32_t *outp, const char *value)
{
	uint32_t out = 0, digit;
	value += *value == '+';
	if (!*value)
		return 0;
	while (isdigit(*value)) {
		digit = (*value & 15);
		if (out > (UINT32_C(0xFFFFFFFF) - digit) / 10)
			return 0;
		out = out * 10 + digit;
	}
	if (*value)
		return 0;
	*outp = out;
	return 1;
}


#else


int
main(void)
{
	return 0; /* TODO add test */
}


#endif
