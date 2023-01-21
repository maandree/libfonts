/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


#define LEVEL_MASK 0x0003
#define ENCODING_LEVEL 1
#define MAPPING_LEVEL 2
#define MAX_LEVEL MAPPING_LEVEL
#define HAVE_SIZE_BIT 0x4000
#define HAVE_FIRST_INDEX_BIT 0x2000
#define HAVE_FIRST_INDEX_BYTE_2_BIT 0x1000
#define HAVE_BITS (HAVE_SIZE_BIT | HAVE_FIRST_INDEX_BIT | HAVE_FIRST_INDEX_BYTE_2_BIT)

enum keyword {
	NO_KEYWORD,
	STARTENCODING,
	ENDENCODING,
	ALIAS,
	SIZE,
	FIRSTINDEX,
	STARTMAPPING,
	ENDMAPPING,
	UNDEFINE
};

static char *
get_string(const char *s, const char **endp)
{
	size_t len;
	const char *p;
	char *r;
	char *ret;

	for (len = 1, p = &s[1]; *p && *p != '\n' && *p != '#'; p++)
		len += (!isblank(*p) || !isblank(p[-1]));

	ret = malloc(len + 1);
	if (!ret)
		return NULL;

	r = ret;
	for (*r++ = *s++; *s && *s != '\n' && *s != '#'; s++) {
		if (!isblank(*s))
			*r++ = *s;
		else if (!isblank(s[-1]))
			*r++ = ' ';
	}
	*r = '\0';
	return ret;
}

static int
get_uints(uintmax_t *u1p, uintmax_t *u2p, uintmax_t *u3p, uintmax_t max,
          const char *s, const char **endp, struct libfonts_context *ctx)
{
	int r = 0;
	uintmax_t u, *up, digit;

	while (r < INT_MAX && isdigit(*s)) {
		u = 0;

		if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X') && isxdigit(s[2])) {
			s = &s[2];
			do {
				digit = (uintmax_t)((*s & 15) + (*s > '9' ? 9 : 0));
				if (u > (max - digit) >> 4)
					goto overflow;
				u = (u << 4) | digit;
			} while (isxdigit(*++s));

		} else if (s[0] == '0') {
			s = &s[1];
			for (;; s++) {
				digit = (uintmax_t)(unsigned char)(*s - '0');
				if (digit > 7)
					break;
				if (u > (max - digit) >> 3)
					goto overflow;
				u = (u << 3) | digit;
			}

		} else if (isdigit(s[0])) {
			do {
				digit = (uintmax_t)(*s & 15);
				if (u > (max - digit) / 10)
					goto overflow;
				u = u * 10 + digit;
			} while (isdigit(*++s));

		} else {
			break;
		}

		if (0) {
		overflow:
			u = max;
			while (isdigit(*++s));
			warning(ctx, 0, "libfonts_parse_encoding_line", "value too large: in excess of 0x%jX", max);
		}

		up = (r == 0 ? u1p :
		      r == 1 ? u2p :
		      r == 2 ? u3p : NULL);
		if (up)
			*up = u;
		r += 1;

		if (!*s || *s == '\n' || *s == '#')
			break;
		while (isblank(*s))
			s++;
	}

	*endp = s;
	return r;
}

