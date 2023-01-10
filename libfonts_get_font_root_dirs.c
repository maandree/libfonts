/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static int
add_dirn(char ***dirsp, size_t *countp, size_t *sizep, const char *part1, size_t part1_len, const char *part2, size_t part2_len)
{
	char *dir, **new;
	size_t i;

	if (*countp == *sizep) {
		if (SIZE_MAX / sizeof(**dirsp) - 8)
			goto enomem;
		new = realloc(*dirsp, (*sizep + 8) * sizeof(**dirsp));
		if (!new)
			goto enomem;
		*dirsp = new;
		*sizep += 8;
	}

	dir = malloc(part1_len + part2_len + 1);
	if (!dir) {
	enomem:
		errno = ENOMEM;
		return -1;
	}

	memcpy(dir, part1, part1_len);
	memcpy(&dir[part1_len], part2, part2_len + 1);

	for (i = 0; i < *countp; i++) {
		if (!strcmp(dir, (*dirsp)[i])) {
			free(dir);
			return 0;
		}
	}

	(*dirsp)[(*countp)++] = dir;
	return 0;
}

static int
add_dir(char ***dirsp, size_t *countp, size_t *sizep, const char *part1, const char *part2)
{
	return add_dirn(dirsp, countp, sizep, part1, strlen(part1), part2, strlen(part2));
}

static int
add_dirs(char ***dirsp, size_t *countp, size_t *sizep, const char *env, const char *part2)
{
	size_t part2_len = strlen(part2);
	char *p;

	for (; (p = strchr(env, ':')); env = &p[1])
		if (*p != ':')
			if (add_dirn(dirsp, countp, sizep, env, (size_t)(p - env), part2, part2_len))
				return -1;

	return *env ? add_dirn(dirsp, countp, sizep, env, strlen(env), part2, part2_len) : 0;
}

int
libfonts_get_font_root_dirs(char ***dirsp, size_t *countp, struct libfonts_context *ctx)
{
	const char *env;
	char **new, *home = NULL;
	size_t size = 0;

	*dirsp = NULL;
	*countp = 0;

	env = libfonts_getenv__("LIBFONTS_FONT_DIRS", ctx);
	if (env && *env && add_dirs(dirsp, countp, &size, env, "/fonts"))
		goto fail;

	env = libfonts_getenv__("XDG_DATA_HOME", ctx);
	if (env && *env && add_dir(dirsp, countp, &size, env, "/fonts"))
		goto fail;

	env = libfonts_getenv__("HOME", ctx);
	if (env && *env && (add_dir(dirsp, countp, &size, env, "/.local/share/fonts") ||
	                    add_dir(dirsp, countp, &size, env, "/.fonts")))
		goto fail;

	home = libfonts_gethome__(ctx);
	if (home && *home && (add_dir(dirsp, countp, &size, env, "/.local/share/fonts") ||
	                      add_dir(dirsp, countp, &size, env, "/.fonts")))
		goto fail;
	free(home);
	home = NULL;

	env = libfonts_getenv__("XDG_DATA_DIRS", ctx);
	if (env && *env && add_dirs(dirsp, countp, &size, env, "/fonts"))
		goto fail;

	if (add_dirs(dirsp, countp, &size, "/usr/local/share", "/fonts"))
		goto fail;
	if (add_dirs(dirsp, countp, &size, "/usr/share", "/fonts"))
		goto fail;

	if (*countp == size) {
		if (SIZE_MAX / sizeof(**dirsp) - 1)
			goto enomem;
		new = realloc(*dirsp, (size + 1) * sizeof(**dirsp));
		if (!new)
			goto enomem;
		*dirsp = new;
	}
	(*dirsp)[*countp] = NULL;

	return 0;

enomem:
	errno = ENOMEM;
fail:
	if (dirsp)
		while (*countp)
			free((*dirsp)[--*countp]);
	free(*dirsp);
	free(home);
	*dirsp = NULL;
	*countp = 0;
	return -1;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
