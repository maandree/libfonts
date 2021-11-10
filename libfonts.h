/* See LICENSE file for copyright and license details. */
#ifndef LIBFONTS_H
#define LIBFONTS_H

#include <stdint.h>


enum libfonts_antialiasing {
	LIBFONTS_ANTIALIASING_NONE,
	LIBFONTS_ANTIALIASING_GREYSCALE,
	LIBFONTS_ANTIALIASING_SUBPIXEL,
	LIBFONTS_ANTIALIASING_SUBPIXEL_FORCED
};

enum libfonts_subpixel_order {
	LIBFONTS_SUBPIXEL_ORDER_UNKNOWN,
	LIBFONTS_SUBPIXEL_ORDER_NONRGB,
	LIBFONTS_SUBPIXEL_ORDER_NONLINEAR,
	LIBFONTS_SUBPIXEL_ORDER_OTHER,

	/* For any value V, a rotation of 90⋅R degrees, R integer,
	 * clockwise, of the physical output, will yeild a subpixel
	 * order represented with the value (V + R) Mod 4 + 4⋅⌊V / 4⌋,
	 * if V ≥ 4, otherwise V asis. A rotation that is not
	 * divisible by 90 degrees, will yeild the subpixel order
	 * LIBFONTS_SUBPIXEL_ORDER_NONLINEAR if V is greater or equal
	 * to LIBFONTS_SUBPIXEL_ORDER_NONLINEAR, otherwise V as is */

	/* The reason the subpixel order RG_GB, and similar orders
	 * are excluded from the list is that the green segment of
	 * the pixel is disjoint and therefore not suitable for
	 * subpixel rendering; instead NONLINEAR should is used. */

	LIBFONTS_SUBPIXEL_ORDER_RGB,
	LIBFONTS_SUBPIXEL_ORDER_R_G_B, /* vertically stacked horizontal stripes */
	LIBFONTS_SUBPIXEL_ORDER_BGR,
	LIBFONTS_SUBPIXEL_ORDER_B_G_R,

	LIBFONTS_SUBPIXEL_ORDER_GBR,
	LIBFONTS_SUBPIXEL_ORDER_G_B_R,
	LIBFONTS_SUBPIXEL_ORDER_RBG,
	LIBFONTS_SUBPIXEL_ORDER_R_B_G,

	LIBFONTS_SUBPIXEL_ORDER_BRG,
	LIBFONTS_SUBPIXEL_ORDER_B_R_G,
	LIBFONTS_SUBPIXEL_ORDER_GRB,
	LIBFONTS_SUBPIXEL_ORDER_G_R_B,

	LIBFONTS_SUBPIXEL_ORDER_RR_GB,
	LIBFONTS_SUBPIXEL_ORDER_GR_BR,
	LIBFONTS_SUBPIXEL_ORDER_BG_RR,
	LIBFONTS_SUBPIXEL_ORDER_RB_RG,

	LIBFONTS_SUBPIXEL_ORDER_RR_BG,
	LIBFONTS_SUBPIXEL_ORDER_BR_GR,
	LIBFONTS_SUBPIXEL_ORDER_GB_RR,
	LIBFONTS_SUBPIXEL_ORDER_RG_RB,

	LIBFONTS_SUBPIXEL_ORDER_GG_RB,
	LIBFONTS_SUBPIXEL_ORDER_RG_BG,
	LIBFONTS_SUBPIXEL_ORDER_BR_GG,
	LIBFONTS_SUBPIXEL_ORDER_GB_GR,

	LIBFONTS_SUBPIXEL_ORDER_GG_BR,
	LIBFONTS_SUBPIXEL_ORDER_BG_RG,
	LIBFONTS_SUBPIXEL_ORDER_RB_GG,
	LIBFONTS_SUBPIXEL_ORDER_GR_GB,

	LIBFONTS_SUBPIXEL_ORDER_BB_RG,
	LIBFONTS_SUBPIXEL_ORDER_RB_GB,
	LIBFONTS_SUBPIXEL_ORDER_GR_BB,
	LIBFONTS_SUBPIXEL_ORDER_BG_BR,

