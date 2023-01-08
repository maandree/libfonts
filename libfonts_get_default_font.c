/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static char *
find(const char *dir_part1, const char *dir_part2, const char *dir_part3, struct libfonts_context *ctx)
{
	return NULL; /* TODO */
}

static char *
getn(const char *file_part1, size_t file_part1_len, const char *file_part2, const char *name, struct libfonts_context *ctx)
{
	size_t file_part2_len = strlen(file_part2);
	char *path;

	if (file_part1_len > SIZE_MAX - file_part2_len - 1)
		goto enomem;

	path = malloc(file_part1_len + file_part2_len + 1);
	if (!path) {
	enomem:
		errno = ENOMEM;
		return NULL; /* TODO abort function */
	}

	memcpy(path, file_part1, file_part1_len);
	memcpy(&path[file_part1_len], file_part2, file_part2_len + 1);

	/* TODO
	 *     sans-serif = $FONTNAME
	 *     serif = $FONTNAME
	 *     monospace = $FONTNAME
	 */

	free(path);
	return NULL;
}

static char *
get(const char *file_part1, const char *file_part2, const char *name, struct libfonts_context *ctx)
{
	return getn(file_part1, strlen(file_part1), file_part2, name, ctx);
}

char *
libfonts_get_default_font(enum libfonts_default_font font, struct libfonts_context *ctx)
{
	const char *env, *var, *next, *confenv, *homeenv;
	char *ret, *home = NULL;
	int saved_errno = errno;
	size_t len;

	if (font == LIBFONTS_DEFAULT_SANS_SERIF) {
		env = "LIBFONTS_DEFAULT_SANS_SERIF";
		var = "sans-serif";
	} else if (font == LIBFONTS_DEFAULT_SERIF) {
		env = "LIBFONTS_DEFAULT_SERIF";
		var = "serif";
	} else if (font == LIBFONTS_DEFAULT_MONOSPACE) {
		env = "LIBFONTS_DEFAULT_MONOSPACE";
		var = "monospace";
	} else {
		errno = EINVAL;
		return NULL;
	}

	env = libfonts_getenv__(env, ctx);
	if (env && *env)
		return strdup(env);

	confenv = libfonts_getenv__("XDG_CONFIG_HOME", ctx);
	if (confenv && *confenv) {
		ret = get(confenv, "/libfonts/default-fonts.conf", var, ctx);
		if (ret)
			goto out;
	}

	homeenv = libfonts_getenv__("HOME", ctx);
	if (homeenv && *homeenv) {
		ret = get(homeenv, "/.config/libfonts/default-fonts.conf", var, ctx);
		if (ret)
			goto out;
		ret = get(homeenv, "/.libfonts/default-fonts.conf", var, ctx);
		if (ret)
			goto out;
	}

	home = libfonts_gethome__(ctx);
	if (home && *home) {
		ret = get(home, "/.config/libfonts/default-fonts.conf", var, ctx);
		if (ret)
			goto out;
		ret = get(home, "/.libfonts/default-fonts.conf", var, ctx);
		if (ret)
			goto out;
	}

	if (confenv && *confenv) {
		ret = find(env, "/libfonts/", var, ctx);
		if (ret)
			goto out;
	}

	if (homeenv && *homeenv) {
		ret = find(homeenv, "/.config/libfonts/", var, ctx);
		if (ret)
			goto out;
		ret = find(homeenv, "/.libfonts/", var, ctx);
		if (ret)
			goto out;
	}

	if (home && *home) {
		ret = find(home, "/.config/libfonts/", var, ctx);
		if (ret)
			goto out;
		ret = find(home, "/.libfonts/", var, ctx);
		if (ret)
			goto out;
	}

	env = libfonts_getenv__("XDG_CONFIG_DIRS", ctx);
	if (env && *env) {
		do {
			next = strchr(&env[1], ':');
			len = next ? (size_t)(next - env) : strlen(env);
			if (len) {
				ret = getn(env, len, "/libfonts/default-fonts.conf", var, ctx);
				if (ret)
					goto out;
			}
			env += len + 1;
		} while (next);
	}

	ret = get("/etc", "/libfonts/default-fonts.conf", var, ctx);
	if (ret)
		goto out;

	ret = find("/etc", "/libfonts/", var, ctx);
	if (ret)
		goto out;

	/* TODO 
	 * as a last resort look around for some
	 * font that matches the specification as
	 * well as possible
	 */

	ret = NULL;

out:
	free(home);
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
