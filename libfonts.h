/* See LICENSE file for copyright and license details. */
#ifndef LIBFONTS_H
#define LIBFONTS_H

#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>


/**
 * The header file's version of `struct libfonts_context`
 */
#define LIBFONTS_CONTEXT_VERSION 0

/**
 * File name of file in font directories that is used
 * to create font aliases
 */
#define LIBFONTS_ALIAS_FILE_NAME "fonts.alias"

/**
 * File name of file in font directories that is used
 * enumerate all font files, and their font names, in
 * in the directory
 */
#define LIBFONTS_DIR_FILE_NAME "fonts.dir"

/**
 * File name of file in font directories that is used
 * enumerate all scalable font files, and their font
 * names, in the directory
 * 
 * `LIBFONTS_SCALE_DIR_FILE_NAME` enumerates a subset
 * of `LIBFONTS_DIR_FILE_NAME`
 */
#define LIBFONTS_SCALE_DIR_FILE_NAME "fonts.scale"

/**
 * File name of file in font directories that is used
 * enumerate character encoding scheme files, and
 * their CHARSET_REGISTRY-CHARSET_ENCODING XLFD propery
 * pair, in the directory
 */
#define LIBFONTS_ENCODING_DIR_FILE_NAME "encodings.dir"


#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wpadded"
#endif


/**
 * Style-based default fonts
 */
enum libfonts_default_font {
	/**
	 * A proportional font without serifs
	 */
	LIBFONTS_DEFAULT_SANS_SERIF,

	/**
	 * A proportional font with serifs
	 */
	LIBFONTS_DEFAULT_SERIF,

	/**
	 * A monospace font, which may escape
	 * the character cell
	 */
	LIBFONTS_DEFAULT_MONOSPACE
};


/**
 * Alias font list file type
 */
enum libfonts_alias_line_type {
	/**
	 * Improperly formatted line
	 */
	LIBFONTS_ALIAS_LINE_MALFORMATTED,

	/**
	 * Empty or white-space only line
	 */
	LIBFONTS_ALIAS_LINE_BLANK,

	/**
	 * Comment line
	 */
	LIBFONTS_ALIAS_LINE_COMMENT,

	/**
	 * Alias definition line
	 */
	LIBFONTS_ALIAS_LINE_ALIAS_DEFINITION,

	/**
	 * Line containing just the string “FILE_NAMES_ALIASES”
	 * indicating that the file name, minus its suffix, of
	 * each font file, in the same directory as the font alias
	 * file, should be set as an alias for the font contained
	 * in that file
	 * 
	 * For example, if the director contains the files
	 * “My Font.ttf” and “My Font Bold.ttf”, the font names
	 * “My Font” and “My Font Bold” should be aliases for the
	 * two fonts, whose proper names are determined by their
	 * content and ought to be listed in the file “fonts.dir”
	 * in the same directory. Note fonts may be compressed
	 * with e.g. gzip, and therefor have an extra suffix
	 * (e.g. .gz), in such cases both the compression suffix
	 * and the font file type suffix shall be stripped, e.g.
	 * “My Font 16.pcf.gz” becomes “My Font 16”. You should
	 * however not assume that the suffixless basename does
	 * not contain any dot.
	 */
	LIBFONTS_ALIAS_LINE_FONT_NAMES_ALIASES
};


/**
 * Antialiasing algorithm
 */
enum libfonts_antialiasing {
	/**
	 * No antialiasing rule specified, use default or if default
	 * is also unspecified: `LIBFONTS_ANTIALIASING_GREYSCALE`
	 */
	LIBFONTS_ANTIALIASING_UNSPECIFIED,

	/**
	 * Do not antialias
	 */
	LIBFONTS_ANTIALIASING_NONE,

	/**
	 * Use greyscale-antialiasing
	 */
	LIBFONTS_ANTIALIASING_GREYSCALE,

	/**
	 * Use greyscale-antialiasing regardless
	 * of other settings
	 */
	LIBFONTS_ANTIALIASING_GREYSCALE_FORCED,

	/**
	 * Use subpixel rendering
	 */
	LIBFONTS_ANTIALIASING_SUBPIXEL,

	/**
	 * Use subpixel rendering regardless of
	 * other settings
	 */
	LIBFONTS_ANTIALIASING_SUBPIXEL_FORCED
};


/**
 * Output subpixel colour
 */
enum libfonts_subpixel_colour {
	/**
	 * Red primary
	 */
	LIBFONTS_SUBPIXEL_COLOUR_RED = 0,

	/**
	 * Green primary
	 */
	LIBFONTS_SUBPIXEL_COLOUR_GREEN = 1,

	/**
	 * Blue primary
	 */
	LIBFONTS_SUBPIXEL_COLOUR_BLUE = 2
};

/**
 * Output subpixel order class
 * 
 * Descriptions of relative dimensions for the members
 * of this enumerated type assume that the monitor's
 * vertical and horizontal DPI (dots per inch) are equal
 */
