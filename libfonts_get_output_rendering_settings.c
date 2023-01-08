/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static int
getn(const char *file_part1, size_t file_part1_len, const char *file_part2,
     struct libfonts_rendering_settings *settings, const char *name, struct libfonts_context *ctx)
{
	size_t file_part2_len = strlen(file_part2);
	char *path;
	int fd;
	ssize_t len;
	char *line, *buf = NULL;
	size_t size = 0, off = 0, avail = 0;
	int found = 0;
	int in_the_section = 0;
	int in_a_section = 0;
	char *value;
	unsigned int found_fields = 0;
	size_t lineno = 0;

	if (file_part1_len > SIZE_MAX - file_part2_len - 1)
		goto enomem;

	path = malloc(file_part1_len + file_part2_len + 1);
	if (!path) {
	enomem:
		errno = ENOMEM;
		return -1;
	}

	memcpy(path, file_part1, file_part1_len);
	memcpy(&path[file_part1_len], file_part2, file_part2_len + 1);

open_again:
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		switch (errno) {
		case EINTR:
			goto open_again;
		case EMFILE:
		case ENFILE:
		case ENOMEM:
		case ENOSPC:
			free(path);
			return -1;
		case EFBIG:
		case EOVERFLOW:
		case EISDIR:
		case ELOOP:
		case ENODEV:
		case ENOTDIR:
		case ENXIO:
			warning(ctx, errno, "libfonts_get_default_rendering_settings", "failed to open %s:", path);
			goto out;
		default:
			goto out;
		}
	}

	for (;;) {
		len = libfonts_getline__(fd, &line, &buf, &size, &off, &avail);
		if (len < 0) {
			if (errno == EINTR)
				continue;
			free(buf);
			free(path);
			close(fd);
			return -1;
		}
		if (!len)
			break;
		line[len -= 1] = '\0';
		lineno++;

		while (isblank(*line)) {
			line++;
			len--;
		}
		if (!*line || *line == '#')
			continue;
		while (len && isblank(line[len - 1]))
			len -= 1;
		line[len] = '\0';

		if (line[0] == '[' && line[len - 1] == ']') {
			in_a_section = 1;
			line = &line[1];
			len -= 2;
			line[len] = '\0';
			found |= in_the_section = !strcmp(line, name);
			found_fields = 0;

		} else if (!in_a_section) {
			value = libfonts_confsplit__(line);
			if (!value) {
				warning(ctx, 0, "libfonts_get_output_rendering_settings", "bad line in %s at line %zu", path, lineno);
				continue;
			}
			/* TODO aliases should be declarable above the first "[%s]" */

		} else if (in_the_section) {
			value = libfonts_confsplit__(line);
			if (!value) {
				warning(ctx, 0, "libfonts_get_output_rendering_settings", "bad line in %s at line %zu", path, lineno);
				continue;
			}
#define X(INDEX, CONFNAME, CNAME, DEFVAL, PARSER)\
			if (!strcmp(line, CONFNAME)) {\
				if (found_fields & (1U << INDEX)) {\
					warning(ctx, 0, "libfonts_get_output_rendering_settings",\
					        "duplicate definition in %s at line %zu", path, lineno); \
				}\
				found_fields |= (1U << INDEX);\
				if (!PARSER(&settings->CNAME, value)) {\
					warning(ctx, 0, "libfonts_get_output_rendering_settings",\
					        "invalid value in %s at line %zu: %s", path, lineno, value);\
				}\
			} else
			LIST_RENDERING_SETTINGS(X,) {
				warning(ctx, 0, "libfonts_get_output_rendering_settings",\
				        "bad definition in %s at line %zu: %s", path, lineno, line);\
			}
#undef X
		}
	}

	free(buf);
	close(fd);

out:
	free(path);
	return found;
}

static int
get(const char *file_part1, const char *file_part2, struct libfonts_rendering_settings *settings,
    const char *name, struct libfonts_context *ctx)
{
	return getn(file_part1, strlen(file_part1), file_part2, settings, name, ctx);
}

int
libfonts_get_output_rendering_settings(struct libfonts_rendering_settings *settings, const char *name, struct libfonts_context *ctx)
{

	const char *env, *next;
	char *home = NULL;
	int saved_errno = errno;
	int ret = 0;
	size_t len;

	if (!name) {
		errno = EINVAL;
		return -1;
	}

	if (settings) {
#define X(INDEX, CONFNAME, CNAME, DEFVAL, PARSER) settings->CNAME = DEFVAL
		LIST_RENDERING_SETTINGS(X, ;);
#undef X
	}

	env = libfonts_getenv__("XDG_CONFIG_HOME", ctx);
	if (env && *env) {
		ret = get(env, "/libfonts/output-rendering.conf", settings, name, ctx);
		if (ret)
			goto out;
	}

	env = libfonts_getenv__("HOME", ctx);
	if (env && *env) {
		ret = get(env, "/.config/libfonts/output-rendering.conf", settings, name, ctx);
		if (ret)
			goto out;
		ret = get(env, "/.libfonts/output-rendering.conf", settings, name, ctx);
		if (ret)
			goto out;
	}

	home = libfonts_gethome__(ctx);
	if (home && *home) {
		ret = get(home, "/.config/libfonts/output-rendering.conf", settings, name, ctx);
		if (ret)
			goto out;
		ret = get(home, "/.libfonts/output-rendering.conf", settings, name, ctx);
		if (ret)
			goto out;
	}

	env = libfonts_getenv__("XDG_CONFIG_DIRS", ctx);
	if (env && *env) {
		do {
			next = strchr(&env[1], ':');
			len = next ? (size_t)(next - env) : strlen(env);
			if (len) {
				ret = getn(env, len, "/libfonts/output-rendering.conf", settings, name, ctx);
				if (ret)
					goto out;
			}
			env += len + 1;
		} while (next);
	}

	ret = get("/etc", "/libfonts/output-rendering.conf", settings, name, ctx);

out:
	free(home);
	if (ret >= 0)
		errno = saved_errno;
	return ret;
}


#else


int
main(void)
{
	return 0; /* TODO add test */
}


#endif
