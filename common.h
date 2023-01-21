/* See LICENSE file for copyright and license details. */
#include "libfonts.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <pwd.h>
#include <stdarg.h>
#include <stddef.h>
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

#define LIST_ANTIALIASINGS(X, _)\
	X(LIBFONTS_ANTIALIASING_UNSPECIFIED, "unspecified") _\
	X(LIBFONTS_ANTIALIASING_NONE, "none") _\
	X(LIBFONTS_ANTIALIASING_NONE, "aliased") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "greyscale") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "grayscale") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "grey") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "gray") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE, "antialiased") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "greyscale!") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "grayscale!") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "grey!") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "gray!") _\
	X(LIBFONTS_ANTIALIASING_GREYSCALE_FORCED, "antialiased!") _\
	X(LIBFONTS_ANTIALIASING_SUBPIXEL, "subpixel") _\
	X(LIBFONTS_ANTIALIASING_SUBPIXEL, "glitter") _\
	X(LIBFONTS_ANTIALIASING_SUBPIXEL_FORCED, "subpixel!") _\
	X(LIBFONTS_ANTIALIASING_SUBPIXEL_FORCED, "glitter!")

#define LIST_SUBPIXEL_ORDERS(X, _)\
	X(LIBFONTS_SUBPIXEL_ORDER_UNKNOWN, "unknown") _\
	X(LIBFONTS_SUBPIXEL_ORDER_NONRGB, "non-rgb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_NONLINEAR, "non-linear") _\
	X(LIBFONTS_SUBPIXEL_ORDER_OTHER, "other") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RGB, "rgb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_R_G_B, "r:g:b") _\
	X(LIBFONTS_SUBPIXEL_ORDER_R_G_B, "vrgb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BGR, "bgr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_B_G_R, "b:g:r") _\
	X(LIBFONTS_SUBPIXEL_ORDER_B_G_R, "vbgr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GBR, "gbr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_G_B_R, "g:b:r") _\
	X(LIBFONTS_SUBPIXEL_ORDER_G_B_R, "vgbr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RBG, "rbg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_R_B_G, "r:b:g") _\
	X(LIBFONTS_SUBPIXEL_ORDER_R_B_G, "vrbg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BRG, "brg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_B_R_G, "b:r:g") _\
	X(LIBFONTS_SUBPIXEL_ORDER_B_R_G, "vbrg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GRB, "grb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_G_R_B, "g:r:b") _\
	X(LIBFONTS_SUBPIXEL_ORDER_G_R_B, "vgrb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RR_GB, "rr:gb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GR_BR, "gr:br") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BG_RR, "bg:rr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RB_RG, "rb:rg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RR_BG, "rr:bg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BR_GR, "br:gr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GB_RR, "gb:rr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RG_RB, "rg:rb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GG_RB, "gg:rb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RG_BG, "rg:bg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BR_GG, "br:gg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GB_GR, "gb:gr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GG_BR, "gg:br") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BG_RG, "bg:rg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RB_GG, "rb:gg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GR_GB, "gr:gb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BB_RG, "bb:rg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RB_GB, "rb:gb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GR_BB, "gr:bb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BG_BR, "bg:br") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BB_GR, "bb:gr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_GB_RB, "gb:rb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_RG_BB, "rg:bb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BR_BG, "br:bg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_RR_GB, "balanced rr:gb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_GR_BR, "balanced gr:br") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_BG_RR, "balanced bg:rr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_RB_RG, "balanced rb:rg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_RR_BG, "balanced rr:bg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_BR_GR, "balanced br:gr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_GB_RR, "balanced gb:rr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_RG_RB, "balanced rg:rb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_GG_RB, "balanced gg:rb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_RG_BG, "balanced rg:bg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_BR_GG, "balanced br:gg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_GB_GR, "balanced gb:gr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_GG_BR, "balanced gg:br") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_BG_RG, "balanced bg:rg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_RB_GG, "balanced rb:gg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_GR_GB, "balanced gr:gb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_BB_RG, "balanced bb:rg") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_RB_GB, "balanced rb:gb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_GR_BB, "balanced gr:bb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_BG_BR, "balanced bg:br") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_BB_GR, "balanced bb:gr") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_GB_RB, "balanced gb:rb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_RG_BB, "balanced rg:bb") _\
	X(LIBFONTS_SUBPIXEL_ORDER_BALANCED_BR_BG, "balanced br:bg")


static inline void
warning(struct libfonts_context *ctx, int err, const char *function, const char *fmt, ...)
{
	va_list args;
	if (ctx && ctx->warning) {
		va_start(args, fmt);
		ctx->warning(err, function, fmt, args, ctx->user);
		va_end(args);
	}
}


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


#if defined(__GNUC__) && !defined(__clang__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunsuffixed-float-constants"
#endif

static inline int
eq(double a, double b)
{
	return b - DOUBLE_TOLERANCE <= a && a <= b + DOUBLE_TOLERANCE;
}

#if defined(__GNUC__) && !defined(__clang__)
# pragma GCC diagnostic pop
#endif


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
			fprintf(stderr, "Failed assertion at line %i: %s\n", __LINE__, #ASSERTION);\
			exit(1);\
		}\
	} while (0)


#if defined(__clang__)
# pragma clang diagnostic ignored "-Wassign-enum"
#endif

#endif