enum libfonts_subpixel_order_class {
	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_UNKNOWN`,
	 * `LIBFONTS_SUBPIXEL_ORDER_NONRGB`,
	 * `LIBFONTS_SUBPIXEL_ORDER_NONLINEAR`, or
	 * `LIBFONTS_SUBPIXEL_ORDER_OTHER`
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_OTHER,

	/**
	 * A square of horizontally stacked vertical
	 * stripes, numbered from left to right
	 * 
	 * ┌───┬───┬───┐
	 * │   │   │   │
	 * │   │   │   │
	 * │ 1 │ 2 │ 3 │
	 * │   │   │   │
	 * │   │   │   │
	 * └───┴───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_123,

	/**
	 * A square of vertically stacked horizontal
	 * stripes, numbered from top to bottom
	 * 
	 * ┌───────────┐
	 * │     1     │
	 * ├───────────┤
	 * │     2     │
	 * ├───────────┤
	 * │     3     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_1_2_3,

	/**
	 * A square of two squares with a horizontal
	 * 2:1-oblong rectangle stacked above them;
	 * the rectangle is numbered 1, and the two
	 * squares numbered 2 and 3 from left to right
	 * 
	 * ┌───────┐
	 * │   1   │
	 * ├───┬───┤
	 * │ 2 │ 3 │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23,

	/**
	 * A square of two squares vertically stacked
	 * squares with a vertical 2:1-oblong rectangle
	 * to their right; the rectangle is numbered 1,
	 * and the two squares are numbered 2 and 3 from
	 * top to bottom
	 * 
	 * ┌───┬───┐
	 * │ 2 │   │
	 * ├───┤ 1 │
	 * │ 3 │   │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_21_31,

	/**
	 * A square of two squares with a horizontal
	 * 2:1-oblong rectangle stacked beneath them;
	 * the rectangle is numbered 1, and the two
	 * squares numbered 2 and 3 from right to left
	 * 
	 * ┌───┬───┐
	 * │ 3 │ 2 │
	 * ├───┴───┤
	 * │   1   │
	 * └───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_32_11,

	/**
	 * A square of two squares vertically stacked
	 * squares with a vertical 2:1-oblong rectangle
	 * to their left; the rectangle is numbered 1,
	 * and the two squares are numbered 2 and 3 from
	 * bottom to top
	 * 
	 * ┌───┬───┐
	 * │   │ 3 │
	 * │ 1 ├───┤
	 * │   │ 2 │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_13_12,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_CLASS_11_23`
	 * with the subpixels redimensioned to
	 * balance the subpixels to have equal size
	 * 
	 * ┌───────────┐
	 * │     1     │
	 * ├─────┬─────┤
	 * │     │     │
	 * │  2  │  3  │
	 * │     │     │
	 * └─────┴─────┘
	 *
	 * Cells 2 and 3 have the aspect ratio
	 * (width to height) 3:4 and cell 1 has
	 * the aspect ratio 6:2
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_11_23,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_CLASS_21_21`
	 * with the subpixels redimensioned to
	 * balance the subpixels to have equal size
	 * 
	 * ┌───────┬───┐
	 * │   2   │   │
	 * │       │   │
	 * ├───────┤ 1 │
	 * │   3   │   │
	 * │       │   │
	 * └───────┴───┘
	 *
	 * Cells 2 and 3 have the aspect ratio
	 * (width to height) 4:3 and cell 1 has
	 * the aspect ratio 2:6
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_21_31,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_CLASS_32_11`
	 * with the subpixels redimensioned to
	 * balance the subpixels to have equal size
	 * 
	 * ┌─────┬─────┐
	 * │     │     │
	 * │  3  │  2  │
	 * │     │     │
	 * ├─────┴─────┤
	 * │     1     │
	 * └───────────┘
	 *
	 * Cells 2 and 3 have the aspect ratio
	 * (width to height) 3:4 and cell 1 has
	 * the aspect ratio 6:2
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_32_11,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_CLASS_13_12`
	 * with the subpixels redimensioned to
	 * balance the subpixels to have equal size
	 * 
	 * ┌───┬───────┐
	 * │   │   3   │
	 * │   │       │
	 * │ 1 ├───────┤
	 * │   │   2   │
	 * │   │       │
	 * └───┴───────┘
	 *
	 * Cells 2 and 3 have the aspect ratio
	 * (width to height) 4:3 and cell 1 has
	 * the aspect ratio 2:6
	 */
	LIBFONTS_SUBPIXEL_ORDER_CLASS_BALANCED_13_12
};


/**
 * Output subpixel order
 */
enum libfonts_subpixel_order {
	/**
	 * Unknown subpixel order
	 */
	LIBFONTS_SUBPIXEL_ORDER_UNKNOWN,

	/**
	 * Output is not an RGB output device
	 */
	LIBFONTS_SUBPIXEL_ORDER_NONRGB,

	/**
	 * Output is an RGB output device, but the subpixels
	 * are not ordered in a grid of rectangles or subpixels
	 * are disjoint
	 * 
	 * This includes the patterns (with disjoint subpixels)
	 * 
	 *     ┌───┬───┐ (LIBFONTS_SUBPIXEL_ORDER_RG_BR,
	 *     │ 1 │ 2 │  LIBFONTS_SUBPIXEL_ORDER_RB_GR,
	 *     ├───┼───┤  LIBFONTS_SUBPIXEL_ORDER_GR_BG,
	 *     │ 3 │ 1 │  LIBFONTS_SUBPIXEL_ORDER_GB_RG,
	 *     └───┴───┘  LIBFONTS_SUBPIXEL_ORDER_BR_GB,
	 *                LIBFONTS_SUBPIXEL_ORDER_BG_RB)
	 * and
	 *               (LIBFONTS_SUBPIXEL_ORDER_RG_GB,
	 *     ┌───┬───┐  LIBFONTS_SUBPIXEL_ORDER_RB_BG,
	 *     │ 1 │ 2 │  LIBFONTS_SUBPIXEL_ORDER_GR_RB,
	 *     ├───┼───┤  LIBFONTS_SUBPIXEL_ORDER_GB_BR,
	 *     │ 2 │ 3 │  LIBFONTS_SUBPIXEL_ORDER_BR_RG,
	 *     └───┴───┘  LIBFONTS_SUBPIXEL_ORDER_BG_GR)
	 * 
	 * as well trianglar arrangements as found on many
	 * CRT-monitors
	 */
	LIBFONTS_SUBPIXEL_ORDER_NONLINEAR,

	/**
	 * Output is an RGB output device, but the subpixel
	 * order is not one of the supported subpixel orders
	 */
	LIBFONTS_SUBPIXEL_ORDER_OTHER,



	/**
	 * Horizontally stacked vertical stripes,
	 * red to the left, green in the middle, and blue to the right
	 * 
	 * ┌───┬───┬───┐
	 * │   │   │   │
	 * │   │   │   │
	 * │ R │ G │ B │
	 * │   │   │   │
	 * │   │   │   │
	 * └───┴───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RGB,

	/**
	 * Vertically stacked horizontal stripes,
	 * red at the top, green in the middle, and blue at the bottom
	 * 
	 * ┌───────────┐
	 * │     R     │
	 * ├───────────┤
	 * │     G     │
	 * ├───────────┤
	 * │     B     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_R_G_B,

	/**
	 * Horizontally stacked vertical stripes,
	 * blue to the left, green in the middle, and red to the right
	 * 
	 * ┌───┬───┬───┐
	 * │   │   │   │
	 * │   │   │   │
	 * │ B │ G │ R │
	 * │   │   │   │
	 * │   │   │   │
	 * └───┴───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BGR,

	/**
	 * Vertically stacked horizontal stripes,
	 * blue at the top, green in the middle, and red at the bottom
	 * 
	 * ┌───────────┐
	 * │     B     │
	 * ├───────────┤
	 * │     G     │
	 * ├───────────┤
	 * │     R     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_B_G_R,



	/**
	 * Horizontally stacked vertical stripes,
	 * green to the left, blue in the middle, and red to the right
	 * 
	 * ┌───┬───┬───┐
	 * │   │   │   │
	 * │   │   │   │
	 * │ G │ B │ R │
	 * │   │   │   │
	 * │   │   │   │
	 * └───┴───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GBR,

	/**
	 * Vertically stacked horizontal stripes,
	 * green at the top, blue in the middle, and red at the bottom
	 * 
	 * ┌───────────┐
	 * │     G     │
	 * ├───────────┤
	 * │     B     │
	 * ├───────────┤
	 * │     R     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_G_B_R,

	/**
	 * Horizontally stacked vertical stripes,
	 * red to the left, blue in the middle, and green to the right
	 * 
	 * ┌───┬───┬───┐
	 * │   │   │   │
	 * │   │   │   │
	 * │ R │ B │ G │
	 * │   │   │   │
	 * │   │   │   │
	 * └───┴───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RBG,

	/**
	 * Vertically stacked horizontal stripes,
	 * red at the top, blue in the middle, and green at the bottom
	 * 
	 * ┌───────────┐
	 * │     R     │
	 * ├───────────┤
	 * │     B     │
	 * ├───────────┤
	 * │     G     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_R_B_G,



	/**
	 * Horizontally stacked vertical stripes,
	 * blue to the left, red in the middle, and green to the right
	 * 
	 * ┌───┬───┬───┐
	 * │   │   │   │
	 * │   │   │   │
	 * │ B │ R │ G │
	 * │   │   │   │
	 * │   │   │   │
	 * └───┴───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BRG,

	/**
	 * Vertically stacked horizontal stripes,
	 * blue at the top, red in the middle, and green at the bottom
	 * 
	 * ┌───────────┐
	 * │     B     │
	 * ├───────────┤
	 * │     R     │
	 * ├───────────┤
	 * │     G     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_B_R_G,

	/**
	 * Horizontally stacked vertical stripes,
	 * green to the left, red in the middle, and blue to the right
	 * 
	 * ┌───┬───┬───┐
	 * │   │   │   │
	 * │   │   │   │
	 * │ G │ R │ B │
	 * │   │   │   │
	 * │   │   │   │
	 * └───┴───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GRB,

	/**
	 * Vertically stacked horizontal stripes,
	 * green at the top, red in the middle, and blue at the bottom
	 * 
	 * ┌───────────┐
	 * │     G     │
	 * ├───────────┤
	 * │     R     │
	 * ├───────────┤
	 * │     B     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_G_R_B,



	/**
	 * Red subpixel stacked above, having equal height,
	 * green, on the left, and blue, on the right,
	 * square subpixels
	 * 
	 * ┌───────┐
	 * │   R   │
	 * ├───┬───┤
	 * │ G │ B │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RR_GB,

	/**
	 * Red subpixel stacked to the right, having
	 * equal width, of green, on the top, and
	 * blue, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ G │   │
	 * ├───┤ R │
	 * │ B │   │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GR_BR,

	/**
	 * Red subpixel stacked beneath, having equal
	 * height, blue, on the left, and green, on
	 * the right, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ B │ G │
	 * ├───┴───┤
	 * │   R   │
	 * └───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BG_RR,

	/**
	 * Red subpixel stacked to the left, having
	 * equal width, of blue, on the top, and
	 * green, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │   │ B │
	 * │ R ├───┤
	 * │   │ G │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RB_RG,



	/**
	 * Red subpixel stacked above, having equal height,
	 * blue, on the left, and green, on the right,
	 * square subpixels
	 * 
	 * ┌───────┐
	 * │   R   │
	 * ├───┬───┤
	 * │ B │ G │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RR_BG,

	/**
	 * Red subpixel stacked to the right, having
	 * equal width, of blue, on the top, and
	 * green, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ B │   │
	 * ├───┤ R │
	 * │ G │   │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BR_GR,

	/**
	 * Red subpixel stacked beneath, having equal
	 * height, green, on the left, and blue, on
	 * the right, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ G │ B │
	 * ├───┴───┤
	 * │   R   │
	 * └───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GB_RR,

	/**
	 * Red subpixel stacked to the left, having
	 * equal width, of green, on the top, and
	 * blue, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │   │ G │
	 * │ R ├───┤
	 * │   │ B │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RG_RB,



	/**
	 * Green subpixel stacked above, having equal height,
	 * red, on the left, and blue, on the right,
	 * square subpixels
	 * 
	 * ┌───────┐
	 * │   G   │
	 * ├───┬───┤
	 * │ R │ B │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GG_RB,

	/**
	 * Green subpixel stacked to the right, having
	 * equal width, of red, on the top, and
	 * blue, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ R │   │
	 * ├───┤ G │
	 * │ B │   │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RG_BG,

	/**
	 * Green subpixel stacked beneath, having equal
	 * height, blue, on the left, and red, on
	 * the right, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ B │ R │
	 * ├───┴───┤
	 * │   G   │
	 * └───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BR_GG,

	/**
	 * Green subpixel stacked to the left, having
	 * equal width, of blue, on the top, and
	 * red, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │   │ B │
	 * │ G ├───┤
	 * │   │ R │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GB_GR,



	/**
	 * Green subpixel stacked above, having equal height,
	 * blue, on the left, and red, on the right,
	 * square subpixels
	 * 
	 * ┌───────┐
	 * │   G   │
	 * ├───┬───┤
	 * │ B │ R │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GG_BR,

	/**
	 * Green subpixel stacked to the right, having
	 * equal width, of blue, on the top, and
	 * red, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ B │   │
	 * ├───┤ G │
	 * │ R │   │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BG_RG,

	/**
	 * Green subpixel stacked beneath, having equal
	 * height, red, on the left, and blue, on
	 * the right, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ R │ B │
	 * ├───┴───┤
	 * │   G   │
	 * └───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RB_GG,

	/**
	 * Green subpixel stacked to the left, having
	 * equal width, of red, on the top, and
	 * blue, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │   │ R │
	 * │ G ├───┤
	 * │   │ B │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GR_GB,



	/**
	 * Blue subpixel stacked above, having equal height,
	 * red, on the left, and green, on the right,
	 * square subpixels
	 * 
	 * ┌───────┐
	 * │   B   │
	 * ├───┬───┤
	 * │ R │ G │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BB_RG,

	/**
	 * Blue subpixel stacked to the right, having
	 * equal width, of red, on the top, and
	 * green, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ R │   │
	 * ├───┤ B │
	 * │ G │   │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RB_GB,

	/**
	 * Blue subpixel stacked beneath, having equal
	 * height, green, on the left, and red, on
	 * the right, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ G │ R │
	 * ├───┴───┤
	 * │   B   │
	 * └───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GR_BB,

	/**
	 * Blue subpixel stacked to the left, having
	 * equal width, of green, on the top, and
	 * red, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │   │ G │
	 * │ B ├───┤
	 * │   │ R │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BG_BR,



	/**
	 * Blue subpixel stacked above, having equal height,
	 * green, on the left, and red, on the right,
	 * square subpixels
	 * 
	 * ┌───────┐
	 * │   B   │
	 * ├───┬───┤
	 * │ G │ R │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BB_GR,

	/**
	 * Blue subpixel stacked to the right, having
	 * equal width, of green, on the top, and
	 * red, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ G │   │
	 * ├───┤ B │
	 * │ R │   │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_GB_RB,

	/**
	 * Blue subpixel stacked beneath, having equal
	 * height, red, on the left, and green, on
	 * the right, square subpixels
	 * 
	 * ┌───┬───┐
	 * │ R │ G │
	 * ├───┴───┤
	 * │   B   │
	 * └───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_RG_BB,

	/**
	 * Blue subpixel stacked to the left, having
	 * equal width, of red, on the top, and
	 * green, on the bottom, square subpixels
	 * 
	 * ┌───┬───┐
	 * │   │ R │
	 * │ B ├───┤
	 * │   │ G │
	 * └───┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BR_BG,



	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_RR_GB` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────────┐
	 * │     R     │
	 * ├─────┬─────┤
	 * │     │     │
	 * │  G  │  B  │
	 * │     │     │
	 * └─────┴─────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_RR_GB,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_GR_BR` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────┬───┐
	 * │   G   │   │
	 * │       │   │
	 * ├───────┤ R │
	 * │   B   │   │
	 * │       │   │
	 * └───────┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_GR_BR,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_BG_RR` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌─────┬─────┐
	 * │     │     │
	 * │  B  │  G  │
	 * │     │     │
	 * ├─────┴─────┤
	 * │     R     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_BG_RR,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_RB_RG` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───┬───────┐
	 * │   │   B   │
	 * │   │       │
	 * │ R ├───────┤
	 * │   │   G   │
	 * │   │       │
	 * └───┴───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_RB_RG,



	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_RR_BG` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────────┐
	 * │     R     │
	 * ├─────┬─────┤
	 * │     │     │
	 * │  B  │  G  │
	 * │     │     │
	 * └─────┴─────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_RR_BG,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_BR_GR` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────┬───┐
	 * │   B   │   │
	 * │       │   │
	 * ├───────┤ R │
	 * │   G   │   │
	 * │       │   │
	 * └───────┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_BR_GR,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_GB_RR` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌─────┬─────┐
	 * │     │     │
	 * │  G  │  B  │
	 * │     │     │
	 * ├─────┴─────┤
	 * │     R     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_GB_RR,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_RG_RB` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───┬───────┐
	 * │   │   G   │
	 * │   │       │
	 * │ R ├───────┤
	 * │   │   B   │
	 * │   │       │
	 * └───┴───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_RG_RB,



	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_GG_RB` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────────┐
	 * │     G     │
	 * ├─────┬─────┤
	 * │     │     │
	 * │  R  │  B  │
	 * │     │     │
	 * └─────┴─────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_GG_RB,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_RG_BG` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────┬───┐
	 * │   R   │   │
	 * │       │   │
	 * ├───────┤ G │
	 * │   B   │   │
	 * │       │   │
	 * └───────┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_RG_BG,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_BR_GG` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌─────┬─────┐
	 * │     │     │
	 * │  B  │  R  │
	 * │     │     │
	 * ├─────┴─────┤
	 * │     G     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_BR_GG,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_GB_GR` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───┬───────┐
	 * │   │   B   │
	 * │   │       │
	 * │ G ├───────┤
	 * │   │   R   │
	 * │   │       │
	 * └───┴───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_GB_GR,



	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_GG_BR` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────────┐
	 * │     G     │
	 * ├─────┬─────┤
	 * │     │     │
	 * │  B  │  R  │
	 * │     │     │
	 * └─────┴─────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_GG_BR,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_BG_RG` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────┬───┐
	 * │   B   │   │
	 * │       │   │
	 * ├───────┤ G │
	 * │   R   │   │
	 * │       │   │
	 * └───────┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_BG_RG,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_RB_GG` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌─────┬─────┐
	 * │     │     │
	 * │  R  │  B  │
	 * │     │     │
	 * ├─────┴─────┤
	 * │     G     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_RB_GG,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_GR_GB` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───┬───────┐
	 * │   │   R   │
	 * │   │       │
	 * │ G ├───────┤
	 * │   │   B   │
	 * │   │       │
	 * └───┴───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_GR_GB,



	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_BB_RG` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────────┐
	 * │     B     │
	 * ├─────┬─────┤
	 * │     │     │
	 * │  R  │  G  │
	 * │     │     │
	 * └─────┴─────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_BB_RG,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_RB_GB` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────┬───┐
	 * │   R   │   │
	 * │       │   │
	 * ├───────┤ B │
	 * │   G   │   │
	 * │       │   │
	 * └───────┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_RB_GB,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_GR_BB` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌─────┬─────┐
	 * │     │     │
	 * │  G  │  R  │
	 * │     │     │
	 * ├─────┴─────┤
	 * │     B     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_GR_BB,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_BG_BR` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───┬───────┐
	 * │   │   G   │
	 * │   │       │
	 * │ B ├───────┤
	 * │   │   R   │
	 * │   │       │
	 * └───┴───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_BG_BR,



	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_BB_GR` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────────┐
	 * │     B     │
	 * ├─────┬─────┤
	 * │     │     │
	 * │  G  │  R  │
	 * │     │     │
	 * └─────┴─────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_BB_GR,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_GB_RB` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───────┬───┐
	 * │   G   │   │
	 * │       │   │
	 * ├───────┤ B │
	 * │   R   │   │
	 * │       │   │
	 * └───────┴───┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_GB_RB,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_RG_BB` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌─────┬─────┐
	 * │     │     │
	 * │  R  │  G  │
	 * │     │     │
	 * ├─────┴─────┤
	 * │     B     │
	 * └───────────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_RG_BB,

