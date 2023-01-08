/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_get_default_font_name(enum libfonts_default_font font, size_t index, const char **namep)
{
	const char *alias = NULL;

	if (font == LIBFONTS_DEFAULT_SANS_SERIF) {
		if (index == 0)
			alias = "sans-serif";
		else if (index == 1)
			alias = "sans serif";
		else if (index == 2)
			alias = "sans";

	} else if (font == LIBFONTS_DEFAULT_SERIF) {
		if (index == 0)
			alias = "serif";

	} else if (font == LIBFONTS_DEFAULT_MONOSPACE) {
		if (index == 0)
			alias = "monospace";
		else if (index == 1)
			alias = "monospaced";

	} else {
		errno = EINVAL;
		return -1;
	}

	if (namep)
		*namep = alias;
	return alias != NULL;
}


#else


int
main(void)
{
	const char *r;

#define T(FONT, INDEX, NAME)\
	do {\
		errno = 0;\
		ASSERT(libfonts_get_default_font_name(FONT, INDEX, &r) == 1);\
		ASSERT(r != NULL);\
		ASSERT(!strcmp(r, NAME));\
		ASSERT(!errno);\
		ASSERT(libfonts_get_default_font_name(FONT, INDEX, NULL) == 1);\
		ASSERT(!errno);\
	} while (0)

#define X(FONT, INDEX)\
	do {\
		errno = 0;\
		ASSERT(libfonts_get_default_font_name(FONT, INDEX, &r) == 0);\
		ASSERT(r == NULL);\
		ASSERT(!errno);\
		ASSERT(libfonts_get_default_font_name(FONT, INDEX, NULL) == 0);\
		ASSERT(!errno);\
	} while (0)

	errno = 0;
	ASSERT(libfonts_get_default_font_name((enum libfonts_default_font)~1, 0, &r) == -1);
	ASSERT(errno == EINVAL);

	errno = 0;
	ASSERT(libfonts_get_default_font_name(99, 0, &r) == -1);
	ASSERT(errno == EINVAL);

	T(LIBFONTS_DEFAULT_SANS_SERIF, 0, "sans-serif");
	T(LIBFONTS_DEFAULT_SANS_SERIF, 1, "sans serif");
	T(LIBFONTS_DEFAULT_SANS_SERIF, 2, "sans");
	X(LIBFONTS_DEFAULT_SANS_SERIF, 3);

	T(LIBFONTS_DEFAULT_SERIF, 0, "serif");
	X(LIBFONTS_DEFAULT_SERIF, 1);

	T(LIBFONTS_DEFAULT_MONOSPACE, 0, "monospace");
	T(LIBFONTS_DEFAULT_MONOSPACE, 1, "monospaced");
	X(LIBFONTS_DEFAULT_MONOSPACE, 2);

	return 0;
}


#endif
