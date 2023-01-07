/* See LICENSE file for copyright and license details. */
#include "common.h"


enum libfonts_subpixel_order_class
libfonts_get_subpixel_order_class(enum libfonts_subpixel_order order,
                                  enum libfonts_subpixel_colour *cell1p,
                                  enum libfonts_subpixel_colour *cell2p,
                                  enum libfonts_subpixel_colour *cell3p)
{
	enum libfonts_subpixel_colour c1, c2, c3;
	enum libfonts_subpixel_order_class layout;
	int i; /* RGB, RBG, GRB, GBR, BRG, BGR */

	if (order >= LIBFONTS_SUBPIXEL_ORDER_RGB && order <= LIBFONTS_SUBPIXEL_ORDER_G_R_B) {
		layout = ((order - LIBFONTS_SUBPIXEL_ORDER_RGB) & 1) + LIBFONTS_SUBPIXEL_ORDER_CLASS_123;
		i = (order - LIBFONTS_SUBPIXEL_ORDER_RGB) / 2;
		i = ((int []){0, 5, 3, 1, 4, 2})[i];

	} else if (order >= LIBFONTS_SUBPIXEL_ORDER_RR_GB && order <= LIBFONTS_SUBPIXEL_ORDER_BALANCED_BR_BG) {
		layout = ((order - LIBFONTS_SUBPIXEL_ORDER_RR_GB) & 3) + LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23;
		if (order >= LIBFONTS_SUBPIXEL_ORDER_BALANCED_RR_GB)
			layout += 4;
		i = (order - LIBFONTS_SUBPIXEL_ORDER_RR_GB) / 4 % 6;

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