	/**
	 * `LIBFONTS_SUBPIXEL_ORDER_BR_BG` with
	 * the subpixels stretched to balance
	 * the subpixels to have equal size
	 * 
	 * ┌───┬───────┐
	 * │   │   R   │
	 * │   │       │
	 * │ B ├───────┤
	 * │   │   G   │
	 * │   │       │
	 * └───┴───────┘
	 */
	LIBFONTS_SUBPIXEL_ORDER_BALANCED_BR_BG
};


/**
 * Output transformation structure
 */
struct libfonts_transformation {
	/**
	 * Transformation matrix, row-major
	 *
	 *     ⎛.m[0][0]   .m[0][1]   .m[0][2]⎞
	 *     ⎜.m[1][0]   .m[1][1]   .m[1][2]⎟
	 *     ⎝.m[2][0]   .m[2][1]   .m[2][2]⎠
	 *
	 * Let all values be zero if it is unknown
	 */
	double m[3][3];
};


/**
 * Text rendering settings
 */
struct libfonts_rendering_settings {
	/**
	 * The output device's horizontal pixel density,
	 * in pixels (not dots) per inch, for the reference
	 * width, without output transformations applied,
	 * 0 if not specified
	 */
	double dpi_x;

	/**
	 * The output device's vertical pixel density,
	 * in pixels (not dots) per inch, for the reference
	 * height, without output transformations applied,
	 * 0 if not specified
	 */
	double dpi_y;

