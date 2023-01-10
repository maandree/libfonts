/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_do_font_descriptions_match(const char *desc, const char *spec)
{
	if (!spec)
		return !desc;
	if (!desc)
		return 0;

	while (*spec && *desc) {
		if (spec[0] == '*' && (!spec[1] || spec[1] == '-')) {
			spec++;
			while (*desc && *desc != '-')
				desc++;
		} else {
			while (*spec && *desc && *spec != '-' && *desc != '-') {
				if (*spec != *desc && *spec != '?')
					return 0;
			}
		}
		if (*spec != *desc || (*spec && *spec != '-'))
			return 0;
	}

	return *spec == *desc;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
