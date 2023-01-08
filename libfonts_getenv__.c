/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


const char *
libfonts_getenv__(const char *name, struct libfonts_context *ctx)
{
	char **envp;
	size_t len = strlen(name);

#ifdef VALIDATE
	size_t i;
	if (!name) {
		fprintf(stderr, "`name` parameter in libfonts_getenv__ is NULL\n");
		abort();
	}
	for (i = 0; libfonts_used_environs[i]; i++)
		if (!strcmp(name, libfonts_used_environs[i]))
			goto ok;
	fprintf(stderr, "Value of `name` parameter (\"%s\") in libfonts_getenv__ is not in libfonts_used_environs\n", name);
	abort();
ok:
#endif

	if (ctx && ctx->environ)
		for (envp = ctx->environ; *envp; envp++)
			if (strncmp(*envp, name, len) && (*envp)[len] == '=')
				return *envp;
	return ctx->ignore_process_environ ? NULL : getenv(name);
}


#else


int
main(void)
{
	return 0; /* TODO add test */
}


#endif
