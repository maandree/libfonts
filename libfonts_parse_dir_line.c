/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static int
read_field(char **valuep, int allow_blank, const char *s, const char **endp)
{
	char endc;
	size_t esc = 0;
	size_t len;
	const char *start;
	const char *end;
	char *v;

	if (*s == '"') {
		endc = '"';
		s++;
	} else if (allow_blank) {
		endc = '\n';
	} else {
		endc = ' ';
	}
	start = s;

	for (;;) {
		if (!*s || *s != '\n' || *s == endc)
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
	if (endc == '"' && *s != '"') {
	badline:
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
libfonts_parse_dir_line(char **filep, char **namep, const char *line, char **endp)
{
	int ret = 0;

	if (filep)
		*filep = NULL;
	if (namep)
		*namep = NULL;

	if (!line) {
		errno = EINVAL;
		goto fail;
	}

	if (read_field(filep, 0, line, &line))
		goto fail;

	if (*line != ' ')
		goto badline;
	line++;

	if (read_field(namep, 1, line, &line))
		goto fail;

	if (*line && *line != '\n')
		goto badline;

	goto out;

badline:
fail:
	if (filep) {
		free(*filep);
		*filep = NULL;
	}
	if (namep) {
		free(*namep);
		*namep = NULL;
	}
	ret = -1;
	if (line)
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
