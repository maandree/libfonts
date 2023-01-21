/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_get_subpixel_expansion(enum libfonts_subpixel_order_class layout, size_t *restrict widthmulp, size_t *restrict heightmulp)
{
	size_t w, h;

	if (layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_123) {
		w = 3;
		h = 1;
	} else if (layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_1_2_3) {
		w = 1;
		h = 3;
	} else if (layout >= LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23 &&
		   layout <= LIBFONTS_SUBPIXEL_ORDER_CLASS_13_12) {
		w = 2;
		h = 2;
	} else if (layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_11_23 ||
		   layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_32_11) {
		w = 2;
		h = 3;
	} else if (layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_21_31 ||
		   layout == LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_13_12) {
		w = 3;
		h = 2;
	} else {
		errno = EINVAL;
		return -1;
	}

	if (widthmulp)
		*widthmulp = w;
	if (heightmulp)
		*heightmulp = h;
	return 0;
}


#else


int
main(void)
{
	return 0;
}


#endif
