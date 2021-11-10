/* See LICENSE file for copyright and license details. */
#include "libfonts.h"


enum libfonts_subpixel_order
libfonts_calculate_subpixel_order(enum libfonts_subpixel_order unrotated, enum libfonts_orientation rotation)
{
	int rot;

	switch (rotation) {
	case LIBFONTS_ORIENTATION_UNKNOWN:
	default:
		switch (unrotated) {
		case LIBFONTS_SUBPIXEL_ORDER_UNKNOWN:
		case LIBFONTS_SUBPIXEL_ORDER_NONRGB:
		case LIBFONTS_SUBPIXEL_ORDER_NONLINEAR:
		case LIBFONTS_SUBPIXEL_ORDER_OTHER:
			return unrotated;
		default:
			return LIBFONTS_SUBPIXEL_ORDER_UNKNOWN;
		}
		break;

	case LIBFONTS_ORIENTATION_0_DEGREES_CLOCKWISE:
		return unrotated;

	case LIBFONTS_ORIENTATION_90_DEGREES_CLOCKWISE:
		rot = 1;
		break;

	case LIBFONTS_ORIENTATION_180_DEGREES_CLOCKWISE:
		rot = 2;
		break;

	case LIBFONTS_ORIENTATION_270_DEGREES_CLOCKWISE:
		rot = 3;
		break;

	case LIBFONTS_ORIENTATION_OTHER:
		switch (unrotated) {
		case LIBFONTS_SUBPIXEL_ORDER_UNKNOWN:
		case LIBFONTS_SUBPIXEL_ORDER_NONRGB:
		case LIBFONTS_SUBPIXEL_ORDER_NONLINEAR:
			return unrotated;
		default:
			return LIBFONTS_SUBPIXEL_ORDER_NONLINEAR;
		}
		break;
	}

	switch (unrotated) {
	case LIBFONTS_SUBPIXEL_ORDER_UNKNOWN:
	case LIBFONTS_SUBPIXEL_ORDER_NONRGB:
	case LIBFONTS_SUBPIXEL_ORDER_NONLINEAR:
	case LIBFONTS_SUBPIXEL_ORDER_OTHER:
		return unrotated;
	default:
		return ((unrotated + rot) & 3) + (unrotated & ~3);
	}
}
