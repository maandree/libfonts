/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


/* alternative to getline(3) that does not have a line length limit */

ssize_t
libfonts_getline__(int fd, char **linep, char **bufp, size_t *sizep, size_t *offp, size_t *availp)
{
	size_t len, size;
	ssize_t r;
	char *new;

	for (;;) {
		for (len = 0; *offp + len < *availp; len++) {
			if ((*bufp)[*offp + len] == '\n') {
				*linep = &(*bufp)[*offp];
				*offp += len += 1;
				return (ssize_t)len;
			}
		}

		if (*offp) {
			memmove(&(*bufp)[0], &(*bufp)[*offp], *availp -= *offp);
			*offp = 0;
		}

		if (*availp == *sizep) {
			if (*sizep > SIZE_MAX - 128)
				goto enomem;
			size = *sizep + 128;
			new = realloc(*bufp, size);
			if (!new) {
			enomem:
				errno = ENOMEM;
				return -1;
			}
			*bufp = new;
			*sizep = size;
		}
		r = read(fd, &(*bufp)[*availp], *sizep - *availp);
		if (r <= 0) {
			if (r < 0)
				return -1;
			*linep = *bufp;
			*offp = *availp;
			return (ssize_t)*availp;
		}
		*availp += (size_t)r;
	}
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
