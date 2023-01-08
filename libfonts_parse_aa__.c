/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_parse_aa__(enum libfonts_antialiasing *outp, const char *value)
{
#define X(C, S)\
	if (!strcasecmp(value, S)) {\
		*outp = C;\
	} else
	LIST_ANTIALIASINGS(X,) {
		return 0;
	}
#undef X
	return 1;

}


#else


int
main(void)
{
	enum libfonts_antialiasing res;
	char buf[1024];
	size_t i;

#define X(C, S)\
		do {\
			res = 999;\
			ASSERT(libfonts_parse_aa__(&res, S) == 1);\
			ASSERT(res == C);\
			strcpy(buf, S);\
			for (i = 0; buf[i]; i++)\
				buf[i] = (char)toupper(buf[i]);\
			res = 999;\
			ASSERT(libfonts_parse_aa__(&res, buf) == 1);\
			ASSERT(res == C);\
		} while (0)
	LIST_ANTIALIASINGS(X, ;);
#undef X

	res = 999;
	ASSERT(libfonts_parse_aa__(&res, " something else ") == 0);
	ASSERT(res == 999);

	return 0;
}


#endif
