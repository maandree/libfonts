/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static int
read_field(char **valuep, const char *s, const char **endp)
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
				goto ebadmsg;
		}
		s++;
	}
	end = s;
	len = (size_t)(end - start) - esc;
	if (end1 == '"' && *s != '"') {
	ebadmsg:
		errno = EBADMSG;
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

int
libfonts_parse_alias_line(char **aliasp, char **namep, const char *line, char **endp)
{
	int ret = 0;

	if (aliasp)
		*aliasp = NULL;
	if (namep)
		*namep = NULL;

	while (isblank(*line))
		line++;
	if (!*line || *line == '!')
		goto out;

	if (read_field(aliasp, line, &line))
		goto fail;

	if (!isblank(*line))
		goto ebadmsg;
	do {
		line++;
	} while (isblank(*line));

	if (read_field(namep, line, &line))
		goto fail;

	while (isblank(*line))
		line++;
	if (*line && *line != '\n')
		goto ebadmsg;

	ret = 1;
	goto out;

ebadmsg:
	errno = EBADMSG;
fail:
	ret = -1;
	while (*line && *line != '\n')
		line++;
out:
	if (endp)
		*endp = *(char **)(void *)&line;
	return ret;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
