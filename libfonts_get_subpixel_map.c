/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_get_subpixel_map(enum libfonts_subpixel_order_class layout, size_t rowsize, uint8_t *restrict map,
                          uint8_t *restrict n0p, uint8_t *restrict n1p, uint8_t *restrict n2p)
{
	size_t n0, n1, n2;

#define SET_1_X_3(R1C1, R1C2, R1C3)\
	do {\
		if (map) {\
			map[0] = (R1C1);\
			map[1] = (R1C2);\
			map[2] = (R1C3);\
		}\
		n0 = 1;\
		n1 = 1;\
		n2 = 1;\
	} while (0)

#define SET_3_X_1(R1C1, R2C1, R3C1)\
	do {\
		if (map) {\
			if (rowsize < 1)\
				goto einval;\
			map[0 * rowsize] = (R1C1);\
			map[1 * rowsize] = (R2C1);\
			map[2 * rowsize] = (R3C1);\
		}\
		n0 = 1;\
		n1 = 1;\
		n2 = 1;\
	} while (0)

#define SET_2_X_2(R1C1, R1C2, R2C1, R2C2)\
	do {\
		if (map) {\
			if (rowsize < 2)\
				goto einval;\
			map[0 * rowsize + 0] = (R1C1);\
			map[0 * rowsize + 1] = (R1C2);\
			map[1 * rowsize + 0] = (R2C1);\
			map[1 * rowsize + 1] = (R2C2);\
		}\
		n0 = 2;\
		n1 = 1;\
		n2 = 1;\
	} while (0)

#define SET_3_X_2(R1C1, R1C2, R2C1, R2C2, R3C1, R3C2)\
	do {\
		if (map) {\
			if (rowsize < 2)\
				goto einval;\
			map[0 * rowsize + 0] = (R1C1);\
			map[0 * rowsize + 1] = (R1C2);\
			map[1 * rowsize + 0] = (R2C1);\
			map[1 * rowsize + 1] = (R2C2);\
			map[2 * rowsize + 0] = (R3C1);\
			map[2 * rowsize + 1] = (R3C2);\
		}\
		n0 = 2;\
		n1 = 2;\
		n2 = 2;\
	} while (0)

#define SET_2_X_3(R1C1, R1C2, R1C3, R2C1, R2C2, R2C3)\
	do {\
		if (map) {\
			if (rowsize < 3)\
				goto einval;\
			map[0 * rowsize + 0] = (R1C1);\
			map[0 * rowsize + 1] = (R1C2);\
			map[0 * rowsize + 2] = (R1C3);\
			map[1 * rowsize + 0] = (R2C1);\
			map[1 * rowsize + 1] = (R2C2);\
			map[1 * rowsize + 2] = (R2C3);\
		}\
		n0 = 2;\
		n1 = 2;\
		n2 = 2;\
	} while (0)

	switch (layout) {
	case LIBFONTS_SUBPIXEL_ORDER_CLASS_123:
		SET_1_X_3(0, 1, 2);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_1_2_3:
		SET_3_X_1(0,
		          1,
		          2);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23:
		SET_2_X_2(0, 0,
		          1, 2);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_21_31:
		SET_2_X_2(1, 0,
		          2, 0);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_32_11:
		SET_2_X_2(2, 1,
		          0, 0);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_13_12:
		SET_2_X_2(0, 2,
		          0, 1);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_11_23:
		SET_3_X_2(0, 0,
		          1, 2,
		          1, 2);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_21_31:
		SET_2_X_3(1, 1, 0,
		          2, 2, 0);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_32_11:
		SET_3_X_2(2, 1,
		          2, 1,
		          0, 0);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_13_12:
		SET_2_X_3(0, 2, 2,
		          0, 1, 1);
		break;

	case LIBFONTS_SUBPIXEL_ORDER_CLASS_OTHER:
	default:
		goto einval;
	}

	if (n0p)
		*n0p = n0;
	if (n1p)
		*n1p = n1;
	if (n2p)
		*n2p = n2;

	return 0;

einval:
	errno = EINVAL;
	return -1;
}