	LIBFONTS_SUBPIXEL_ORDER_BB_GR,
	LIBFONTS_SUBPIXEL_ORDER_GB_RB,
	LIBFONTS_SUBPIXEL_ORDER_RG_BB,
	LIBFONTS_SUBPIXEL_ORDER_BR_BG
};

enum libfonts_orientation {
	LIBFONTS_ORIENTATION_UNKNOWN,
	LIBFONTS_ORIENTATION_0_DEGREES_CLOCKWISE,
	LIBFONTS_ORIENTATION_90_DEGREES_CLOCKWISE,
	LIBFONTS_ORIENTATION_180_DEGREES_CLOCKWISE,
	LIBFONTS_ORIENTATION_270_DEGREES_CLOCKWISE,
	LIBFONTS_ORIENTATION_OTHER
};

struct libfonts_rendering_settings {
	double dpi_x; /* actually pixels rather than dots */
	double dpi_y;
	enum libfonts_antialiasing horizontal_grey_text_antialiasing;
	enum libfonts_antialiasing vertical_grey_text_antialiasing;
	enum libfonts_antialiasing diagonal_grey_text_antialiasing;
	enum libfonts_antialiasing horizontal_colour_text_antialiasing;
	enum libfonts_antialiasing vertical_colour_text_antialiasing;
	enum libfonts_antialiasing diagonal_colour_text_antialiasing;
	enum libfonts_subpixel_order unrotated_subpixel_order;
	enum libfonts_orientation default_physical_screen_orientation;
};

struct libfonts_output {
	int32_t output_x;
	int32_t output_y;
	uint32_t output_width;
	uint32_t output_height;
	int output_screen;
	enum libfonts_orientation physical_screen_orientation;
	enum libfonts_subpixel_order unrotated_subpixel_order;
	double dpi_x;
	double dpi_y;
	struct libfonts_rendering_settings *rendering_settings;
};


struct libfonts_font_description {
	/**
	 * The font name unless the foundry is registered with X,
	 * in which case, all following fields will be NULL.
	 * This field is normally NULL.
	 * 
	 * This field will also be used to to store the font name
	 * (including the initial "-") if it is malformated.
	 */
	const char *private_font_name;

	/**
	 * Version if the XLFD used, NULL if not included in the font name
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
	 * Character subset hint, or NULL if none
	 * 
	 * Will be a sorted, in ascending order, <space>-separated list of
	 * inclusive ranges included the subset. A range will either be a
	 * decimal integer, or an two decimal integers separated by a "-"
	 * (the left-hand side will be strictly less than the right-hand side).
	 * All ranges will be disjunction, and have a non-zero distance.
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
	 * "-"-separated list of unrecognised fields, NULL if none.
	 * Note that the empty string indicates that there was one
	 * unrecognised fields, containing the empty string.
	 * 
	 * These fields will not be parsed in any way, meaning
	 * for example that negative values encoded will a leading
	 * "~" will retain the "~" uncoverted.
	 */
	const char *unrecognised_fields;

	/**
	 * For internal use only.
	 * 
	 * Store the strings in the struct. Limited to 256
	 * bytes because a font name may only included up
	 * to 255 bytes; add one for NUL-termination.
	 */
	char _buf[256];
};


int libfonts_encode_font_description(const struct libfonts_font_description *, char[static 256]);
int libfonts_decode_font_description(struct libfonts_font_description *, const char *);
int libfonts_do_font_descriptions_match(const char *, const char *);

int libfonts_get_default_rendering_settings(struct libfonts_rendering_settings *);
int libfonts_get_output_rendering_settings(struct libfonts_rendering_settings *, const char *);
int libfonts_get_output_dpi(struct libfonts_output *, const char *);

enum libfonts_subpixel_order libfonts_calculate_subpixel_order(enum libfonts_subpixel_order, enum libfonts_orientation);


#endif