	/**
	 * The width, in pixels, that `.dpi_x` is calculated
	 * for; if 0, `dpi_x` shall apply regardless of
	 * current resolution and transformations
	 */
	uint32_t reference_width;

	/**
	 * The height, in pixels, that `.dpi_y` is calculated
	 * for; if 0, `.dpi_y` shall apply regardless of
	 * current resolution and transformations
	 */
	uint32_t reference_height;

	/**
	 * If the product of the applicable pixel densities
	 * (horizontal and vertical) is less than this
	 * value, the antialiasing mode shall downgrade
	 * to `LIBFONTS_ANTIALIASING_NONE` if it is
	 * `LIBFONTS_ANTIALIASING_GREYSCALE` or
	 * `LIBFONTS_ANTIALIASING_SUBPIXEL`
	 */
	double min_dpsqi_for_greyscale;

	/**
	 * If the product of the applicable pixel densities
	 * (horizontal and vertical) is less than this
	 * value, the antialiasing mode shall downgrade
	 * to `LIBFONTS_ANTIALIASING_GREYSCALE` if it is
	 * `LIBFONTS_ANTIALIASING_SUBPIXEL`
	 */
	double min_dpsqi_for_subpixel;

	/**
	 * The output device's physical subpixel order,
	 * when it is not rotated
	 */
	enum libfonts_subpixel_order subpixel_order;

