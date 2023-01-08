/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


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
			return unrotated ^ 3;
		else
			return unrotated ^ 5;

	case TRANSPOSED:
		if (unrotated <= LIBFONTS_SUBPIXEL_ORDER_G_R_B)
			return unrotated ^ 1;
		else
			return 7 - (unrotated & 7) + (unrotated & ~7);

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


#else


static int
test(int xtrans, int ytrans, int zscale, int xscale, int yscale)
{
#define T(IN, OUT, MAT)\
	T_(LIBFONTS_SUBPIXEL_ORDER_##IN, LIBFONTS_SUBPIXEL_ORDER_##OUT, &MAT)

#define T_(IN, OUT, MAT)\
	ASSERT(libfonts_calculate_subpixel_order(IN, MAT) == OUT)

#define ASIS_(IN, OUT)          T(IN, OUT, asis_matrix)
#define ROT90CW_(IN, OUT)       T(IN, OUT, rot90cw_matrix)
#define ROT180CW_(IN, OUT)      T(IN, OUT, rot180cw_matrix)
#define ROT270CW_(IN, OUT)      T(IN, OUT, rot270cw_matrix)
#define FLIP_(IN, OUT)          T(IN, OUT, flip_matrix)
#define FLOP_(IN, OUT)          T(IN, OUT, flop_matrix)
#define TRANSPOSE_(IN, OUT)     T(IN, OUT, transpose_matrix)
#define ANTITRANSPOSE_(IN, OUT) T(IN, OUT, antitranspose_matrix)
#define NONLINEAR_(IN, OUT)     T(IN, OUT, nonlinear_matrix)
#define UNKNOWN_(IN, OUT)       T(IN, OUT, unknown_matrix)

#define ROT(R0, R270, R180, R90)\
	do {\
		ASIS_(R0, R0);\
		ASIS_(R90, R90);\
		ASIS_(R180, R180);\
		ASIS_(R270, R270);\
		ROT90CW_(R0, R90);\
		ROT90CW_(R90, R180);\
		ROT90CW_(R180, R270);\
		ROT90CW_(R270, R0);\
		ROT180CW_(R0, R180);\
		ROT180CW_(R90, R270);\
		ROT180CW_(R180, R0);\
		ROT180CW_(R270, R90);\
		ROT270CW_(R0, R270);\
		ROT270CW_(R90, R0);\
		ROT270CW_(R180, R90);\
		ROT270CW_(R270, R180);\
	} while (0)
#define FLIP(A, B)\
	do {\
		FLIP_(A, B);\
		FLIP_(B, A);\
	} while(0)
#define FLOP(A, B)\
	do {\
		FLOP_(A, B);\
		FLOP_(B, A);\
	} while(0)
#define TRANSPOSE(A, B)\
	do {\
		TRANSPOSE_(A, B);\
		TRANSPOSE_(B, A);\
	} while(0)
#define ANTITRANSPOSE(A, B)\
	do {\
		ANTITRANSPOSE_(A, B);\
		ANTITRANSPOSE_(B, A);\
	} while(0)
#define NONLINEAR(IN) NONLINEAR_(IN, NONLINEAR)
#define UNKNOWN(IN) UNKNOWN_(IN, UNKNOWN)

#define ROTX(R0, R270, R180, R90)\
	do {\
		ROT(R0, R270, R180, R90);\
		ROT(BALANCED_##R0, BALANCED_##R270, BALANCED_##R180, BALANCED_##R90);\
	} while (0)
#define X(M, A, B)\
	do {\
		M(A, B);\
		M(BALANCED_##A, BALANCED_##B);\
	} while(0)
#define NONLINEARX(A)\
	do {\
		NONLINEAR(A);\
		NONLINEAR(BALANCED_##A);\
	} while(0)
#define UNKNOWNX(A)\
	do {\
		UNKNOWN(A);\
		UNKNOWN(BALANCED_##A);\
	} while(0)

	struct libfonts_transformation asis_matrix = {.m = {
			{+xscale,       0, xtrans},
			{      0, +yscale, ytrans},
			{      0,       0, zscale}}};
	struct libfonts_transformation rot90cw_matrix = {.m = {
			{      0, -yscale, xtrans},
			{+xscale,       0, ytrans},
			{      0,       0, zscale}}};
	struct libfonts_transformation rot180cw_matrix = {.m = {
			{-xscale,       0, xtrans},
			{      0, -yscale, ytrans},
			{      0,       0, zscale}}};
	struct libfonts_transformation rot270cw_matrix = {.m = {
			{      0, +yscale, xtrans},
			{-xscale,       0, ytrans},
			{      0,       0, zscale}}};
	struct libfonts_transformation flip_matrix = {.m = {
			{+xscale,       0, xtrans},
			{      0, -yscale, ytrans},
			{      0,       0, zscale}}};
	struct libfonts_transformation flop_matrix = {.m = {
			{-xscale,       0, xtrans},
			{      0, +yscale, ytrans},
			{      0,       0, zscale}}};
	struct libfonts_transformation transpose_matrix = {.m = {
			{      0, +yscale, xtrans},
			{+xscale,       0, ytrans},
			{      0,       0, zscale}}};
	struct libfonts_transformation antitranspose_matrix = {.m = {
			{      0, -yscale, xtrans},
			{-xscale,       0, ytrans},
			{      0,       0, zscale}}};
	struct libfonts_transformation nonlinear_matrix = {.m = {
			{+xscale, +yscale / 2., xtrans},
			{      0, +yscale,      ytrans},
			{      0,       0,      zscale}}};
	struct libfonts_transformation unknown_matrix = {.m = {
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0}}};

#define UNSUPPORTED(A)\
	do {\
		ASIS_(A, A);\
		ROT90CW_(A, A);\
		ROT180CW_(A, A);\
		ROT270CW_(A, A);\
		FLIP_(A, A);\
		FLOP_(A, A);\
		TRANSPOSE_(A, A);\
		ANTITRANSPOSE_(A, A);\
		NONLINEAR_(A, A);\
		UNKNOWN_(A, A);\
	} while (0)

	UNSUPPORTED(UNKNOWN);
	UNSUPPORTED(NONRGB);
	UNSUPPORTED(NONLINEAR);
	UNSUPPORTED(OTHER);

	ROT(RGB, R_G_B, BGR, B_G_R);
	ROT(GBR, G_B_R, RBG, R_B_G);
	ROT(BRG, B_R_G, GRB, G_R_B);
	ROTX(RR_GB, GR_BR, BG_RR, RB_RG);
	ROTX(RR_BG, BR_GR, GB_RR, RG_RB);
	ROTX(GG_RB, RG_BG, BR_GG, GB_GR);
	ROTX(GG_BR, BG_RG, RB_GG, GR_GB);
	ROTX(BB_RG, RB_GB, GR_BB, BG_BR);
	ROTX(BB_GR, GB_RB, RG_BB, BR_BG);

	FLIP(RGB, RGB);
	FLIP(R_G_B, B_G_R);
	FLIP(BGR, BGR);
	FLIP(GBR, GBR);
	FLIP(G_B_R, R_B_G);
	FLIP(RBG, RBG);
	FLIP(BRG, BRG);
	FLIP(B_R_G, G_R_B);
	FLIP(GRB, GRB);
	X(FLIP, RR_GB, GB_RR);
	X(FLIP, GR_BR, BR_GR);
	X(FLIP, BG_RR, RR_BG);
	X(FLIP, RB_RG, RG_RB);
	X(FLIP, GG_RB, RB_GG);
	X(FLIP, RG_BG, BG_RG);
	X(FLIP, BR_GG, GG_BR);
	X(FLIP, GB_GR, GR_GB);
	X(FLIP, BB_RG, RG_BB);
	X(FLIP, RB_GB, GB_RB);
	X(FLIP, GR_BB, BB_GR);
	X(FLIP, BG_BR, BR_BG);

	FLOP(RGB, BGR);
	FLOP(R_G_B, R_G_B);
	FLOP(B_G_R, B_G_R);
	FLOP(GBR, RBG);
	FLOP(G_B_R, G_B_R);
	FLOP(R_B_G, R_B_G);
	FLOP(BRG, GRB);
	FLOP(B_R_G, B_R_G);
	FLOP(G_R_B, G_R_B);
	X(FLOP, RR_GB, RR_BG);
	X(FLOP, GR_BR, RG_RB);
	X(FLOP, BG_RR, GB_RR);
	X(FLOP, RB_RG, BR_GR);
	X(FLOP, GG_RB, GG_BR);
	X(FLOP, RG_BG, GR_GB);
	X(FLOP, BR_GG, RB_GG);
	X(FLOP, GB_GR, BG_RG);
	X(FLOP, BB_RG, BB_GR);
	X(FLOP, RB_GB, BR_BG);
	X(FLOP, GR_BB, RG_BB);
	X(FLOP, BG_BR, GB_RB);

	TRANSPOSE(RGB, R_G_B);
	TRANSPOSE(BGR, B_G_R);
	TRANSPOSE(GBR, G_B_R);
	TRANSPOSE(RBG, R_B_G);
	TRANSPOSE(BRG, B_R_G);
	TRANSPOSE(GRB, G_R_B);
	X(TRANSPOSE, RR_GB, RG_RB);
	X(TRANSPOSE, GR_BR, GB_RR);
	X(TRANSPOSE, BG_RR, BR_GR);
	X(TRANSPOSE, RB_RG, RR_BG);
	X(TRANSPOSE, GG_RB, GR_GB);
	X(TRANSPOSE, RG_BG, RB_GG);
	X(TRANSPOSE, BR_GG, BG_RG);
	X(TRANSPOSE, GB_GR, GG_BR);
	X(TRANSPOSE, BB_RG, BR_BG);
	X(TRANSPOSE, RB_GB, RG_BB);
	X(TRANSPOSE, GR_BB, GB_RB);
	X(TRANSPOSE, BG_BR, BB_GR);

	ANTITRANSPOSE(RGB, B_G_R);
	ANTITRANSPOSE(BGR, R_G_B);
	ANTITRANSPOSE(GBR, R_B_G);
	ANTITRANSPOSE(RBG, G_B_R);
	ANTITRANSPOSE(BRG, G_R_B);
	ANTITRANSPOSE(GRB, B_R_G);
	X(ANTITRANSPOSE, RR_GB, BR_GR);
	X(ANTITRANSPOSE, GR_BR, RR_BG);
	X(ANTITRANSPOSE, BG_RR, RG_RB);
	X(ANTITRANSPOSE, RB_RG, GB_RR);
	X(ANTITRANSPOSE, GG_RB, BG_RG);
	X(ANTITRANSPOSE, RG_BG, GG_BR);
	X(ANTITRANSPOSE, BR_GG, GR_GB);
	X(ANTITRANSPOSE, GB_GR, RB_GG);
	X(ANTITRANSPOSE, BB_RG, GB_RB);
	X(ANTITRANSPOSE, RB_GB, BB_GR);
	X(ANTITRANSPOSE, GR_BB, BR_BG);
	X(ANTITRANSPOSE, BG_BR, RG_BB);

	NONLINEAR(RGB);
	NONLINEAR(R_G_B);
	NONLINEAR(BGR);
	NONLINEAR(B_G_R);
	NONLINEAR(GBR);
	NONLINEAR(G_B_R);
	NONLINEAR(RBG);
	NONLINEAR(R_B_G);
	NONLINEAR(BRG);
	NONLINEAR(B_R_G);
	NONLINEAR(GRB);
	NONLINEAR(G_R_B);
	NONLINEARX(RR_GB);
	NONLINEARX(GR_BR);
	NONLINEARX(BG_RR);
	NONLINEARX(RB_RG);
	NONLINEARX(RR_BG);
	NONLINEARX(BR_GR);
	NONLINEARX(GB_RR);
	NONLINEARX(RG_RB);
	NONLINEARX(GG_RB);
	NONLINEARX(RG_BG);
	NONLINEARX(BR_GG);
	NONLINEARX(GB_GR);
	NONLINEARX(GG_BR);
	NONLINEARX(BG_RG);
	NONLINEARX(RB_GG);
	NONLINEARX(GR_GB);
	NONLINEARX(BB_RG);
	NONLINEARX(RB_GB);
	NONLINEARX(GR_BB);
	NONLINEARX(BG_BR);
	NONLINEARX(BB_GR);
	NONLINEARX(GB_RB);
	NONLINEARX(RG_BB);
	NONLINEARX(BR_BG);

	UNKNOWN(RGB);
	UNKNOWN(R_G_B);
	UNKNOWN(BGR);
	UNKNOWN(B_G_R);
	UNKNOWN(GBR);
	UNKNOWN(G_B_R);
	UNKNOWN(RBG);
	UNKNOWN(R_B_G);
	UNKNOWN(BRG);
	UNKNOWN(B_R_G);
	UNKNOWN(GRB);
	UNKNOWN(G_R_B);
	UNKNOWNX(RR_GB);
	UNKNOWNX(GR_BR);
	UNKNOWNX(BG_RR);
	UNKNOWNX(RB_RG);
	UNKNOWNX(RR_BG);
	UNKNOWNX(BR_GR);
	UNKNOWNX(GB_RR);
	UNKNOWNX(RG_RB);
	UNKNOWNX(GG_RB);
	UNKNOWNX(RG_BG);
	UNKNOWNX(BR_GG);
	UNKNOWNX(GB_GR);
	UNKNOWNX(GG_BR);
	UNKNOWNX(BG_RG);
	UNKNOWNX(RB_GG);
	UNKNOWNX(GR_GB);
	UNKNOWNX(BB_RG);
	UNKNOWNX(RB_GB);
	UNKNOWNX(GR_BB);
	UNKNOWNX(BG_BR);
	UNKNOWNX(BB_GR);
	UNKNOWNX(GB_RB);
	UNKNOWNX(RG_BB);
	UNKNOWNX(BR_BG);

	return 0;
}

int
main(void)
{
	int xtrans, ytrans;

	for (xtrans = -1; xtrans <= +1; xtrans++) {
		for (ytrans = -1; ytrans <= +1; ytrans++) {
			test(xtrans, ytrans, 1, 1, 1);
			test(xtrans, ytrans, 1, 2, 1);
			test(xtrans, ytrans, 1, 1, 2);
			test(xtrans, ytrans, 1, 2, 2);
			test(xtrans, ytrans, 2, 1, 1);
			test(xtrans, ytrans, 2, 2, 2);
		}
	}

	return 0;
}


#endif
