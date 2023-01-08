/* See LICENSE file for copyright and license details. */
#include "libfonts.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
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
	X(0, "dpi-x", dpi_x, 96, libfonts_parse_double__) _\
	X(1, "dpi-y", dpi_y, 96, libfonts_parse_double__) _\
	X(2, "ref-width", reference_width, 0, libfonts_parse_uint32__) _\
	X(3, "ref-height", reference_height, 0, libfonts_parse_uint32__) _\
	X(4, "subpixel-order", subpixel_order, LIBFONTS_SUBPIXEL_ORDER_UNKNOWN, libfonts_parse_order__) _\
	X(5, "greyscale-min", min_dpsqi_for_greyscale, 0, libfonts_parse_double__) _\
	X(6, "subpixel-min", min_dpsqi_for_subpixel, 0, libfonts_parse_double__) _\
	X(7, "h-grey-text-aa", horizontal_grey_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE, libfonts_parse_aa__) _\
	X(8, "v-grey-text-aa", vertical_grey_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE, libfonts_parse_aa__) _\
	X(9, "d-grey-text-aa", diagonal_grey_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE, libfonts_parse_aa__) _\
	X(10, "h-coloured-text-aa", horizontal_colour_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE, libfonts_parse_aa__) _\
	X(11, "v-coloured-text-aa", vertical_colour_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE, libfonts_parse_aa__) _\
	X(12, "d-coloured-text-aa", diagonal_colour_text_antialiasing, LIBFONTS_ANTIALIASING_GREYSCALE, libfonts_parse_aa__)


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
ssize_t libfonts_getline__(int fd, char **linep, char **bufp, size_t *sizep, size_t *offp, size_t *availp);
char *libfonts_confsplit__(char *line);

int libfonts_parse_double__(double *outp, const char *value);
int libfonts_parse_uint32__(uint32_t *outp, const char *value);
int libfonts_parse_order__(enum libfonts_subpixel_order *outp, const char *value);
int libfonts_parse_aa__(enum libfonts_antialiasing *outp, const char *value);


#ifdef TEST

# define ASSERT(ASSERTION)\
	do {\
		if (!(ASSERTION)) {\
			fprintf(stderr, "Failed assertion at line %u: %s\n", __LINE__, #ASSERTION);\
			exit(1);\
		}\
	} while (0)

#endif
