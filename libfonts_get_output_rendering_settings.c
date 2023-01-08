/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static int
getn(const char *file_part1, size_t file_part1_len, const char *file_part2,
     struct libfonts_rendering_settings *settings, const char *name, struct libfonts_context *ctx)
{
	size_t file_part2_len = strlen(file_part2);
	char *path;

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

	/* TODO aliases should be declarable above the first "[$name]" */
	/* TODO look for "[$name]" */
	/* TODO (use LIST_RENDERING_SETTINGS) */

	free(path);
	return 1;
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
#define X(CONFNAME, CNAME, DEFVAL) settings->CNAME = DEFVAL
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
