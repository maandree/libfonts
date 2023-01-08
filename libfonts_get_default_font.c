/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static int
find(char **outp, const char *dir_part1, const char *dir_part2, const char *dir_part3, struct libfonts_context *ctx)
{
	return 0; /* TODO */
}

static int
getn(char **outp, const char *file_part1, size_t file_part1_len, const char *file_part2,
     const char *name, struct libfonts_context *ctx)
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

	/* TODO
	 *     sans-serif = $FONTNAME
	 *     serif = $FONTNAME
	 *     monospace = $FONTNAME
	 */

	free(path);
	return 0;
}

static int
get(char **outp, const char *file_part1, const char *file_part2, const char *name, struct libfonts_context *ctx)
{
	return getn(outp, file_part1, strlen(file_part1), file_part2, name, ctx);
}

char *
libfonts_get_default_font(enum libfonts_default_font font, struct libfonts_context *ctx)
{
	const char *env, *var, *next, *confenv, *homeenv;
	char *ret, *home = NULL;
	int r, saved_errno = errno;
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
		r = get(&ret, confenv, "/libfonts/default-fonts.conf", var, ctx);
		if (r)
			goto out;
	}

	homeenv = libfonts_getenv__("HOME", ctx);
	if (homeenv && *homeenv) {
		r = get(&ret, homeenv, "/.config/libfonts/default-fonts.conf", var, ctx);
		if (r)
			goto out;
		r = get(&ret, homeenv, "/.libfonts/default-fonts.conf", var, ctx);
		if (r)
			goto out;
	}

	home = libfonts_gethome__(ctx);
	if (home && *home) {
		r = get(&ret, home, "/.config/libfonts/default-fonts.conf", var, ctx);
		if (r)
			goto out;
		r = get(&ret, home, "/.libfonts/default-fonts.conf", var, ctx);
		if (r)
			goto out;
	}

	if (confenv && *confenv) {
		r = find(&ret, env, "/libfonts/", var, ctx);
		if (r)
			goto out;
	}

	if (homeenv && *homeenv) {
		r = find(&ret, homeenv, "/.config/libfonts/", var, ctx);
		if (r)
			goto out;
		r = find(&ret, homeenv, "/.libfonts/", var, ctx);
		if (r)
			goto out;
	}

	if (home && *home) {
		r = find(&ret, home, "/.config/libfonts/", var, ctx);
		if (r)
			goto out;
		r = find(&ret, home, "/.libfonts/", var, ctx);
		if (r)
			goto out;
	}

	env = libfonts_getenv__("XDG_CONFIG_DIRS", ctx);
	if (env && *env) {
		do {
			next = strchr(&env[1], ':');
			len = next ? (size_t)(next - env) : strlen(env);
			if (len) {
				r = getn(&ret, env, len, "/libfonts/default-fonts.conf", var, ctx);
				if (r)
					goto out;
			}
			env += len + 1;
		} while (next);
	}

	r = get(&ret, "/etc", "/libfonts/default-fonts.conf", var, ctx);
	if (r)
		goto out;

	r = find(&ret, "/etc", "/libfonts/", var, ctx);
	if (r)
		goto out;

	/* TODO 
	 * as a last resort look around for some
	 * font that matches the specification as
	 * well as possible
	 */

	ret = NULL;
	r = 0;

out:
	free(home);
	if (r < 0)
		return NULL;
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
