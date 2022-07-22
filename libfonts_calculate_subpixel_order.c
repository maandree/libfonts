/* See LICENSE file for copyright and license details. */
#include "libfonts.h"


#define TOLERANCE 0.000001


static void
transform(double *x_out, double *y_out, double x, double y, const struct libfonts_transformation *transformation)
{
	double a = transformation->m[0][0], b = transformation->m[0][1], c = transformation->m[0][2];
	double d = transformation->m[1][0], e = transformation->m[1][1], f = transformation->m[1][2];
	double g = transformation->m[2][0], h = transformation->m[2][1], i = transformation->m[2][2];
	double w = g*x + h*y + i;
	*x_out = (a*x + b*y + c) / w;
	*y_out = (d*x + e*y + f) / w;
}


static int
eq(double a, double b)
{
	return b - TOLERANCE <= a && a <= b + TOLERANCE;
}


enum libfonts_subpixel_order
libfonts_calculate_subpixel_order(enum libfonts_subpixel_order unrotated, const struct libfonts_transformation *transformation)
{
#define ASIS 0
#define ROTATED_90_DEG_CLOCKWISE 1
#define ROTATED_180_DEG_CLOCKWISE 2
#define ROTATED_270_DEG_CLOCKWISE 3
#define FLIPPED -1
#define FLOPPED -2
#define TRANSPOSED -3
#define ANTITRANSPOSED -4
#define OTHER -5

	double x[4], y[4], xmin, ymin, xmax, ymax, t1, t2;
	int trans, i, j;

	switch (unrotated) {
	case LIBFONTS_SUBPIXEL_ORDER_UNKNOWN:
	case LIBFONTS_SUBPIXEL_ORDER_NONRGB:
	case LIBFONTS_SUBPIXEL_ORDER_NONLINEAR:
	case LIBFONTS_SUBPIXEL_ORDER_OTHER:
		return unrotated;
	default:
		break;
	}

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			if (!eq(transformation->m[i][j], 0))
				goto known;
	return LIBFONTS_SUBPIXEL_ORDER_UNKNOWN;
known:

	transform(&x[0], &y[0], 0, 0, transformation);
	transform(&x[1], &y[1], 1, 0, transformation);
	transform(&x[2], &y[2], 0, 1, transformation);
	transform(&x[3], &y[3], 1, 1, transformation);

	t1 = x[0] < x[1] ? x[0] : x[1];
	t2 = x[2] < x[3] ? x[2] : x[3];
	xmin = t1 < t2 ? t1 : t2;

	t1 = y[0] < y[1] ? y[0] : y[1];
	t2 = y[2] < y[3] ? y[2] : y[3];
	ymin = t1 < t2 ? t1 : t2;

	for (i = 0; i < 4; i++) {
		x[i] -= xmin;
		y[i] -= ymin;
	}

	t1 = x[0] > x[1] ? x[0] : x[1];
	t2 = x[2] > x[3] ? x[2] : x[3];
	xmax = t1 > t2 ? t1 : t2;

	t1 = y[0] > y[1] ? y[0] : y[1];
	t2 = y[2] > y[3] ? y[2] : y[3];
	ymax = t1 > t2 ? t1 : t2;

	for (i = 0; i < 4; i++) {
		x[i] /= xmax;
		y[i] /= ymax;
	}

	/* Here we select the inverse transformation from what `transformation`
	 * applies, as `transformation` applies to the image on the output and
	 * we want to know how the output is physically configured, which is
	 * the inverse */
	trans = OTHER;
	if (eq(x[0], 0) && eq(y[0], 0)  &&  eq(x[3], 1) && eq(y[3], 1)) {
		if (eq(x[1], 1) && eq(y[1], 0)  &&  eq(x[2], 0) && eq(y[2], 1))
			trans = ASIS;
		else if (eq(x[2], 1) && eq(y[2], 0)  &&  eq(x[1], 0) && eq(y[1], 1))
			trans = TRANSPOSED;
	} else if (eq(x[1], 0) && eq(y[1], 0)  &&  eq(x[2], 1) && eq(y[2], 1)) {
		if (eq(x[0], 1) && eq(y[0], 0)  &&  eq(x[3], 0) && eq(y[3], 1))
			trans = FLOPPED;
		else if (eq(x[3], 1) && eq(y[3], 0)  &&  eq(x[0], 0) && eq(y[0], 1))
			trans = ROTATED_90_DEG_CLOCKWISE;
	} else if (eq(x[2], 0) && eq(y[2], 0)  &&  eq(x[1], 1) && eq(y[1], 1)) {
		if (eq(x[0], 1) && eq(y[0], 0)  &&  eq(x[3], 0) && eq(y[3], 1))
			trans = ROTATED_270_DEG_CLOCKWISE;
		else if (eq(x[3], 1) && eq(y[3], 0)  &&  eq(x[0], 0) && eq(y[0], 1))
			trans = FLIPPED;
	} else if (eq(x[3], 0) && eq(y[3], 0)  &&  eq(x[0], 1) && eq(y[0], 1)) {
		if (eq(x[1], 1) && eq(y[1], 0)  &&  eq(x[2], 0) && eq(y[2], 1))
			trans = ANTITRANSPOSED;
		else if (eq(x[2], 1) && eq(y[2], 0)  &&  eq(x[1], 0) && eq(y[1], 1))
			trans = ROTATED_180_DEG_CLOCKWISE;
	}

	switch (trans) {
	case OTHER:
		return LIBFONTS_SUBPIXEL_ORDER_NONLINEAR;

	case ANTITRANSPOSED:
		if (unrotated <= LIBFONTS_SUBPIXEL_ORDER_G_R_B)
			return unrotated ^ 1;
		else
			return unrotated ^ 5;

	case TRANSPOSED:
		if (unrotated <= LIBFONTS_SUBPIXEL_ORDER_G_R_B)
			return unrotated ^ 1;
		else
			return 8 - (unrotated & 7) + (unrotated & ~7);

	case FLOPPED:
		if (unrotated <= LIBFONTS_SUBPIXEL_ORDER_G_R_B)
			return unrotated ^ ((~unrotated << 1) & 2);
		else
			return unrotated ^ 4 ^ ((unrotated << 1) & 2);

	case FLIPPED:
		if (unrotated <= LIBFONTS_SUBPIXEL_ORDER_G_R_B)
			return unrotated ^ ((unrotated << 1) & 2);
		else
			return unrotated ^ 4 ^ ((~unrotated << 1) & 2);

	case ASIS:
		return unrotated;

	default:
		return ((unrotated + trans) & 3) + (unrotated & ~3);
	}
}
