/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


char *
libfonts_gethome__(struct libfonts_context *ctx)
{
	uid_t uid = (ctx && ctx->use_context_uid) ? ctx->uid : getuid();
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


#else


int
main(void)
{
	return 0; /* TODO add test */
}


#endif
