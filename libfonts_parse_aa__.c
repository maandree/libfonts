/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


#define LIST_ANTIALIASINGS(X)\
	X(LIBFONTS_ANTIALIASING_UNSPECIFIED, "unspecified")\
	X(LIBFONTS_ANTIALIASING_NONE, "none")\
	X(LIBFONTS_ANTIALIASING_NONE, "aliased")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "greyscale")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "grayscale")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "grey")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "gray")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "antialiased")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "greyscale!")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "grayscale!")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "grey!")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "gray!")\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "antialiased!")\
	X(LIBFONTS_ANTIALIASING_SUBPIXEL, "subpixel")\
	X(LIBFONTS_ANTIALIASING_SUBPIXEL, "glitter")\
	X(LIBFONTS_ANTIALIASING_SUBPIXEL_FORCED, "subpixel!")\
	X(LIBFONTS_ANTIALIASING_SUBPIXEL_FORCED, "glitter!")

int
libfonts_parse_aa__(enum libfonts_antialiasing *outp, const char *value)
{
#define X(C, S)\
	if (!strcasecmp(value, S)) {\
		*outp = C;\
	} else
	LIST_ANTIALIASINGS(X) {
		return 0;
	}
#undef X
	return 1;

}


#else


int
main(void)
{
	return 0; /* TODO add test */
}


#endif
