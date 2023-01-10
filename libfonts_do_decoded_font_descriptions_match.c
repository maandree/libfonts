/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


#define LIST_ALL_FIELDS(X, _)\
	X(xlfd_version) _\
	X(foundry) _\
	X(family_name) _\
	X(weight_name) _\
	X(slant) _\
	X(setwidth_name) _\
	X(add_style_name) _\
	X(pixel_size) _\
	X(point_size) _\
	X(resolution_x) _\
	X(resolution_y) _\
	X(spacing) _\
	X(average_width) _\
	X(charset_registry) _\
	X(charset_encoding) _\
	X(charset_subset)


static int
equal(const char *desc, const char *spec)
{
	size_t i;

	if (!spec)
		return !desc;
	if (spec[0] == '*' && !spec[1])
		return 1;
	if (!desc)
		return 0;

	for (; desc[i] && spec[i]; i++)
		if (spec[i] != desc[i] && spec[i] != '?')
			return 0;

	return desc[i] == spec[i];
}

static int
many_equal(const char *desc, const char *spec)
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

int
libfonts_do_decoded_font_descriptions_match(const struct libfonts_font_description *desc,
                                            const struct libfonts_font_description *spec)
{
	if (desc->private_font_name || spec->private_font_name)
		return many_equal(desc->private_font_name, spec->private_font_name);

#define X(F)\
	if (!equal(desc->F, spec->F))\
		return 0
	LIST_ALL_FIELDS(X, ;);
#undef X

	return many_equal(desc->unrecognised_fields, spec->unrecognised_fields);
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
