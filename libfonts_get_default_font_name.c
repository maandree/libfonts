/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


ssize_t
libfonts_get_default_font_name(enum libfonts_default_font font, char *name, size_t size, size_t index)
{
	const char *alias;
	size_t len;

	if (font == LIBFONTS_DEFAULT_SANS_SERIF) {
		if (index == 0)
			alias = "sans-serif";
		else if (index == 1)
			alias = "sans serif";
		else if (index == 2)
			alias = "sans";
		else
			return 0;

	} else if (font == LIBFONTS_DEFAULT_SERIF) {
		if (index == 0)
			alias = "serif";
		else
			return 0;

	} else if (font == LIBFONTS_DEFAULT_MONOSPACE) {
		if (index == 0)
			alias = "monospace";
		else if (index == 1)
			alias = "monospaced";
		else
			return 0;

	} else {
		errno = EINVAL;
		return -1;
	}

	len = strlen(alias);
	if (size) {
		size -= 1;
		if (size > len)
			size = len;
		memcpy(name, alias, size);
		name[size] = '\0';
	}
	return (ssize_t)(len + 1);
}


#else


int
main(void)
{
#define T(FONT, INDEX, SIZE, NAME, NAMELEN)\
	do {\
		errno = 0;\
		ASSERT(libfonts_get_default_font_name(FONT, buf, SIZE, INDEX) == NAMELEN);\
		ASSERT(!buf[SIZE - 1]);\
		ASSERT(!strcmp(buf, NAME));\
		ASSERT(!errno);\
	} while (0)

#define X(FONT, INDEX, SIZE)\
	do {\
		errno = 0;\
		ASSERT(libfonts_get_default_font_name(FONT, buf, SIZE, INDEX) == 0);\
		ASSERT(!buf[SIZE - 1]);\
		ASSERT(!errno);\
	} while (0)

	char buf[50];

	errno = 0;
	ASSERT(libfonts_get_default_font_name(-1, buf, sizeof(buf), 0) == -1);
	ASSERT(errno == EINVAL);

	errno = 0;
	ASSERT(libfonts_get_default_font_name(99, buf, sizeof(buf), 0) == -1);
	ASSERT(errno == EINVAL);

	T(LIBFONTS_DEFAULT_SANS_SERIF, 0, sizeof(buf), "sans-serif", 11);
	T(LIBFONTS_DEFAULT_SANS_SERIF, 1, sizeof(buf), "sans serif", 11);
	T(LIBFONTS_DEFAULT_SANS_SERIF, 2, sizeof(buf), "sans", 5);
	X(LIBFONTS_DEFAULT_SANS_SERIF, 3, sizeof(buf));

	T(LIBFONTS_DEFAULT_SERIF, 0, sizeof(buf), "serif", 6);
	X(LIBFONTS_DEFAULT_SERIF, 1, sizeof(buf));

	T(LIBFONTS_DEFAULT_MONOSPACE, 0, sizeof(buf), "monospace", 10);
	T(LIBFONTS_DEFAULT_MONOSPACE, 1, sizeof(buf), "monospaced", 11);
	X(LIBFONTS_DEFAULT_MONOSPACE, 2, sizeof(buf));

	T(LIBFONTS_DEFAULT_SANS_SERIF, 0, 4, "san", 11);
	T(LIBFONTS_DEFAULT_SANS_SERIF, 1, 4, "san", 11);
	T(LIBFONTS_DEFAULT_SANS_SERIF, 2, 4, "san", 5);
	X(LIBFONTS_DEFAULT_SANS_SERIF, 3, 4);

	T(LIBFONTS_DEFAULT_SERIF, 0, 4, "ser", 6);
	X(LIBFONTS_DEFAULT_SERIF, 1, 4);

	T(LIBFONTS_DEFAULT_MONOSPACE, 0, 4, "mon", 10);
	T(LIBFONTS_DEFAULT_MONOSPACE, 1, 4, "mon", 11);
	X(LIBFONTS_DEFAULT_MONOSPACE, 2, 4);

	errno = 0;

	ASSERT(libfonts_get_default_font_name(LIBFONTS_DEFAULT_SANS_SERIF, NULL, 0, 0) == 11);
	ASSERT(libfonts_get_default_font_name(LIBFONTS_DEFAULT_SERIF, NULL, 0, 0) == 6);
	ASSERT(libfonts_get_default_font_name(LIBFONTS_DEFAULT_MONOSPACE, NULL, 0, 0) == 10);

	ASSERT(libfonts_get_default_font_name(LIBFONTS_DEFAULT_SANS_SERIF, NULL, 0, 3) == 0);
	ASSERT(libfonts_get_default_font_name(LIBFONTS_DEFAULT_SERIF, NULL, 0, 1) == 0);
	ASSERT(libfonts_get_default_font_name(LIBFONTS_DEFAULT_MONOSPACE, NULL, 0, 2) == 0);

	ASSERT(!errno);

	return 0;
}


#endif
