/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


enum libfonts_subpixel_order_class
libfonts_get_subpixel_order_class(enum libfonts_subpixel_order order,
                                  enum libfonts_subpixel_colour *cell1p,
                                  enum libfonts_subpixel_colour *cell2p,
                                  enum libfonts_subpixel_colour *cell3p)
{
	enum libfonts_subpixel_colour c1, c2, c3;
	enum libfonts_subpixel_order_class layout;
	unsigned int i; /* RGB, RBG, GRB, GBR, BRG, BGR */

	if (order >= LIBFONTS_SUBPIXEL_ORDER_RGB && order <= LIBFONTS_SUBPIXEL_ORDER_G_R_B) {
		layout = ((order - LIBFONTS_SUBPIXEL_ORDER_RGB) & 1) + LIBFONTS_SUBPIXEL_ORDER_CLASS_123;
		i = (unsigned int)(order - LIBFONTS_SUBPIXEL_ORDER_RGB) / 2;
		i = ((unsigned int []){0, 5, 3, 1, 4, 2})[i];

	} else if (order >= LIBFONTS_SUBPIXEL_ORDER_RR_GB && order <= LIBFONTS_SUBPIXEL_ORDER_BALANCED_BR_BG) {
		layout = ((order - LIBFONTS_SUBPIXEL_ORDER_RR_GB) & 3) + LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23;
		if (order >= LIBFONTS_SUBPIXEL_ORDER_BALANCED_RR_GB)
			layout += 4;
		i = (unsigned int)(order - LIBFONTS_SUBPIXEL_ORDER_RR_GB) / 4 % 6;

	} else {
		layout = LIBFONTS_SUBPIXEL_ORDER_CLASS_OTHER;
		i = 0;
	}

	c1 = i / 2;
	c2 = (i + 1 + i / 3) % 3;
	c3 = 3 - c1 - c2;
	if (cell1p)
		*cell1p = c1;
	if (cell2p)
		*cell2p = c2;
	if (cell3p)
		*cell3p = c3;
	return layout;
}


#else