	/**
	 * Antialiasing mode for horizontal (on unrotated output), grey text
	 */
	enum libfonts_antialiasing horizontal_grey_text_antialiasing;

	/**
	 * Antialiasing mode for vertical (on unrotated output), grey text
	 */
	enum libfonts_antialiasing vertical_grey_text_antialiasing;

	/**
	 * Antialiasing mode for non-horizontal, non-vertical
	 * (on unrotated output), grey text
	 */
	enum libfonts_antialiasing diagonal_grey_text_antialiasing;

	/**
	 * Antialiasing mode for horizontal (on unrotated output),
	 * non-grey text
	 */
	enum libfonts_antialiasing horizontal_colour_text_antialiasing;

	/**
	 * Antialiasing mode for vertical (on unrotated output),
	 * non-grey text
	 */
	enum libfonts_antialiasing vertical_colour_text_antialiasing;

	/**
	 * Antialiasing mode for non-horizontal, non-vertical (on
	 * unrotated output), non-grey text
	 */
	enum libfonts_antialiasing diagonal_colour_text_antialiasing;
};


/**
 * Output device information structure
 */
struct libfonts_output {
	/**
	 * The output's X-location on the screen (pixels left)
	 */
	int32_t output_x;

	/**
	 * The output's Y-location on the screen (pixels down)
	 */
	int32_t output_y;

	/**
	 * The output's width, in pixels, on the screen
	 */
	uint32_t output_width;

	/**
	 * The output's height, in pixels, on the screen
	 */
	uint32_t output_height;

	/**
	 * The output's width, in pixels, before transformation is applied
	 */
	uint32_t unrotated_output_width;

	/**
	 * The output's height, in pixels, before transformation is applied
	 */
	uint32_t unrotated_output_height;

	/**
	 * The index of the screen the output belongs to
	 */
	int output_screen;

	/**
	 * The output's subpixel order, disregarding applied rotation
	 */
	enum libfonts_subpixel_order unrotated_subpixel_order;

	/**
	 * Transformation that is applied to the output
	 */
	struct libfonts_transformation output_transformation;

	/**
	 * The output's horizontal pixel density (pixels per inch),
	 * zero if not applicable or unknown, after transformation
	 * is applied
	 * 
	 * This `.unrotated_output_width` divided by the output's
	 * physical width in inches, with inverse of
	 * `.output_transformation` than applied
	 */
	double dpi_x;

	/**
	 * The output's vertical pixel density (pixels per inch),
	 * zero if not applicable or unknown, after transformation
	 * is applied
	 * 
	 * This `.unrotated_output_height` divided by the output's
	 * physical height in inches, with inverse of
	 * `.output_transformation` than applied
	 */
	double dpi_y;

	/**
	 * Text renderings settings for the output device
	 */
	struct libfonts_rendering_settings *rendering_settings;
};


/**
 * Font description structure
 */
struct libfonts_font_description {
	/**
	 * The font name unless the foundry is registered with X,
	 * in which case, all following fields will be `NULL`.
	 * This field is normally `NULL`.
	 * 
	 * This field will also be used to to store the font name
	 * (including the initial "-") if it is malformated.
	 */
	const char *private_font_name;

	/**
	 * Version if the XLFD used, `NULL` if not included in the font name
	 * (this is normally the case)
	 */
	const char *xlfd_version;

