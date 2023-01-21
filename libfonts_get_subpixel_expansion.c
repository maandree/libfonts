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
	size_t w, h;

#define T(L)\
	do {\
		errno = 0;\
		ASSERT(libfonts_get_subpixel_expansion(L, NULL, NULL) == -1);\
		ASSERT(errno == EINVAL);\
		\
		errno = 0;\
		ASSERT(libfonts_get_subpixel_expansion(L, &w, NULL) == -1);\
		ASSERT(errno == EINVAL);\
		\
		errno = 0;\
		ASSERT(libfonts_get_subpixel_expansion(L, NULL, &h) == -1);\
		ASSERT(errno == EINVAL);\
		\
		errno = 0;\
		ASSERT(libfonts_get_subpixel_expansion(L, &w, &h) == -1);\
		ASSERT(errno == EINVAL);\
	} while (0)

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_OTHER);
	T((enum libfonts_subpixel_order_class)999);

#undef T

#define T(L, W, H)\
	do {\
		w = 0, h = 0;\
		ASSERT(!libfonts_get_subpixel_expansion(L, NULL, NULL));\
		\
		w = 0, h = 0;\
		ASSERT(!libfonts_get_subpixel_expansion(L, &w, NULL));\
		ASSERT(w == (W));\
		\
		w = 0, h = 0;\
		ASSERT(!libfonts_get_subpixel_expansion(L, NULL, &h));\
		ASSERT(h == (H));\
		\
		w = 0, h = 0;\
		ASSERT(!libfonts_get_subpixel_expansion(L, &w, &h));\
		ASSERT(w == (W));\
		ASSERT(h == (H));\
	} while (0)

	errno = EDOM;
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_123, 3, 1);
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_1_2_3, 1, 3);
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23, 2, 2);
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_21_31, 2, 2);
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_32_11, 2, 2);
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_13_12, 2, 2);
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_11_23, 2, 3);
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_21_31, 3, 2);
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_32_11, 2, 3);
	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_13_12, 3, 2);
	ASSERT(errno == EDOM);

	return 0;
}


#endif