#else


int
main(void)
{
	uint8_t map[128], ns[3];
	size_t i, j, rowsize;

#define T(L)\
	do {\
		errno = 0;\
		ASSERT(libfonts_get_subpixel_map(L, 16, NULL, NULL, NULL, NULL) == -1);\
		ASSERT(errno == EINVAL);\
		\
		errno = 0;\
		ASSERT(libfonts_get_subpixel_map(L, 16, map, NULL, NULL, NULL) == -1);\
		ASSERT(errno == EINVAL);\
		\
		errno = 0;\
		ASSERT(libfonts_get_subpixel_map(L, 16, map, &ns[0], &ns[1], &ns[2]) == -1);\
		ASSERT(errno == EINVAL);\
	} while (0)

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_OTHER);
	T((enum libfonts_subpixel_order_class)999);

#undef T

#define TEST_1_X_3(R1C1, R1C2, R1C3)\
	do {\
		ASSERT(map[0] == (R1C1) - 1);\
		ASSERT(map[1] == (R1C2) - 1);\
		ASSERT(map[2] == (R1C3) - 1);\
		ASSERT(ns[0] == 1);\
		ASSERT(ns[1] == 1);\
		ASSERT(ns[2] == 1);\
	} while (0)

#define TEST_3_X_1(R1C1, R2C1, R3C1)\
	do {\
		ASSERT(map[0 * rowsize] == (R1C1) - 1);\
		ASSERT(map[1 * rowsize] == (R2C1) - 1);\
		ASSERT(map[2 * rowsize] == (R3C1) - 1);\
		ASSERT(ns[0] == 1);\
		ASSERT(ns[1] == 1);\
		ASSERT(ns[2] == 1);\
	} while (0)

#define TEST_2_X_2(R1C1, R1C2, R2C1, R2C2)\
	do {\
		ASSERT(map[0 * rowsize + 0] == (R1C1) - 1);\
		ASSERT(map[0 * rowsize + 1] == (R1C2) - 1);\
		ASSERT(map[1 * rowsize + 0] == (R2C1) - 1);\
		ASSERT(map[1 * rowsize + 1] == (R2C2) - 1);\
		ASSERT(ns[0] == 2);\
		ASSERT(ns[1] == 1);\
		ASSERT(ns[2] == 1);\
	} while (0)

#define TEST_3_X_2(R1C1, R1C2, R2C1, R2C2, R3C1, R3C2)\
	do {\
		ASSERT(map[0 * rowsize + 0] == (R1C1) - 1);\
		ASSERT(map[0 * rowsize + 1] == (R1C2) - 1);\
		ASSERT(map[1 * rowsize + 0] == (R2C1) - 1);\
		ASSERT(map[1 * rowsize + 1] == (R2C2) - 1);\
		ASSERT(map[2 * rowsize + 0] == (R3C1) - 1);\
		ASSERT(map[2 * rowsize + 1] == (R3C2) - 1);\
		ASSERT(ns[0] == 2);\
		ASSERT(ns[1] == 2);\
		ASSERT(ns[2] == 2);\
	} while (0)

#define TEST_2_X_3(R1C1, R1C2, R1C3, R2C1, R2C2, R2C3)\
	do {\
		ASSERT(map[0 * rowsize + 0] == (R1C1) - 1);\
		ASSERT(map[0 * rowsize + 1] == (R1C2) - 1);\
		ASSERT(map[0 * rowsize + 2] == (R1C3) - 1);\
		ASSERT(map[1 * rowsize + 0] == (R2C1) - 1);\
		ASSERT(map[1 * rowsize + 1] == (R2C2) - 1);\
		ASSERT(map[1 * rowsize + 2] == (R2C3) - 1);\
		ASSERT(ns[0] == 2);\
		ASSERT(ns[1] == 2);\
		ASSERT(ns[2] == 2);\
	} while (0)