	/**
	 * X-registered foundry name (name of digitaliser, not designer)
	 */
	const char *foundry;

	/**
	 * Human-readable, non-translated, hyphen-free name of the font
	 */
	const char *family_name;

	 /**
	  * The nominal blackness of the font, according the the foundry's
	  * judgement. Human-readable and suitable for presentation to the
	  * user. "0" if the font is polymorphic.
	  * 
	  * Should not be used for font matching or substitution.
	  */
	const char *weight_name;

	/**
	 * Overall posture
	 * 
	 * "R" = Roman
	 * "I" = Italic
	 * "O" = Oblique
	 * "RI" = Reverse italic
	 * "RO" = Reverse oblique
	 * "OT" = Other
	 * numeric = Polymorphic
	 */
	const char *slant;

	/**
	 * The nominal width per horizontal unit of the font, according to the
	 * foundry's judgment. "0" if the font is polymorphic.
	 * 
	 * Should not be used for font matching or substitution.
	 * 
	 * Example values:
	 *     "Normal"
	 *     "Condensed"
	 *     "Narrow"
	 *     "Double Wide"
	 */
	const char *setwidth_name;

	/**
	 * Additional style information. Includes the "[" character if the
	 * font is polymorphic
	 * 
	 * Example values:
	 *     ""
	 *     "Serif"
	 *     "Sans Serif"
	 *     "Informal"
	 *     "Decorated"
	 */
	const char *add_style_name;

	/**
	 * The body size of the font at a particular point size and Y-resolution.
	 * Either an integer-string or (if using matrix transformation) a string
	 * beginning with "[". "0" if the font is scalable.
	 */
	const char *pixel_size;

	/**
	 * The body size the font was designed for. Either an integer-string or
	 * (if using matrix transformation) a string beginning with "[". If the
	 * value is an integer, the body size is expressed in decipoints (72.27
	 * points equal 1 inch). "0" if the font is scalable.
	 */
	const char *point_size;

	/**
	 * Horizontal resolution (unsigned integer), measured in pixels or dots
	 * per inch (DPI), for which the font was designed. "0" if the font is
	 * scalable.
	 */
	const char *resolution_x;

	/**
	 * Vertical resolution (unsigned integer), measured in pixels or dots
	 * per inch (DPI), for which the font was designed. "0" if the font is
	 * scalable.
	 */
	const char *resolution_y;

	/**
	 * Escapement class of the font
	 * 
	 * "P" = Proportional
	 *         Logical character widths vary for each glyph
	 * 
	 * "M" = Monospace
	 *         Logical character widths are constant
	 * 
	 * "C" = Character cell
	 *         Monospaced font where no glyph have "ink" outside of
	 *         the character cell, and there is no kerning and
	 *         vertical extents of the font do not exceed the vertical
	 *         spacing. The cell height = font-descent + font-ascent,
	 *         and the cell width = the average width.
	 */
	const char *spacing;

	/**
	 * Unweighted arithmetic mean of the absolute value of the width of each
	 * glyph, measured in decipixels, and encoded as an integer. Multiplied
	 * by −1 if the dominant writing direction for the font is right-to-left.
	 * "0" if the font is scalable.
	 * 
	 * Negative values are encoding with a leading "~", which will be
	 * converted to "-" when parsed.
	 */
	const char *average_width;

	/**
	 * X-registered authority the character set encoding is
	 * standardised by
	 */
	const char *charset_registry;

	/**
	 * The character set encoding
	 * 
	 * Unicode shall be represented with the combnation of "iso10646"
	 * (any case) in `.charset_registry` and `1` in `.charset_encoding`
	 */
	const char *charset_encoding;

	/**
	 * Character subset hint, or `NULL` if none
	 * 
	 * Will be a sorted, in ascending order, <space>-separated list of
	 * inclusive ranges included the subset. A range will either be a
	 * decimal integer, or an two decimal integers separated by a "-"
	 * (the left-hand side will be strictly less than the right-hand side).
	 * All ranges will be disjunctive, and have a non-zero distance.
	 * 
	 * The library will take care of converting hexadecimal numbers (prefixed
	 * with "0x") to decimal, removing leading zeroes, converting "_" to "-",
	 * and sorting the range, reducing singleton ranges to single numbers,
	 * and merge mergable ranges. Ranges including negative values and values
	 * 0x80000000 (1 exceeded the maximum value of an unsigned 31-bit integer,
	 * which is less than the smallest number (0xE8D4A51000 = 1000000000000)
	 * whose prefixed hexadecimal representation is shorter than it's decimal
	 * representation) or greater, will be rejected.
	 * 
	 * This information is encoded in the character set encoding field,
	 * but the libraries separates out this information to its own
	 * field in this `struct`.
	 */
	const char *charset_subset;

	/**
	 * "-"-separated list of unrecognised fields, `NULL` if none.
	 * Note that the empty string indicates that there was one
	 * unrecognised fields, containing the empty string.
	 * 
	 * These fields will not be parsed in any way, meaning
	 * for example that negative values encoded will a leading
	 * "~" will retain the "~" uncoverted.
	 */
	const char *unrecognised_fields;

	/**
	 * For internal use only
	 * 
	 * Store the strings in the struct. Limited to 256
	 * bytes because a font name may only included up
	 * to 255 bytes; add one for NUL-termination.
	 */
	char _buf[256];
};

/**
 * Structure that can be used to spoof the
 * environment the library is executed in,
 * as well as the print warnings from the
 * library
 */
struct libfonts_context {
	/**
	 * Set to `LIBFONTS_CONTEXT_VERSION`
	 */
	int version;

	/**
	 * Non-zero to remove all environment
	 * variables that are not included in
	 * `.environ`
	 */
	int ignore_process_environ;

	/**
	 * Environment variables to add or override
	 *
	 * `libfonts_used_environs` can be used to
	 * enumerate the environment variables that
	 * affect the execution of the environment
	 */
	char **environ;

	/**
	 * Whether `.uid` is used
	 */
	int use_context_uid;

	/**
	 * The real user ID of the process
	 *
	 * Only used if `.use_context_uid` is non-zero
	 */
	uid_t uid;

	/**
	 * Application data used in as the last argument
	 * in callback functions
	 */
	void *user;

