/* See LICENSE file for copyright and license details. */
#include "common.h"


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
	return (ssize_t)len;
}