#define TEST_3_X_2_BALANCED_2_X_2(R1C1, R1C2, R2C1, R2C2)\
	TEST_3_X_2((R1C1), (R1C2),\
		   (R1C1) == (R1C2) ? (R2C1) : (R1C1),\
		   (R1C1) == (R1C2) ? (R2C2) : (R1C2),\
	           (R2C1), (R2C2))

#define TEST_2_X_3_BALANCED_2_X_2(R1C1, R1C2, R2C1, R2C2)\
	TEST_2_X_3((R1C1), (R1C1) == (R2C1) ? (R1C2) : (R1C1), (R1C2),\
	           (R2C1), (R1C1) == (R2C1) ? (R2C2) : (R2C1), (R2C2))

#define CHECK_NS(H, W, N0, N1, N2)\
	do {\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		for (i = 0; i < (H); i++) {\
			for (j = 0; j < (W); j++) {\
				ASSERT(map[i * rowsize + j] <= 2);\
				ns[map[i * rowsize + j]] += 1;\
			}\
		}\
		ASSERT(ns[0] == N0);\
		ASSERT(ns[1] == N1);\
		ASSERT(ns[2] == N2);\
	} while (0)

#define T_(L, ROWSIZE, H, W, MAP, N0, N1, N2, NSCHECK, MAPTEST)\
	do {\
		rowsize = (ROWSIZE);\
		\
		errno = EDOM;\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		ASSERT(!libfonts_get_subpixel_map(L, rowsize, MAP, NULL, NULL, NULL));\
		ASSERT(errno == EDOM);\
		NSCHECK;\
		MAPTEST;\
		\
		errno = EDOM;\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		ASSERT(!libfonts_get_subpixel_map(L, rowsize, MAP, &ns[0], NULL, NULL));\
		ASSERT(errno == EDOM);\
		ASSERT(ns[0] == (N0));\
		NSCHECK;\
		MAPTEST;\
		\
		errno = EDOM;\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		ASSERT(!libfonts_get_subpixel_map(L, rowsize, MAP, NULL, &ns[1], NULL));\
		ASSERT(errno == EDOM);\
		ASSERT(ns[1] == (N1));\
		NSCHECK;\
		MAPTEST;\
		\
		errno = EDOM;\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		ASSERT(!libfonts_get_subpixel_map(L, rowsize, MAP, &ns[0], &ns[1], NULL));\
		ASSERT(errno == EDOM);\
		ASSERT(ns[0] == (N0));\
		ASSERT(ns[1] == (N1));\
		NSCHECK;\
		MAPTEST;\
		\
		errno = EDOM;\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		ASSERT(!libfonts_get_subpixel_map(L, rowsize, MAP, NULL, NULL, &ns[2]));\
		ASSERT(errno == EDOM);\
		ASSERT(ns[2] == (N2));\
		NSCHECK;\
		MAPTEST;\
		\
		errno = EDOM;\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		ASSERT(!libfonts_get_subpixel_map(L, rowsize, MAP, &ns[0], NULL, &ns[2]));\
		ASSERT(errno == EDOM);\
		ASSERT(ns[0] == (N0));\
		ASSERT(ns[2] == (N2));\
		NSCHECK;\
		MAPTEST;\
		\
		errno = EDOM;\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		ASSERT(!libfonts_get_subpixel_map(L, rowsize, MAP, NULL, &ns[1], &ns[2]));\
		ASSERT(errno == EDOM);\
		ASSERT(ns[1] == (N1));\
		ASSERT(ns[2] == (N2));\
		NSCHECK;\
		MAPTEST;\
		\
		errno = EDOM;\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		ASSERT(!libfonts_get_subpixel_map(L, rowsize, MAP, &ns[0], &ns[1], &ns[2]));\
		ASSERT(errno == EDOM);\
		ASSERT(ns[0] == (N0));\
		ASSERT(ns[1] == (N1));\
		ASSERT(ns[2] == (N2));\
		NSCHECK;\
		MAPTEST;\
	} while (0)