	/**
	 * If non-`NULL` some function may call thing
	 * function to let the application print a
	 * warning
	 * 
	 * @param  err       `errno` value for the warning, 0 if none
	 * @param  function  The name of the function the warning is emitted from (closest public function)
	 * @param  fmt       Formattable description of the warning, will start in lower case,
	 *                   and will end with a colon if and only if a description of `err`
	 *                   shall be appended to the message (with a preceding space)
	 * @param  args      Argument used into the description via formatting
	 * @param  user      `.user` as is
	 */
	void (*warning)(int err, const char *function, const char *fmt, va_list args, void *user);
};


#if defined(__clang__)
# pragma clang diagnostic pop
#endif


/**
 * `NULL`-terminated list of environment variables
 * that affect the execution of the library
 */
extern const char *const libfonts_used_environs[];


/**
 * Get a list of all font root directories, for example
 * /usr/share/fonts and /usr/local/share/fonts, but not
 * their subdirectories
 * 
 * @param   dirsp   Output parameter for the directory list;
 *                  note that directories are not necessarily unique
 *                  (however no two returned strings will be identical)
 *                  nor do they necessarily exist
 * @param   countp  Output parameter for the number of listed directories
 * @param   ctx     Optional `struct libfonts_context`
 * @return          0 on success, -1 on failure
 * 
 * Unless `*dirsp == NULL` (only happens on failure), `(*dirsp)[*dirsp] == NULL`
 */
int libfonts_get_font_root_dirs(char ***, size_t *, struct libfonts_context *);

/**
 * Parse line from a font alias file
 * 
 * Font alias files are named "fonts.alias" (`LIBFONTS_ALIAS_FILE_NAME`)
 * and are located in any font directory, i.e. directories returned by
 * `libfonts_get_font_root_dirs` subdirectors (and further decedents)
 * 
 * @param   typep   Output parameter fot the line's type
 * @param   aliasp  Output parameter for the new alias
 * @param   namep   Output parameter for the alias target,
 *                  which can be ether a proper XLFD (font name),
 *                  a XLFD with wildcards, or another alias
 * @param   line    The line to parse; parsing stops at the first
 *                  newline or NUL byte
 * @param   endp    Output parameter for the parsing end, i.e. the
 *                  first newline or NUL byte in `line`
 * @return          0 on success, -1 on failure
 * 
 * @throws  ENOMEM   Failed to allocate memory for `*aliasp` or `*namep`
 * @throws  EINVAL   `line` is `NULL`
 * 
 * Unless `*typep` is set to (or would be set to) `LIBFONTS_ALIAS_LINE_ALIAS_DEFINITION`,
 * `*aliasp` and `*namep` are set to `NULL`. On failure, `*aliasp` and `*namep` are set
 * to `NULL` and `*typep` is set to `LIBFONTS_ALIAS_LINE_BLANK`. `*endp` is updated even
 * on failure.
 */
int libfonts_parse_alias_line(enum libfonts_alias_line_type *, char **, char **, const char *, char **);

/**
 * Parse any other than the first line in a
 * font enumeration file
 * 
 * Font enumeration files are named either "fonts.dir"
 * (`LIBFONTS_DIR_FILE_NAME`; for full enumeration) or
 * "fonts.scale" (`LIBFONTS_SCALE_DIR_FILE_NAME`, for
 * enumeration of scalable fonts only), and are located
 * in any font directory, i.e. directories returned by
 * `libfonts_get_font_root_dirs` subdirectors (and
 * further decedents)
 * 
 * The first line the file is a decimally formatted
 * non-negative number of entries in the file, and
 * should be ignored; and it cannot be parsed by this
 * function
 * 
 * @param   filep  Output parameter for the font file name (basename with suffix)
 * @param   namep  Output parameter for the font's XLFD
 * @param   line   The line to parse; parsing stops at the first
 *                 newline or NUL byte
 * @param   endp   Output parameter for the parsing end, i.e. the
 *                 first newline or NUL byte in `line`
 * @return         0 on success, -1 on failure
 * 
 * @throws  ENOMEM        Failed to allocate memory for `*filep` or `*namep`
 * @throws  EINVAL        `line` is `NULL`
 * @throws  (unmodified)  Malformatted line (possibly the first line in the file)
 * @throws  (unmodified)  Listed font file name contains a slash
 * 
 * On failure, `*filep` and `*namep` are set to `NULL`.
 * `*endp` is updated even on failure.
 *
 * Alternatively this function can be used to enumerate
 * encodings from a "encodings.dir" (`LIBFONTS_ENCODING_DIR_FILE_NAME`),
 * where the only difference will be that `filep` will be the output
 * parameter for an encoding file (rather than a font file), which
 * is usually suffixed ".enc.gz" and the character-set registry–
 * character-set encoding pair used in XLFD, with a hyphen separating
 * them, rather than a full XLFD
 */
int libfonts_parse_dir_line(char **, char **, const char *, char **);

/**
 * Sets if a specific character is in a subset of
 * of character set
 * 
 * @param   c       The character to test; it is assumed that
 *                  it is a member of the complete character set
 * @param   subset  The subset (of the complete character set)
 *                  to test against; shall be formatted as
 *                  `struct libfonts_font_description.charset_subset`
 * @return          1 if the character is a member of the subset,
 *                  or `subset` is `NULL`, 0 otherwise; -1 on failure
 * 
 * @throws  EINVAL  If `c` is equal to greater than 0x80000000
 * @throws  EINVAL  If `subset` is invalid
 */
int libfonts_char_is_in_subset(uint32_t, const char *);

/* TODO add font listing */


/**
 * Get the font a default font name aliases to
 * 
 * @param   font  The default font
 * @param   ctx   Optional `struct libfonts_context`
 * @return        The font the default font is an alias for
 *                (shall be deallocated using free(3) when no
 *                longer used), or `NULL` on failure; in the
 *                case that no font is found, `NULL` with be
 *                returned without modifying `errno`
 * 
 * @throws  EINVAL  Unrecognised value on `font`
 */
char *libfonts_get_default_font(enum libfonts_default_font, struct libfonts_context *);

/**
 * Get the alias for a default font
 * 
 * @param   font   The default font
 * @param   index  The index of the alias (some default fonts
 *                 have multiple aliases)
 * @param   namep  Output parameter for the font name
 * @return         If the alias exists, or 0 if `index` is equal to
 *                 or greater than the number of aliases, or
 *                 -1 on failure
 * 
 * @throws  EINVAL  Unrecognised value on `font`
 */
int libfonts_get_default_font_name(enum libfonts_default_font, size_t, const char **);

