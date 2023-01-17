/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


#define LIST_ALL_FIELDS(X, _)\
	X(xlfd_version, equal) _\
	X(foundry, equal) _\
	X(family_name, equal) _\
	X(weight_name, equal) _\
	X(slant, equal) _\
	X(setwidth_name, equal) _\
	X(add_style_name, equal) _\
	X(pixel_size, zero_or_equal) _\
	X(point_size, zero_or_equal) _\
	X(resolution_x, zero_or_equal) _\
	X(resolution_y, zero_or_equal) _\
	X(spacing, equal) _\
	X(average_width, equal) _\
	X(charset_registry, equal) _\
	X(charset_encoding, equal) _\
	X(charset_subset, super)


static const char *
read_uint32(uint32_t *restrict nump, const char *s)
{
	uint32_t r;
	for (r = 0; isdigit(*s); s++)
		r = r * 10 + (uint32_t)(*s & 15);
	*nump = r;
	return s;
}

static int
super(const char *desc, const char *spec)
{
	uint32_t dl = 0, dh = 0, sl = 0, sh = 0;
	int have_d = 0, have_s = 0;

	if (!spec || !desc)
		return 1;

	while (*desc && *spec) {
		if (!have_d) {
			have_d = 1;
			if (!isdigit(*desc))
				return 0;
			desc = read_uint32(&dl, desc);
			if (desc[0] == '-' && isdigit(desc[1]))
				desc = read_uint32(&dh, &desc[1]);
			else
				dh = dl;
			if (*desc) {
				if (*desc != ' ')
					return 0;
				desc++;
			}
		}

		if (!have_s) {
			have_s = 1;
			if (!isdigit(*spec))
				return 0;
			spec = read_uint32(&sl, spec);
			if (spec[0] == '-' && isdigit(spec[1]))
				spec = read_uint32(&sh, spec);
			else
				sh = sl;
			if (*spec) {
				if (*spec != ' ')
					return 0;
				spec++;
			}
		}

		if (sl < dl)
			return 0;
		else if (sl > dh)
			have_d = 0;
		else if (sh > dh)
			return 0;
		else
			have_s = 0;
	}

	return !*spec;
}

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
		if (tolower(spec[i]) != tolower(desc[i]) && spec[i] != '?')
			return 0;

	return desc[i] == spec[i];
}

static int
zero_or_equal(const char *desc, const char *spec)
{
	if (desc && desc[0] == '0' && !desc[1])
		return 1;
	else
		return equal(desc, spec);
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
				if (tolower(*spec) != tolower(*desc) && *spec != '?')
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

#define X(FIELD, FUNC)\
	if (!FUNC(desc->FIELD, spec->FIELD))\
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
