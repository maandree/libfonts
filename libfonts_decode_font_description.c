/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


#define LIST_FIELDS_EXCEPT_FINAL(X, _)\
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
	X(charset_registry)


struct range {
	uint32_t first;
	uint32_t last;
};


static int
parse_hexadecimal(const char *s, const char **endp, uint32_t *valuep)
{
	uint64_t r = 0;
	if (!isxdigit(*s))
		return -1;
	for (;; s++) {
		if (isdigit(*s))
			r = (r * 16) | (*s & 15);
		else if (isxdigit(*s))
			r = (r * 16) | ((*s & 15) + 9);
		else
			break;
		if (r > INT32_MAX)
			return -1;
	}
	*endp = s;
	*valuep = (uint32_t)r;
	return 0;
}


static int
parse_decimal(const char *s, const char **endp, uint32_t *valuep)
{
	uint64_t r = 0;
	if (!isdigit(*s))
		return -1;
	for (; isdigit(*s); s++) {
		r = r * 10 + (*s & 15);
		if (r > INT32_MAX)
			return -1;
	}
	*endp = s;
	*valuep = (uint32_t)r;
	return 0;
}


static int
parse_number(const char *s, const char **endp, uint32_t *valuep)
{
	if (s[0] == '0' && s[1] == 'x')
		return parse_hexadecimal(&s[2], endp, valuep);
	else
		return parse_decimal(&s[0], endp, valuep);
}


static int
cmprange(const void *av, const void *bv)
{
	const struct range *a = av, *b = bv;
	return a->first < b->first ? -1 : a->first > b->first;
}


static int
fix_charset_subset(char *out, const char *in)
{
	struct range ranges[128], saved;
	size_t i, n = 0;
	const char *s;
	uint32_t t;

	for (s = in; *s; s++) {
		if (*s == ' ')
			continue;
		if (parse_number(s, &s, &ranges[0].first))
			return -1;
		if (*s == '_') {
			if (parse_number(&s[1], &s, &ranges[0].last))
				return -1;
			if (ranges[0].first > ranges[0].last) {
				t = ranges[0].first;
				ranges[0].first = ranges[0].last;
				ranges[0].last = t;
			}
		} else {
			ranges[0].last = ranges[0].first;
		}
		if (*s && *s != ' ')
			return -1;
	}

	if (!n) {
		*out = '\0';
		return 0;
	}

	qsort(ranges, n, sizeof(*ranges), cmprange);
	saved = ranges[0];
	for (i = 1; i < n; i++) {
		if (ranges[i].first <= saved.last) {
			if (ranges[i].last > saved.last)
				saved.last = ranges[i].last;
		} else {
			if (saved.first == saved.last)
				out += sprintf(out, "%"PRIu32" ", saved.first);
			else
				out += sprintf(out, "%"PRIu32"-%"PRIu32" ", saved.first, saved.last);
			saved = ranges[i];
		}
	}
	if (saved.first == saved.last)
		out += sprintf(out, "%"PRIu32" ", saved.first);
	else
		out += sprintf(out, "%"PRIu32"-%"PRIu32" ", saved.first, saved.last);

	out[-1] = '\0';
	return 0;
}


int
libfonts_decode_font_description(struct libfonts_font_description *desc, const char *in)
{
	char subset[256];
	const char *s = in;
	char *buf, *ss = subset;
	int have_subset = 0;
	int in_subset = 0;

	if (strlen(in) > 255) {
		errno = EINVAL;
		return -1;
	}

	memset(desc, 0, sizeof(*desc));
	buf = desc->_buf;

	if (*s == '+') {
		desc->xlfd_version = buf;
		for (s++; *s && *s != '-'; s++)
			*buf++ = *s == '~' ? '-' : *s;
		*buf++ = '\0';
	}

#define X(F)\
	if (*s != '-')\
		goto private;\
	desc->F = buf;\
	for (s++; *s && *s != '-'; s++)\
		*buf++ = *s == '~' ? '-' : *s;\
	*buf++ = '\0'
	LIST_FIELDS_EXCEPT_FINAL(X, ;);
#undef X

	if (*s != '-')
		goto private;
	desc->charset_encoding = buf;
	for (s++; *s && *s != '-'; s++) {
		if (in_subset) {
			if (*s == ']')
				in_subset = 0;
			else
				*ss++ = *s == '~' ? '-' : *s;
		} else {
			if (*s == '[') {
				in_subset = 1;
				if (have_subset)
					*ss++ = ' ';
				else
					have_subset = 1;
			} else {
				*buf++ = *s == '~' ? '-' : *s;
			}
		}
	}
	*buf++ = '\0';
	if (in_subset)
		goto private;
	if (have_subset) {
		*ss = '\0';
		desc->charset_subset = buf;
		if (fix_charset_subset(buf, subset))
			goto private;
	}

	if (*s == '-') {
		desc->unrecognised_fields = buf;
		stpcpy(buf, &s[1]);
	}

	return 0;

private:
	memset(desc, 0, sizeof(*desc));
	stpcpy(desc->_buf, in);
	desc->private_font_name = desc->_buf;
	return 0;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
