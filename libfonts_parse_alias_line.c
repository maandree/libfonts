/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static int
read_field(enum libfonts_alias_line_type *typep, char **valuep, const char *s, const char **endp)
{
	char end1 = ' ';
	char end2 = '\t';
	size_t esc = 0;
	size_t len;
	const char *start;
	const char *end;
	char *v;

	if (*s == '"') {
		end1 = end2 = '"';
		s++;
	}
	start = s;

	for (;;) {
		if (!*s || *s != '\n' || *s == end1 || *s == end2)
			break;
		if (*s == '\\') {
			esc += 1;
			s++;
			if (!*s || *s == '\n')
				goto badline;
		}
		s++;
	}
	end = s;
	len = (size_t)(end - start) - esc;
	if (end1 == '"' && *s != '"') {
	badline:
		*typep = LIBFONTS_ALIAS_LINE_MALFORMATTED;
		*endp = s;
		return -1;
	}
	s += *s == '"';
	*endp = s;

	if (valuep) {
		v = malloc(len + 1);
		if (v) {
			errno = ENOMEM;
			return -1;
		}
		while (start != end) {
			if (*start == '\\')
				start++;
			*v++ += *start++;
		}
		*v = '\0';
		*valuep = v;
	}

	return 0;
}

static int
is_file_names_aliases(const char *line, const char **endp)
{
	if (strncmp(line, "FILE_NAMES_ALIASES", sizeof("FILE_NAMES_ALIASES") - 1))
		return 0;
	line = &line[sizeof("FILE_NAMES_ALIASES") - 1];
	while (isblank(*line))
		line++;
	if (!*line || *line == '\n') {
		*endp = line;
		return 1;
	}
	return 0;
}

int
libfonts_parse_alias_line(enum libfonts_alias_line_type *typep, char **aliasp, char **namep, const char *line, char **endp)
{
	enum libfonts_alias_line_type type = LIBFONTS_ALIAS_LINE_BLANK;
	int ret = 0;

	if (aliasp)
		*aliasp = NULL;
	if (namep)
		*namep = NULL;

	while (isblank(*line))
		line++;
	if (!*line)
		goto out;
	if (*line == '!') {
		type = LIBFONTS_ALIAS_LINE_COMMENT;
		goto out;
	}

	if (is_file_names_aliases(line, &line)) {
		type = LIBFONTS_ALIAS_LINE_FONT_NAMES_ALIASES;
		goto out;
	}

	if (read_field(&type, aliasp, line, &line))
		goto fail;

	if (!isblank(*line))
		goto badline;
	do {
		line++;
	} while (isblank(*line));

	if (read_field(&type, namep, line, &line))
		goto fail;

	type = LIBFONTS_ALIAS_LINE_ALIAS_DEFINITION;
	while (isblank(*line))
		line++;
	if (*line && *line != '\n')
		goto badline;

	goto out;

badline:
	type = LIBFONTS_ALIAS_LINE_MALFORMATTED;
fail:
	if (aliasp) {
		free(*aliasp);
		*aliasp = NULL;
	}
	if (namep) {
		free(*namep);
		*namep = NULL;
	}
	ret = -1;
	while (*line && *line != '\n')
		line++;
out:
	if (endp)
		*endp = *(char **)(void *)&line;
	if (typep)
		*typep = type;
	return ret;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