/**
 * Get a default font a font name represents
 * 
 * @param  font   Output parameter for the default font
 * @param  name   The alias for the default font, optionally
 *                with style and size information
 * @param  style  Output parameter for the offset in `name`
 *                where the style and size information begins;
 *                will be set to 0 if this information is
 *                excluded from `name`
 * @return        1 on successful completion, 0 if `name` is not
 *                an alias for a default font
 */
int libfonts_get_default_font_by_name(enum libfonts_default_font *, const char *, size_t *);
/* TODO implement libfonts_get_default_font_by_name */


/**
 * Encode an X Logical Font Description (XLFD) string
 *
 * @param   desc  The font description to encode
 * @param   out   Output buffer for the font description string
 * @return        0 on successful completion, -1 on failure
 * 
 * @throws  EINVAL  Invalid font description
 */
int libfonts_encode_font_description(const struct libfonts_font_description *, char[static 256]);

/**
 * Decode an X Logical Font Description (XLFD) string
 *
 * @param   desc  Output parameter for the font description
 * @param   in    The font description string to decode
 * @return        0 on successful completion, -1 on failure
 * 
 * @throws  EINVAL  Invalid font description
 */
int libfonts_decode_font_description(struct libfonts_font_description *, const char *);

/**
 * Check if an X Logical Font Description (XLFD) matches
 * a font specification
 * 
 * @param   desc  The font description to test, may not include wildcards
 * @param   spec  The specification to test the description against, may include wildcards
 * @return        1 if the arguments match, 0 if they don't match
 */
int libfonts_do_font_descriptions_match(const char *, const char *);

/**
 * Check if an X Logical Font Description (XLFD) matches
 * a font specification
 * 
 * @param   desc  The font description to test, may not include wildcards
 * @param   spec  The specification to test the description against, may include wildcards
 * @return        1 if the arguments match, 0 if they don't match
 */
int libfonts_do_decoded_font_descriptions_match(const struct libfonts_font_description *,
                                                const struct libfonts_font_description *);


/**
 * Get the rendering settings to use for output devices
 * without any specific rendering settings configured
 *
 * Applications that are not output-aware shall use
 * these settings
 * 
 * @param   settings  Output parameter for the rendering settings;
 *                    `settings->dpi_x` and `settings->dpi_y` will
 *                    have a fallback value assigned if configured to 0
 * @param   ctx       Optional `struct libfonts_context`
 * @return            1 if rendering settings where found,
 *                    0 otherwise (`*settings` will be filled with fallback values),
 *                    -1 on failure
 */
int libfonts_get_default_rendering_settings(struct libfonts_rendering_settings *, struct libfonts_context *);

/**
 * Get rendering settings specified for a specific output device
 * 
 * @param   settings  Output parameter for the rendering settings
 * @param   name      The EDID (or other identifier) of the output
 * @param   ctx       Optional `struct libfonts_context`
 * @return            1 if rendering settings where found,
 *                    0 otherwise (`*settings` will be unmodified),
 *                    -1 on failure
 * 
 * @throws  EINVAL  `name` is `NULL`
 */
int libfonts_get_output_rendering_settings(struct libfonts_rendering_settings *, const char *, struct libfonts_context *);


/**
 * Get an approximate pixel density for an output device
 * 
 * The returned pixel density applied to the applied
 * screen resolution, but without any rotation
 *
 * The output pixel density is only approximate because
 * the EDID has limited dimension resolution
 * 
 * @param   output  Output device information, `.unrotated_output_width`,
 *                  `.unrotated_output_height`, and `.output_transformation` must
 *                  already be set; this function will set `.dpi_x` and `.dpi_y`
 *                  (both will be set to zero if the function returns zero)
 * @param   edid    The output device's EDID, in hexadecimal representation;
 *                  if `NULL`, `output->unrotated_output_width` and
 *                  `output->unrotated_output_height` need not be set, instead
 *                  `output->dpi_x` and `output->dpi_y` must be set to the pixel
 *                  density before the inverse of `output->output_transformation`
 *                  is applied
 * @return          1 if a pixel density was calculated,
 *                  0 otherwise (projector, unsupported EDID, or
 *                  non-invertable output transformation)
 */
int libfonts_get_output_dpi(struct libfonts_output *, const char *);

/**
 * Calculate the subpixel order on an output device after
 * a transformation is applied on its output
 * 
 * @param   unrotated       The device's subpixel order
 * @param   transformation  The applied transformation
 * @return                  The device's logical subpixel order
 */
enum libfonts_subpixel_order libfonts_calculate_subpixel_order(enum libfonts_subpixel_order,
                                                               const struct libfonts_transformation *);

/**
 * Get the general subpixel layout, as well
 * as which primary is contained in which
 * cell in the layout, from a specific
 * subpixel order
 *
 * @param   order   The subpixel order
 * @param   cell1p  Output parameter for the primary contained in cell 1
 * @param   cell2p  Output parameter for the primary contained in cell 2
 * @param   cell3p  Output parameter for the primary contained in cell 3
 * @return          The subpixel layout
 */
enum libfonts_subpixel_order_class libfonts_get_subpixel_order_class(enum libfonts_subpixel_order,
                                                                     enum libfonts_subpixel_colour *,
                                                                     enum libfonts_subpixel_colour *,
                                                                     enum libfonts_subpixel_colour *);

/**
 * Get a specific subpixel order from the
 * combination of a general subpixel layout
 * and the primaries for each cell in the
 * layout
 *
 * @param   orderp  Output parameter for the subpixel order
 * @param   layout  The subpixel layout
 * @param   cell1   The primary contained in cell 1
 * @param   cell2   The primary contained in cell 2
 * @param   cell3   The primary contained in cell 3
 * @return          Normally 1, but 0 if `layout` is `LIBFONTS_SUBPIXEL_ORDER_CLASS_OTHER`;
 *                  -1 on failure
 * 
 * @throws  EINVAL  Unrecognised value in `layout`, `cell1`, `cell2`, or `cell3`
 * @throws  EINVAL  The values of `cell1`, `cell2`, and `cell3` are not unique
 */
int libfonts_unget_subpixel_order_class(enum libfonts_subpixel_order *,
                                        enum libfonts_subpixel_order_class,
                                        enum libfonts_subpixel_colour,
                                        enum libfonts_subpixel_colour,
                                        enum libfonts_subpixel_colour);


#endif
