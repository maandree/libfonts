/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


#define LIST_FIELDS(X, _)\
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
	X(charset_encoding)


static char *
encode(char *out, const char *s)
{
	for (; *s; s++)
		*out++ = (*s == '-' ? '~' : *s);
	return out;
}


static char *
encode_range(char *out, const char *s)
{
	if (*s == '-') {
		*out++ = '~';
		s++;
	}
	for (; *s; s++)
		*out++ = (*s == '-' ? isdigit(s[-1]) ? '_' : '~' : *s);
	return out;
}


int
libfonts_encode_font_description(const struct libfonts_font_description *desc, char out[static 256])
{
	size_t n = 1;
	char *p;

	if (desc->private_font_name) {
		if (strlen(desc->private_font_name) > 255)
			goto einval;
		stpcpy(out, desc->private_font_name);
		return 0;
	}

	if (desc->xlfd_version)
		n += strlen(desc->xlfd_version) + 1;

#define X(F)\
	if (!desc->F)\
		goto einval;\
	n += strlen(desc->F) + 1
	LIST_FIELDS(X, ;);
#undef X

	if (desc->charset_subset)
		n += strlen(desc->charset_subset) + 2;

	if (desc->unrecognised_fields)
		n += strlen(desc->unrecognised_fields) + 1;

	if (n > 255)
		goto einval;

	p = out;
	if (desc->xlfd_version) {
		*p++ = '+';
		p = encode(p, desc->xlfd_version);
	}

#define X(F)\
	*p++ = '-';\
	p = encode(p, desc->F)
	LIST_FIELDS(X, ;);
#undef X

	if (desc->charset_subset) {
		*p++ = '[';
		p = encode_range(p, desc->charset_subset);
		*p++ = ']';
	}

	if (desc->unrecognised_fields) {
		*p++ = '-';
		p = stpcpy(p, desc->unrecognised_fields);
	}

	*p = '\0';
	return 0;

einval:
	errno = EINVAL;
	return -1;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
