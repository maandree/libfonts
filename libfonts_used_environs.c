/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


const char *const libfonts_used_environs[] = {
	"LIBFONTS_DEFAULT_SANS_SERIF",
	"LIBFONTS_DEFAULT_SERIF",
	"LIBFONTS_DEFAULT_MONOSPACE",
	"LIBFONTS_FONT_DIRS",
	"XDG_CONFIG_HOME",
	"HOME",
	NULL
};


#else


int
main(void)
{
	size_t i, j, n;
	for (n = 0; n < 100; n++)
		if (!libfonts_used_environs[n])
			break;
	ASSERT(n < 100);
	for (i = 0; i < n; i++)
		for (j = i + 1; j < n; j++)
			ASSERT(strcmp(libfonts_used_environs[i], libfonts_used_environs[j]));
	return 0;
}


#endif
