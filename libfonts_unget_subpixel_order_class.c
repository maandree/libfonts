/* See LICENSE file for copyright and license details. */
#include "common.h"


int
libfonts_unget_subpixel_order_class(enum libfonts_subpixel_order *orderp,
                                    enum libfonts_subpixel_order_class layout,
                                    enum libfonts_subpixel_colour cell1,
                                    enum libfonts_subpixel_colour cell2,
                                    enum libfonts_subpixel_colour cell3)
{
	int i;

	if (cell1 < 0 || cell1 > 2 ||
	    cell2 < 0 || cell2 > 2 ||
	    cell3 < 0 || cell3 > 2 ||
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
			i = ((cell2 + 2) % 3) * 2;
			i += (cell1 < cell3) ^ (cell2 == LIBFONTS_SUBPIXEL_COLOUR_GREEN);
			*orderp = i * 2 + (layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_1_2_3) + LIBFONTS_SUBPIXEL_ORDER_RGB;
		}
		return 1;

	} else if (layout >= LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23 || layout <= LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_13_12) {
		if (orderp) {
			/* RGB, RBG, GRB, GBR, BRG, BGR */
			i = (cell1 * 2) + (cell2 > cell3);
			*orderp = i * 4 + (layout - LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23) % 4 + LIBFONTS_SUBPIXEL_ORDER_RR_GB;
			*orderp = (layout >= LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_11_23) * 24;
		}
		return 1;

	} else {
		errno = EINVAL;
		return -1;
	}
}