int
libfonts_parse_encoding_line(struct libfonts_encoding **encodingp, int *statep,
                             const char *line, char **endp, struct libfonts_context *ctx)
{
	int ret = 0, r;
	size_t keyword_len;
	uintmax_t ju1, ju2, ju3;
	enum keyword keyword;
	struct libfonts_encoding_mapping *mapping;
	void *new;

	if (!encodingp ||
	    !statep ||
	    !line ||
	    (*statep & LEVEL_MASK) > MAX_LEVEL ||
	    ((*statep & LEVEL_MASK) ? (*statep & ~(LEVEL_MASK | HAVE_BITS)) : *statep) ||
	    (*statep && !*encodingp) ||
	    ((*statep & LEVEL_MASK) == MAPPING_LEVEL && !(*encodingp)->nmappings) ||
	    (*encodingp && ((!(*encodingp)->aliases && (*encodingp)->naliases) ||
	                    (!(*encodingp)->mappings && (*encodingp)->nmappings) ||
	                    (*encodingp)->size_rows > 0x100 ||
			    (*encodingp)->size_cols > 0x100))) {
		errno = EINVAL;
		goto fail;
	}

	while (isblank(*line))
		line++;

	if (!*line || *line == '#')
		goto out;

	if (!strncasecmp(line, "STARTENCODING", keyword_len = sizeof("STARTENCODING") - 1))
		keyword = STARTENCODING;
	else if (!strncasecmp(line, "ENDENCODING", keyword_len = sizeof("ENDENCODING") - 1))
		keyword = ENDENCODING;
	else if (!strncasecmp(line, "ALIAS", keyword_len = sizeof("ALIAS") - 1))
		keyword = ALIAS;
	else if (!strncasecmp(line, "SIZE", keyword_len = sizeof("SIZE") - 1))
		keyword = SIZE;
	else if (!strncasecmp(line, "FIRSTINDEX", keyword_len = sizeof("FIRSTINDEX") - 1))
		keyword = FIRSTINDEX;
	else if (!strncasecmp(line, "STARTMAPPING", keyword_len = sizeof("STARTMAPPING") - 1))
		keyword = STARTMAPPING;
	else if (!strncasecmp(line, "ENDMAPPING", keyword_len = sizeof("ENDMAPPING") - 1))
		keyword = ENDMAPPING;
	else if (!strncasecmp(line, "UNDEFINE", keyword_len = sizeof("UNDEFINE") - 1))
		keyword = UNDEFINE;
	else if (!isdigit(*line))
		goto bad_keyword;
	else
		keyword = NO_KEYWORD;

	if (keyword != NO_KEYWORD) {
		if (line[keyword_len] && !isblank(line[keyword_len]))
			goto bad_keyword;
		line = &line[keyword_len];
		while (isblank(*line))
			line++;
	}

	switch (keyword) {
	case STARTENCODING:
		if (*statep != 0) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "STARTENCODING nested inside another STARTENCODING");
			break;
		}
		if (!*encodingp) {
			*encodingp = calloc(1, sizeof(**encodingp));
			if (!*encodingp)
				goto enomem;
		}
		(*encodingp)->size_rows = 256;
		(*encodingp)->name = get_string(line, &line);
		if (!(*encodingp)->name)
			goto enomem;
		if (!*(*encodingp)->name)
			warning(ctx, 0, "libfonts_parse_encoding_line", "empty string as name of encoding");
		*statep = ENCODING_LEVEL;
		break;

	case ENDENCODING:
		if ((*statep & LEVEL_MASK) == MAPPING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "missing ENDMAPPING");
		} else if ((*statep & LEVEL_MASK) != ENCODING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "ENDENCODING without corresponding STARTENCODING");
			break;
		}
		if (*line && *line != '\n' && *line != '#')
			warning(ctx, 0, "libfonts_parse_encoding_line", "unexpected data after ENDENCODING keyword");
		if ((*statep & HAVE_FIRST_INDEX_BYTE_2_BIT) && (*encodingp)->size_cols) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "dual-byte FIRSTINDEX was combined with single-byte SIZE");
			(*encodingp)->first_index_col = 0;
		}
		ret = 1;
		*statep = 0;
		break;

	case ALIAS:
		if ((*statep & LEVEL_MASK) != ENCODING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "ALIAS keyword in wrong scope");
			break;
		}
		if ((*encodingp)->naliases > SIZE_MAX / sizeof(*(*encodingp)->aliases) - 1)
			goto enomem;
		new = realloc((*encodingp)->aliases, ((*encodingp)->naliases + 1) * sizeof(*(*encodingp)->aliases));
		if (!new)
			goto enomem;
		(*encodingp)->aliases = new;
		(*encodingp)->aliases[(*encodingp)->naliases] = get_string(line, &line);
		if (!(*encodingp)->aliases[(*encodingp)->naliases])
			goto enomem;
		if (!*(*encodingp)->aliases[(*encodingp)->naliases])
			warning(ctx, 0, "libfonts_parse_encoding_line", "empty string as alias name of encoding");
		(*encodingp)->naliases += 1;
		break;

	case SIZE:
		if ((*statep & LEVEL_MASK) != ENCODING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "SIZE keyword in wrong scope");
			break;
		} else if (*statep & HAVE_SIZE_BIT) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "multiple SIZE declarations in encoding");
		}
		ju2 = 0;
		r = get_uints(&ju1, &ju2, NULL, 0x100, line, &line, ctx);
		if (r < 0) {
			goto fail;
		} else if (r == 0) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "missing numerical data");
			break;
		} else if (r > 2) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "excess numerical data");
			break;
		} else if (*line && *line != '\n' && *line != '#') {
			warning(ctx, 0, "libfonts_parse_encoding_line", "unexpected non-numerical data");
			break;
		}
		if (!ju1 || (r > 1 && !ju2)) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "invalid SIZE of zero");
			break;
		}
		(*encodingp)->size_rows = (uint16_t)ju1;
		(*encodingp)->size_cols = (uint16_t)ju2;
		*statep |= HAVE_SIZE_BIT;
		break;

	case FIRSTINDEX:
		if ((*statep & LEVEL_MASK) != ENCODING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "FIRSTINDEX keyword in wrong scope");
			break;
		} else if (*statep & HAVE_FIRST_INDEX_BIT) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "multiple FIRSTINDEX declarations in encoding");
		}
		r = get_uints(&ju1, &ju2, NULL, 0xFF, line, &line, ctx);
		if (r < 0) {
			goto fail;
		} else if (r == 0) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "missing numerical data");
			break;
		} else if (r == 1) {
			ju2 = 0;
			*statep &= ~HAVE_FIRST_INDEX_BIT;
		} else if (r == 2) {
			*statep |= HAVE_FIRST_INDEX_BYTE_2_BIT;
		} else {
			warning(ctx, 0, "libfonts_parse_encoding_line", "excess numerical data");
			break;
		}
		if (*line && *line != '\n' && *line != '#') {
			warning(ctx, 0, "libfonts_parse_encoding_line", "unexpected non-numerical data");
			break;
		}
		(*encodingp)->first_index_row = (uint16_t)ju1;
		(*encodingp)->first_index_col = (uint16_t)ju2;
		*statep |= HAVE_FIRST_INDEX_BIT;
		break;

	case STARTMAPPING:
		if ((*statep & LEVEL_MASK) == MAPPING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "missing ENDMAPPING");
		} else if ((*statep & LEVEL_MASK) != ENCODING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "STARTMAPPING keyword in wrong scope");
			break;
		}
		if ((*encodingp)->nmappings > SIZE_MAX / sizeof(*(*encodingp)->mappings) - 1)
			goto enomem;
		new = realloc((*encodingp)->mappings, ((*encodingp)->nmappings + 1) * sizeof(*(*encodingp)->mappings));
		if (!new)
			goto enomem;
		(*encodingp)->mappings = new;
		(*encodingp)->mappings[(*encodingp)->nmappings].entries = NULL;
		(*encodingp)->mappings[(*encodingp)->nmappings].nentries = 0;
		(*encodingp)->mappings[(*encodingp)->nmappings].target = get_string(line, &line);
		if (!(*encodingp)->mappings[(*encodingp)->nmappings].target)
			goto enomem;
		if (!*(*encodingp)->mappings[(*encodingp)->nmappings].target)
			warning(ctx, 0, "libfonts_parse_encoding_line", "empty string as target encoding");
		(*encodingp)->nmappings += 1;
		*statep = (*statep & ~LEVEL_MASK) | MAPPING_LEVEL;
		break;

	case ENDMAPPING:
		if ((*statep & LEVEL_MASK) != MAPPING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "ENDMAPPING without corresponding STARTMAPPING");
			break;
		}
		if (*line && *line != '\n' && *line != '#')
			warning(ctx, 0, "libfonts_parse_encoding_line", "unexpected data after ENDMAPPING keyword");
		*statep = (*statep & ~LEVEL_MASK) | ENCODING_LEVEL;
		break;

	case UNDEFINE:
		if ((*statep & LEVEL_MASK) != MAPPING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "UNDEFINE keyword in wrong scope");
			break;
		}
		r = get_uints(&ju1, &ju2, NULL, UINT32_C(0xFFFFFFFF), line, &line, ctx);
		if (r < 0) {
			goto fail;
		} else if (r == 0) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "missing numerical data");
			break;
		} else if (r == 1) {
			ju2 = ju1;
		} else if (r == 2) {
			if (ju2 < ju1) {
				warning(ctx, 0, "libfonts_parse_encoding_line", "reversed range");
				break;
			}
		} else {
			warning(ctx, 0, "libfonts_parse_encoding_line", "excess numerical data");
			break;
		}
		if (*line && *line != '\n' && *line != '#') {
			warning(ctx, 0, "libfonts_parse_encoding_line", "unexpected non-numerical data");
			break;
		}
		mapping = &(*encodingp)->mappings[(*encodingp)->nmappings - 1];
		if (mapping->nentries > SIZE_MAX / sizeof(*mapping->entries) - 1)
			goto enomem;
		new = realloc(mapping->entries, (mapping->nentries + 1) * sizeof(*mapping->entries));
		if (!new)
			goto enomem;
		mapping->entries = new;
		mapping->entries[mapping->nentries].undefined_range.type = LIBFONTS_ENCODING_MAPPING_ENTRY_UNDEFINED_RANGE;
		mapping->entries[mapping->nentries].undefined_range.low_source = (uint32_t)ju1;
		mapping->entries[mapping->nentries].undefined_range.high_source = (uint32_t)ju2;
		mapping->nentries += 1;
		break;

	case NO_KEYWORD:
		if ((*statep & LEVEL_MASK) != MAPPING_LEVEL) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "index mapping in wrong scope");
			break;
		}
		r = get_uints(&ju1, &ju2, &ju3, UINT32_C(0xFFFFFFFF), line, &line, ctx);
		if (r < 0) {
			goto fail;
		} else if (r == 0) {
			warning(ctx, 0, "libfonts_parse_encoding_line", "internal error");
			break;
		} else if (r == 1) {
			if (!*line || *line == '\n' || *line == '#') {
				warning(ctx, 0, "libfonts_parse_encoding_line", "missing data");
				break;
			}
		} else if (r == 2) {
			ju3 = ju2;
			ju2 = ju1;
		} else if (r == 3) {
			if (ju2 < ju1) {
				warning(ctx, 0, "libfonts_parse_encoding_line", "reversed range");
				break;
			}
		} else {
			warning(ctx, 0, "libfonts_parse_encoding_line", "excess numerical data");
			break;
		}
		if (r != 1 && *line && *line != '\n' && *line != '#') {
			warning(ctx, 0, "libfonts_parse_encoding_line", "unexpected non-numerical data");
			break;
		}

		mapping = &(*encodingp)->mappings[(*encodingp)->nmappings - 1];
		if (mapping->nentries > SIZE_MAX / sizeof(*mapping->entries) - 1)
			goto enomem;
		new = realloc(mapping->entries, (mapping->nentries + 1) * sizeof(*mapping->entries));
		if (!new)
			goto enomem;
		mapping->entries = new;
		if (r != 1) {
			mapping->entries[mapping->nentries].remapped_range.type = LIBFONTS_ENCODING_MAPPING_ENTRY_REMAPPED_RANGE;
			mapping->entries[mapping->nentries].remapped_range.low_source = (uint32_t)ju1;
			mapping->entries[mapping->nentries].remapped_range.high_source = (uint32_t)ju2;
			mapping->entries[mapping->nentries].remapped_range.low_target = (uint32_t)ju3;
		} else {
			mapping->entries[mapping->nentries].index_to_name.type = LIBFONTS_ENCODING_MAPPING_ENTRY_INDEX_TO_NAME;
			mapping->entries[mapping->nentries].index_to_name.source = (uint32_t)ju1;
			mapping->entries[mapping->nentries].index_to_name.target = get_string(line, &line);
			if (!mapping->entries[mapping->nentries].index_to_name.target)
				goto enomem;
		}
		mapping->nentries += 1;
		break;

	default:
		warning(ctx, 0, "libfonts_parse_encoding_line", "internal error");
		break;
	}

out:
	if (line)
		while (*line && *line != '\n')
			line++;
	if (endp)
		*endp = *(char **)(void *)&line;
	return ret;

enomem:
	errno = ENOMEM;
fail:
	ret = -1;
	goto out;

bad_keyword:
	warning(ctx, 0, "libfonts_parse_encoding_line", "unrecognised keyword");
	goto out;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
