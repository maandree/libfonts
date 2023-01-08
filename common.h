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

#define LIST_RENDERING_SETTINGS(X, _)\
	X("dpi-x", dpi_x, 96) _\
	X("dpi-y", dpi_y, 96) _\
	X("ref-width", reference_width, 0) _\
	X("ref-height", reference_height, 0) _\
	X("subpixel-order", subpixel_order, LIBFONTS_SUBPIXEL_ORDER_UNKNOWN) _\
	X("greyscale-min", min_dpsqi_for_greyscale, 0) _\
	X("subpixel-min", min_dpsqi_for_subpixel, 0) _\
	X("h-grey-text-aa", horizontal_grey_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE) _\
	X("v-grey-text-aa", vertical_grey_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE) _\
	X("d-grey-text-aa", diagonal_grey_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE) _\
	X("h-coloured-text-aa", horizontal_colour_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE) _\
	X("v-coloured-text-aa", vertical_colour_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE) _\
	X("d-coloured-text-aa", diagonal_colour_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE)


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