#define T2_(L, ROWSIZE, H, N0, N1, N2, N0P, N1P, N2P)\
	do {\
		errno = EDOM;\
		ns[0] = 0, ns[1] = 0, ns[2] = 0;\
		ASSERT(!libfonts_get_subpixel_map(L, ROWSIZE, NULL, N0P, N1P, N2P));\
		ASSERT(errno == EDOM);\
		ASSERT(ns[0] == (N0));\
		ASSERT(ns[1] == (N1));\
		ASSERT(ns[2] == (N2));\
		\
		if (H > 1) {\
			errno = 0;\
			ASSERT(libfonts_get_subpixel_map(L, ROWSIZE, map, N0P, N1P, N2P) == -1);\
			ASSERT(errno == EINVAL);\
		} else {\
			errno = EDOM;\
			ns[0] = 0, ns[1] = 0, ns[2] = 0;\
			ASSERT(!libfonts_get_subpixel_map(L, ROWSIZE, map, N0P, N1P, N2P));\
			ASSERT(errno == EDOM);\
			ASSERT(ns[0] == (N0));\
			ASSERT(ns[1] == (N1));\
			ASSERT(ns[2] == (N2));\
		}\
	} while (0)

#define T(L, H, W, N0, N1, N2, MAPTEST)\
	do {\
		T_(L, W, H, W, map, N0, N1, N2, CHECK_NS(H, W, N0, N1, N2), MAPTEST);\
		T_(L, W, H, W, NULL, N0, N1, N2,,);\
		T_(L, W + 1, H, W, map, N0, N1, N2, CHECK_NS(H, W, N0, N1, N2), MAPTEST);\
		T_(L, W + 1, H, W, NULL, N0, N1, N2,,);\
		T2_(L, W - 1, H, 0, 0, 0, NULL, NULL, NULL);\
		T2_(L, W - 1, H, N0, 0, 0, &ns[0], NULL, NULL);\
		T2_(L, W - 1, H, 0, N1, 0, NULL, &ns[1], NULL);\
		T2_(L, W - 1, H, N0, N1, 0, &ns[0], &ns[1], NULL);\
		T2_(L, W - 1, H, 0, 0, N2, NULL, NULL, &ns[2]);\
		T2_(L, W - 1, H, N0, 0, N2, &ns[0], NULL, &ns[2]);\
		T2_(L, W - 1, H, 0, N1, N2, NULL, &ns[1], &ns[2]);\
		T2_(L, W - 1, H, N0, N1, N2, &ns[0], &ns[1], &ns[2]);\
	} while (0)

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_123,
	  1, 3, 1, 1, 1,
	  TEST_1_X_3(1, 2, 3));

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_1_2_3,
	  3, 1, 1, 1, 1,
	  TEST_3_X_1(1,
	             2,
	             3));

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23,
	  2, 2, 2, 1, 1,
	  TEST_2_X_2(1, 1,
	             2, 3));

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_21_31,
	  2, 2, 2, 1, 1,
	  TEST_2_X_2(2, 1,
	             3, 1));

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_32_11,
	  2, 2, 2, 1, 1,
	  TEST_2_X_2(3, 2,
	             1, 1));

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_13_12,
	  2, 2, 2, 1, 1,
	  TEST_2_X_2(1, 3,
	             1, 2));

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_11_23,
	  3, 2, 2, 2, 2,
	  TEST_3_X_2(1, 1,
	             2, 3,
	             2, 3));

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_21_31,
	  2, 3, 2, 2, 2,
	  TEST_2_X_3(2, 2, 1,
	             3, 3, 1));

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_32_11,
	  3, 2, 2, 2, 2,
	  TEST_3_X_2_BALANCED_2_X_2(3, 2,
	                            1, 1));

	T(LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_13_12,
	  2, 3, 2, 2, 2,
	  TEST_2_X_3_BALANCED_2_X_2(1, 3,
	                            1, 2));

	return 0;
}


#endif
