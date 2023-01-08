/* See LICENSE file for copyright and license details. */
#include "libfonts.h"
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define DOUBLE_TOLERANCE 0.000001


static inline void
transform(double *x_out, double *y_out, double x, double y, const struct libfonts_transformation *transformation)
{
	double a = transformation->m[0][0], b = transformation->m[0][1], c = transformation->m[0][2];
	double d = transformation->m[1][0], e = transformation->m[1][1], f = transformation->m[1][2];
	double g = transformation->m[2][0], h = transformation->m[2][1], i = transformation->m[2][2];
	double w = g*x + h*y + i;
	*x_out = (a*x + b*y + c) / w;
	*y_out = (d*x + e*y + f) / w;
}


static inline int
eq(double a, double b)
{
	return b - DOUBLE_TOLERANCE <= a && a <= b + DOUBLE_TOLERANCE;
}


const char *libfonts_getenv__(const char *name, struct libfonts_context *ctx);
char *libfonts_gethome__(struct libfonts_context *ctx);


#ifdef TEST

# define ASSERT(ASSERTION)\
	do {\
		if (!(ASSERTION)) {\
			fprintf(stderr, "Failed assertion at line %u: %s\n", __LINE__, #ASSERTION);\
			exit(1);\
		}\
	} while (0)

#endif
