/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static char *
find(const char *dir_part1, const char *dir_part2, const char *dir_part3)
{
	return NULL; /* TODO */
}

static char *
gethome(void)
{
	uid_t uid = getuid();
	size_t size = 512;
	char *buf = NULL, *new;
	char *ret = NULL;
	struct passwd pw, *pwp;
	int err;

	goto beginning;

	do {
		if (size > SIZE_MAX / 2)
			goto enomem;
		size *= 2;
	beginning:
		new = realloc(buf, size);
		if (!new) {
		enomem:
			free(buf);
			errno = ENOMEM;
			return NULL;
		}
		buf = new;
	} while ((err = getpwuid_r(uid, &pw, buf, size, &pwp)) == ERANGE);
	if (err) {
		free(buf);
		return NULL;
	}

	if (pwp && pwp->pw_dir)
		ret = strdup(pwp->pw_dir);
	free(buf);
	return ret;
}

static char *
getn(const char *file_part1, size_t file_part1_len, const char *file_part2, const char *name)
{
	size_t file_part2_len = strlen(file_part2);
	char *path;

	if (file_part1_len > SIZE_MAX - file_part2_len - 1)
		goto enomem;

	path = malloc(file_part1_len + file_part2_len + 1);
	if (!path) {
	enomem:
		errno = ENOMEM;
		return NULL;
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
get(const char *file_part1, const char *file_part2, const char *name)
{
	return getn(file_part1, strlen(file_part1), file_part2, name);
}

char *
libfonts_get_default_font(enum libfonts_default_font font)
{
	const char *env, *var, *next;
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

	ret = getenv(env);
	if (ret && *ret)
		return strdup(ret);

	env = getenv("XDG_CONFIG_HOME");
	if (env && *env) {
		ret = get(env, "/libfonts/default-fonts.conf", var);
		if (ret)
			goto out;
	}

	env = getenv("HOME");
	if (env && *env) {
		ret = get(env, "/.config/libfonts/default-fonts.conf", var);
		if (ret)
			goto out;
		ret = get(env, "/.libfonts/default-fonts.conf", var);
		if (ret)
			goto out;
	}

	home = gethome();
	if (home && *home) {
		ret = get(home, "/.config/libfonts/default-fonts.conf", var);
		if (ret)
			goto out;
		ret = get(home, "/.libfonts/default-fonts.conf", var);
		if (ret)
			goto out;
	}

	env = getenv("XDG_CONFIG_HOME");
	if (env && *env) {
		ret = find(env, "/libfonts/", var);
		if (ret)
			goto out;
	}

	env = getenv("HOME");
	if (env && *env) {
		ret = find(env, "/.config/libfonts/", var);
		if (ret)
			goto out;
		ret = find(env, "/.libfonts/", var);
		if (ret)
			goto out;
	}

	if (home && *home) {
		ret = find(env, "/.config/libfonts/", var);
		if (ret)
			goto out;
		ret = find(env, "/.libfonts/", var);
		if (ret)
			goto out;
	}

	env = getenv("XDG_CONFIG_DIRS");
	if (env && *env) {
		do {
			next = strchr(&env[1], ':');
			len = next ? (size_t)(next - env) : strlen(env);
			if (len) {
				ret = getn(env, len, "/libfonts/default-fonts.conf", var);
				if (ret)
					goto out;
			}
			env += len + 1;
		} while (next);
	}

	ret = get("/etc", "/libfonts/default-fonts.conf", var);
	if (ret)
		goto out;

	ret = find("/etc", "/libfonts/", var);
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
