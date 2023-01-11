/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_parse_alias_line(char **aliasp, char **namep, const char *line, char **endp)
{
	const char *alias_start;
	const char *alias_end;
	const char *name_start;
	const char *name_end;
	int ret = 0;
	size_t len;

	if (aliasp)
		*aliasp = NULL;
	if (namep)
		*namep = NULL;

	while (isblank(*line))
		line++;

	if (!*line || *line == '!')
		goto out;

	if (*line == '"') {
		alias_start = ++line;
		while (*line && *line != '\n' && *line != '"')
			line++;
		if (*line != '"')
			goto ebadmsg;
		alias_end = line++;
	} else {
		alias_start = line;
		while (*line && *line != '\n' && isblank(*line))
			line++;
		alias_end = line;
	}

	if (!isblank(*line))
		goto ebadmsg;
	do {
		line++;
	} while (isblank(*line));

	if (*line == '"') {
		name_start = ++line;
		while (*line && *line != '\n' && *line != '"')
			line++;
		if (*line != '"')
			goto ebadmsg;
		name_end = line++;
	} else {
		name_start = line;
		while (*line && *line != '\n' && isblank(*line))
			line++;
		name_end = line;
	}

	while (isblank(*line))
		line++;
	if (*line && *line != '\n')
		goto ebadmsg;

	if (aliasp) {
		len = (size_t)(alias_end - alias_start);
		*aliasp = malloc(len + 1);
		if (!*aliasp)
			goto enomem;
		memcpy(*aliasp, alias_start, len);
		(*aliasp)[len] = '\0';
	}

	if (namep) {
		len = (size_t)(name_end - name_start);
		*namep = malloc(len + 1);
		if (!*namep)
			goto enomem;
		memcpy(*namep, name_start, len);
		(*namep)[len] = '\0';
	}

	*endp = *(char **)(void *)&line;
	return 1;

ebadmsg:
	errno = EBADMSG;
	ret = -1;
out:
	while (*line && *line != '\n')
		line++;
out_at_end:
	if (endp)
		*endp = *(char **)(void *)&line;
	return ret;

enomem:
	if (aliasp) {
		free(*aliasp);
		*aliasp = NULL;
	}
	errno = ENOMEM;
	ret = -1;
	goto out_at_end;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
