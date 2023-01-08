/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_unget_subpixel_order_class(enum libfonts_subpixel_order *orderp,
                                    enum libfonts_subpixel_order_class layout,
                                    enum libfonts_subpixel_colour cell1,
                                    enum libfonts_subpixel_colour cell2,
                                    enum libfonts_subpixel_colour cell3)
{
	unsigned int i;

	if ((uintmax_t)cell1 > 2 ||
	    (uintmax_t)cell2 > 2 ||
	    (uintmax_t)cell3 > 2 ||
	    cell1 == cell2 ||
	    cell2 == cell3 ||
	    cell3 == cell1) {
		errno = EINVAL;
		return -1;
	}

	if (layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_OTHER) {
		if (orderp)
			*orderp = LIBFONTS_SUBPIXEL_ORDER_UNKNOWN;
		return 0;

	} else if (layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_123 || layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_1_2_3) {
		if (orderp) {
			/* RGB, BGR, GBR, RBG, BRG, GRB */
			i = (unsigned int)((cell2 + 2) % 3) * 2;
			i += (unsigned int)(cell1 < cell3) ^ (cell2 == LIBFONTS_SUBPIXEL_COLOUR_GREEN);
			*orderp = (enum libfonts_subpixel_order)(i * 2 +
			                                         (layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_1_2_3) +
			                                         LIBFONTS_SUBPIXEL_ORDER_RGB);
		}
		return 1;

	} else if (layout >= LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23 && layout <= LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_13_12) {
		if (orderp) {
			/* RGB, RBG, GRB, GBR, BRG, BGR */
			i = (cell1 * 2) + (cell2 > cell3);
			*orderp = i * 4 + (layout - LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23) % 4 + LIBFONTS_SUBPIXEL_ORDER_RR_GB;
			*orderp += (enum libfonts_subpixel_order)((layout >= LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_11_23) * 24);
		}
		return 1;

	} else {
		errno = EINVAL;
		return -1;
	}
}


#else


int
main(void)
{
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
		order = (enum libfonts_subpixel_order)~1;\
		ASSERT(libfonts_unget_subpixel_order_class(&order, LAYOUT, C1, C2, C3) == 1);\
		ASSERT(order == ORDER);\
		ASSERT(!errno);\
		ASSERT(libfonts_unget_subpixel_order_class(NULL, LAYOUT, C1, C2, C3) == 1);\
		ASSERT(!errno);\
	} while (0)

#define TU(LAYOUT, C1, C2, C3)\
	TU_(LIBFONTS_SUBPIXEL_ORDER_CLASS_##LAYOUT, C1, C2, C3)
#define TU_(LAYOUT, C1, C2, C3)\
	do {\
		errno = 0;\
		order = (enum libfonts_subpixel_order)~1;\
		ASSERT(libfonts_unget_subpixel_order_class(&order, LAYOUT, C1, C2, C3) == 0);\
		ASSERT(order == LIBFONTS_SUBPIXEL_ORDER_UNKNOWN);\
		ASSERT(!errno);\
		ASSERT(libfonts_unget_subpixel_order_class(NULL, LAYOUT, C1, C2, C3) == 0);\
		ASSERT(!errno);\
	} while (0)

#define TE(LAYOUT, C1, C2, C3)\
	TE_(LIBFONTS_SUBPIXEL_ORDER_CLASS_##LAYOUT, C1, C2, C3)
#define TE_(LAYOUT, C1, C2, C3)\
	do {\
		errno = 0;\
		ASSERT(libfonts_unget_subpixel_order_class(&order, LAYOUT, C1, C2, C3) == -1);\
		ASSERT(errno == EINVAL);\
		errno = 0;\
		ASSERT(libfonts_unget_subpixel_order_class(NULL, LAYOUT, C1, C2, C3) == -1);\
		ASSERT(errno == EINVAL);\
	} while (0)

	enum libfonts_subpixel_order order;

	TU(OTHER, R, G, B);
	TU(OTHER, G, B, R);
	TU(OTHER, B, R, G);
	TU(OTHER, R, B, G);
	TU(OTHER, G, R, B);
	TU(OTHER, B, G, R);

	TE(OTHER, (enum libfonts_subpixel_colour)~1, G, B);
	TE(OTHER, R, (enum libfonts_subpixel_colour)~1, B);
	TE(OTHER, R, G, (enum libfonts_subpixel_colour)~1);
	TE(OTHER, (enum libfonts_subpixel_colour)~1, (enum libfonts_subpixel_colour)~2, B);
	TE(OTHER, 3, G, B);
	TE(OTHER, R, 3, B);
	TE(OTHER, R, G, 3);
	TE(OTHER, R, 3, 4);
	TE(123, (enum libfonts_subpixel_colour)~1, G, B);
	TE(123, R, (enum libfonts_subpixel_colour)~1, B);
	TE(123, R, G, (enum libfonts_subpixel_colour)~1);
	TE(123, (enum libfonts_subpixel_colour)~1, (enum libfonts_subpixel_colour)~2, B);
	TE(123, 3, G, B);
	TE(123, R, 3, B);
	TE(123, R, G, 3);
	TE(123, R, 3, 4);
	TE(123, R, R, G);
	TE(123, R, G, R);
	TE(123, R, G, G);
	TE_((enum libfonts_subpixel_order_class)~1, R, G, B);
	TE_(9999, R, G, B);

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