int
main(void)
{
	enum libfonts_subpixel_colour c1, c2, c3;

#define R LIBFONTS_SUBPIXEL_COLOUR_RED
#define G LIBFONTS_SUBPIXEL_COLOUR_GREEN
#define B LIBFONTS_SUBPIXEL_COLOUR_BLUE

#define T(ORDER, LAYOUT, C1, C2, C3)\
	T_(LIBFONTS_SUBPIXEL_ORDER_##ORDER,\
	   LIBFONTS_SUBPIXEL_ORDER_CLASS_##LAYOUT,\
	   C1, C2, C3)
#define T_(ORDER, LAYOUT, C1, C2, C3)\
	do {\
		errno = 0;\
		c1 = c2 = c3 = (enum libfonts_subpixel_colour)~0;\
		ASSERT(libfonts_get_subpixel_order_class(ORDER, &c1, &c2, &c3) == LAYOUT);\
		ASSERT(c1 == C1);\
		ASSERT(c2 == C2);\
		ASSERT(c3 == C3);\
		ASSERT(!errno);\
		ASSERT(libfonts_get_subpixel_order_class(ORDER, NULL, &c2, &c3) == LAYOUT);\
		ASSERT(c2 == C2);\
		ASSERT(c3 == C3);\
		ASSERT(!errno);\
		ASSERT(libfonts_get_subpixel_order_class(ORDER, &c1, NULL, &c3) == LAYOUT);\
		ASSERT(c1 == C1);\
		ASSERT(c3 == C3);\
		ASSERT(!errno);\
		ASSERT(libfonts_get_subpixel_order_class(ORDER, &c1, &c2, NULL) == LAYOUT);\
		ASSERT(c1 == C1);\
		ASSERT(c2 == C2);\
		ASSERT(!errno);\
	} while (0)

	T(UNKNOWN, OTHER, R, G, B);
	T(NONRGB, OTHER, R, G, B);
	T(NONLINEAR, OTHER, R, G, B);
	T(OTHER, OTHER, R, G, B);
	T_((enum libfonts_subpixel_order)~1, LIBFONTS_SUBPIXEL_ORDER_CLASS_OTHER, R, G, B);
	T_(9999, LIBFONTS_SUBPIXEL_ORDER_CLASS_OTHER, R, G, B);

	T(RGB, 123, R, G, B);
	T(R_G_B, 1_2_3, R, G, B);
	T(BGR, 123, B, G, R);
	T(B_G_R, 1_2_3, B, G, R);

	T(GBR, 123, G, B, R);
	T(G_B_R, 1_2_3, G, B, R);
	T(RBG, 123, R, B, G);
	T(R_B_G, 1_2_3, R, B, G);

	T(BRG, 123, B, R, G);
	T(B_R_G, 1_2_3, B, R, G);
	T(GRB, 123, G, R, B);
	T(G_R_B, 1_2_3, G, R, B);

	T(RR_GB, 11_23, R, G, B);
	T(GR_BR, 21_31, R, G, B);
	T(BG_RR, 32_11, R, G, B);
	T(RB_RG, 13_12, R, G, B);

	T(RR_BG, 11_23, R, B, G);
	T(BR_GR, 21_31, R, B, G);
	T(GB_RR, 32_11, R, B, G);
	T(RG_RB, 13_12, R, B, G);

	T(GG_RB, 11_23, G, R, B);
	T(RG_BG, 21_31, G, R, B);
	T(BR_GG, 32_11, G, R, B);
	T(GB_GR, 13_12, G, R, B);

	T(GG_BR, 11_23, G, B, R);
	T(BG_RG, 21_31, G, B, R);
	T(RB_GG, 32_11, G, B, R);
	T(GR_GB, 13_12, G, B, R);

	T(BB_RG, 11_23, B, R, G);
	T(RB_GB, 21_31, B, R, G);
	T(GR_BB, 32_11, B, R, G);
	T(BG_BR, 13_12, B, R, G);

	T(BB_GR, 11_23, B, G, R);
	T(GB_RB, 21_31, B, G, R);
	T(RG_BB, 32_11, B, G, R);
	T(BR_BG, 13_12, B, G, R);

	T(BALANCED_RR_GB, BALANCED_11_23, R, G, B);
	T(BALANCED_GR_BR, BALANCED_21_31, R, G, B);
	T(BALANCED_BG_RR, BALANCED_32_11, R, G, B);
	T(BALANCED_RB_RG, BALANCED_13_12, R, G, B);

	T(BALANCED_RR_BG, BALANCED_11_23, R, B, G);
	T(BALANCED_BR_GR, BALANCED_21_31, R, B, G);
	T(BALANCED_GB_RR, BALANCED_32_11, R, B, G);
	T(BALANCED_RG_RB, BALANCED_13_12, R, B, G);

	T(BALANCED_GG_RB, BALANCED_11_23, G, R, B);
	T(BALANCED_RG_BG, BALANCED_21_31, G, R, B);
	T(BALANCED_BR_GG, BALANCED_32_11, G, R, B);
	T(BALANCED_GB_GR, BALANCED_13_12, G, R, B);

	T(BALANCED_GG_BR, BALANCED_11_23, G, B, R);
	T(BALANCED_BG_RG, BALANCED_21_31, G, B, R);
	T(BALANCED_RB_GG, BALANCED_32_11, G, B, R);
	T(BALANCED_GR_GB, BALANCED_13_12, G, B, R);

	T(BALANCED_BB_RG, BALANCED_11_23, B, R, G);
	T(BALANCED_RB_GB, BALANCED_21_31, B, R, G);
	T(BALANCED_GR_BB, BALANCED_32_11, B, R, G);
	T(BALANCED_BG_BR, BALANCED_13_12, B, R, G);

	T(BALANCED_BB_GR, BALANCED_11_23, B, G, R);
	T(BALANCED_GB_RB, BALANCED_21_31, B, G, R);
	T(BALANCED_RG_BB, BALANCED_32_11, B, G, R);
	T(BALANCED_BR_BG, BALANCED_13_12, B, G, R);

	return 0;
}


#endif
