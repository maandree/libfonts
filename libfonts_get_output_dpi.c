/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static int
invert(struct libfonts_transformation *out, const struct libfonts_transformation *in)
{
	double m[3][6], t;
	int i, j;

#define SWAP_ROWS(A, B)\
	do {\
		for (i = 0; i < 6; i++) {\
			t = m[A][i];\
			m[A][i] = m[B][i];\
			m[B][i] = t;\
		}\
	} while (0)

	/* Copy in-matrix and augment with identity-matrix */
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			m[i][j] = in->m[i][j];
			m[i][j + 3] = 0;
		}
		m[i][i + 3] = 1;
	}

	/* Set m[0][0] ≠ 0 */
	if (eq(m[0][0], 0)) {
		if (!eq(m[1][0], 0))
			SWAP_ROWS(0, 1);
		else if (!eq(m[2][0], 0))
			SWAP_ROWS(0, 2);
		else
			return 0;
	}
	/* Set m[0][0] = 1 */ for (t = m[0][0], i = 0; i < 6; i++)  m[0][i] /= t;
	/* Set m[1][0] = 0 */ for (t = m[1][0], i = 0; i < 6; i++)  m[1][i] -= t * m[0][i];
	/* Set m[2][0] = 0 */ for (t = m[2][0], i = 0; i < 6; i++)  m[2][i] -= t * m[0][i];

	/* Set m[1][1] ≠ 0 */
	if (eq(m[1][1], 0)) {
		if (!eq(m[2][1], 0))
			SWAP_ROWS(1, 2);
		else
			return 0;
	}
	/* Set m[1][1] = 1 */ for (t = m[1][1], i = 1; i < 6; i++)  m[1][i] /= t;
	/* Set m[2][1] = 0 */ for (t = m[2][1], i = 1; i < 6; i++)  m[2][i] -= t * m[1][i];
	/* Set m[0][1] = 0 */ for (t = m[0][1], i = 1; i < 6; i++)  m[0][i] -= t * m[1][i];

	/* Set m[2][2] ≠ 0 */
	if (eq(m[2][2], 0))
		return 0;
	/* Set m[2][2] = 1 */ for (t = m[2][2], i = 2; i < 6; i++)  m[2][i] /= t;
	/* Set m[1][2] = 0 */ for (t = m[1][2], i = 2; i < 6; i++)  m[1][i] -= t * m[2][i];
	/* Set m[0][2] = 0 */ for (t = m[0][2], i = 2; i < 6; i++)  m[0][i] -= t * m[2][i];

	/* Output augment */
	for (i = 0; i < 3; i++) {
		out->m[i][0] = m[i][3];
		out->m[i][1] = m[i][4];
		out->m[i][2] = m[i][5];
	}

	return 1;
}


int
libfonts_get_output_dpi(struct libfonts_output *output, const char *edid)
{
	struct libfonts_transformation invtrans;
	int width, height;
	char width1, width2, height1, height2;
	double x[4], y[4], c1, c2, c3, c4;

	if (edid) {
		output->dpi_x = 0;
		output->dpi_y = 0;

		if (strncasecmp(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2) || strlen(edid) < 256)
			return 0;

		width1 = edid[21 * 2 + 0];
		width2 = edid[21 * 2 + 1];
		height1 = edid[22 * 2 + 0];
		height2 = edid[22 * 2 + 1];

		if (!isxdigit(width1) || !isxdigit(width2) || !isxdigit(height1) || !isxdigit(height2))
			return 0;

		width  = ((width1  & 15) + (width1  > '9' ? 9 : 0)) << 4;
		width  |= (width2  & 15) + (width2  > '9' ? 9 : 0);
		height = ((height1 & 15) + (height1 > '9' ? 9 : 0)) << 4;
		height |= (height2 & 15) + (height2 > '9' ? 9 : 0);

		if (!width || !height)
			return 0;

		output->dpi_x = (double)output->unrotated_output_width  / (double)width  * 254 / 100;
		output->dpi_y = (double)output->unrotated_output_height / (double)height * 254 / 100;
	}

	if (!invert(&invtrans, &output->output_transformation)) {
		output->dpi_x = 0;
		output->dpi_y = 0;
		return 0;
	}

	transform(&x[0], &y[0], 0, 0, &invtrans);
	transform(&x[1], &y[1], output->dpi_x, 0, &invtrans);
	transform(&x[2], &y[2], 0, output->dpi_y, &invtrans);
	transform(&x[3], &y[3], output->dpi_x, output->dpi_y, &invtrans);

	c1 = x[0] > x[1] ? x[0] - x[1] : x[1] - x[0];
	c2 = x[0] > x[2] ? x[0] - x[2] : x[2] - x[0];
	c3 = x[3] > x[1] ? x[3] - x[1] : x[1] - x[3];
	c4 = x[3] > x[2] ? x[3] - x[2] : x[2] - x[3];
	c1 = c1 > c2 ? c1 : c2;
	c3 = c3 > c4 ? c3 : c4;
	output->dpi_x = c1 > c3 ? c1 : c3;

	c1 = y[0] > y[1] ? y[0] - y[1] : y[1] - y[0];
	c2 = y[0] > y[2] ? y[0] - y[2] : y[2] - y[0];
	c3 = y[3] > y[1] ? y[3] - y[1] : y[1] - y[3];
	c4 = y[3] > y[2] ? y[3] - y[2] : y[2] - y[3];
	c1 = c1 > c2 ? c1 : c2;
	c3 = c3 > c4 ? c3 : c4;
	output->dpi_y = c1 > c3 ? c1 : c3;

	return 1;
}


#else


int
main(void)
{
#define T_(MAT, XOUT, YOUT, RET)\
	do {\
		output.dpi_x = 100;\
		output.dpi_y = 150;\
		memcpy(&output.output_transformation, &MAT, sizeof(struct libfonts_transformation));\
		ASSERT(libfonts_get_output_dpi(&output, NULL) == RET);\
		ASSERT(eq(output.dpi_x, XOUT));\
		ASSERT(eq(output.dpi_y, YOUT));\
	} while (0)

#define T(MAT, XOUT, YOUT) T_(MAT, XOUT, YOUT, 1)
#define ASIS(MAT) T(MAT, 100, 150)
#define SWAPS(MAT) T(MAT, 150, 100)

#if defined(__GNUC__) && !defined(__clang__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunsuffixed-float-constants"
#endif

	const double sqrt1half = 0.7071067811865475;

#if defined(__GNUC__) && !defined(__clang__)
# pragma GCC diagnostic pop
#endif

	char edid[512];
	struct libfonts_output output;
	struct libfonts_transformation asis_matrix = {.m = {
			{+1,  0, 0},
			{ 0, +1, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation rot90cw_matrix = {.m = {
			{ 0, -1, 0},
			{+1,  0, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation rot180cw_matrix = {.m = {
			{-1,  0, 0},
			{ 0, -1, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation rot270cw_matrix = {.m = {
			{ 0, +1, 0},
			{-1,  0, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation flip_matrix = {.m = {
			{+1,  0, 0},
			{ 0, -1, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation flop_matrix = {.m = {
			{-1,  0, 0},
			{ 0, +1, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation transpose_matrix = {.m = {
			{ 0, +1, 0},
			{+1,  0, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation antitranspose_matrix = {.m = {
			{ 0, -1, 0},
			{-1,  0, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation xdouble_matrix = {.m = {
			{+2,  0, 0},
			{ 0, +1, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation ydouble_matrix = {.m = {
			{+1,  0, 0},
			{ 0, +2, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation xydouble_matrix = {.m = {
			{+2,  0, 0},
			{ 0, +2, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation xtranslate_matrix = {.m = {
			{+1,  0, 1},
			{ 0, +1, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation ytranslate_matrix = {.m = {
			{+1,  0, 0},
			{ 0, +1, 1},
			{ 0,  0, 1}}};
	struct libfonts_transformation xytranslate_matrix = {.m = {
			{+1,  0, 1},
			{ 0, +1, 1},
			{ 0,  0, 1}}};
	struct libfonts_transformation xshear_matrix = {.m = {
			{+1, +1, 0},
			{ 0, +1, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation yshear_matrix = {.m = {
			{+1,  0, 0},
			{+1, +1, 0},
			{ 0,  0, 1}}};
	struct libfonts_transformation rot45cw_matrix = {.m = {
			{+sqrt1half, -sqrt1half, 0},
			{+sqrt1half, +sqrt1half, 0},
			{ 0,          0,         1}}};
	struct libfonts_transformation uninvertable_matrix = {.m = {
			{1, 1, 0},
			{1, 1, 0},
			{0, 0, 1}}};
	struct libfonts_transformation null_matrix = {.m = {
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0}}};

	ASIS(asis_matrix);
	ASIS(rot180cw_matrix);
	ASIS(flip_matrix);
	ASIS(flop_matrix);
	ASIS(xtranslate_matrix);
	ASIS(ytranslate_matrix);
	ASIS(xytranslate_matrix);

	SWAPS(rot90cw_matrix);
	SWAPS(rot270cw_matrix);
	SWAPS(transpose_matrix);
	SWAPS(antitranspose_matrix);

	T(xdouble_matrix, 50, 150);
	T(ydouble_matrix, 100, 75);
	T(xydouble_matrix, 50, 75);

	T(xshear_matrix, 150, 150); /* not important */
	T(yshear_matrix, 100, 150); /* not important */

	T(rot45cw_matrix, 150 * sqrt1half, 150 * sqrt1half); /* not important */

	T_(uninvertable_matrix, 0, 0, 0);
	T_(null_matrix, 0, 0, 0);

	output.unrotated_output_width = 400;
	output.unrotated_output_height = 500;

	output.dpi_x = 100;
	output.dpi_y = 150;
	strcpy(edid, "00""FF""FF""FF""FF""FF""FF""00");
	memcpy(&output.output_transformation, &asis_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 0);
	ASSERT(eq(output.dpi_x, 0));
	ASSERT(eq(output.dpi_y, 0));

	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = 'A';
	edid[21 * 2 + 1] = 'A';
	edid[22 * 2 + 0] = 'A';
	edid[22 * 2 + 1] = 'A';
	edid[255] = '\0';
	memcpy(&output.output_transformation, &asis_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 0);
	ASSERT(eq(output.dpi_x, 0));
	ASSERT(eq(output.dpi_y, 0));

	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = 'A';
	edid[21 * 2 + 1] = 'A';
	edid[22 * 2 + 0] = 'A';
	edid[22 * 2 + 1] = 'A';
	edid[254] = '\0';
	memcpy(&output.output_transformation, &asis_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 0);
	ASSERT(eq(output.dpi_x, 0));
	ASSERT(eq(output.dpi_y, 0));

	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = 'X';
	edid[21 * 2 + 1] = 'X';
	edid[22 * 2 + 0] = 'X';
	edid[22 * 2 + 1] = 'X';
	edid[256] = '\0';
	memcpy(&output.output_transformation, &asis_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 0);
	ASSERT(eq(output.dpi_x, 0));
	ASSERT(eq(output.dpi_y, 0));

	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = 'A';
	edid[21 * 2 + 1] = 'A';
	edid[22 * 2 + 0] = '0';
	edid[22 * 2 + 1] = '0';
	edid[256] = '\0';
	memcpy(&output.output_transformation, &asis_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 0);
	ASSERT(eq(output.dpi_x, 0));
	ASSERT(eq(output.dpi_y, 0));

	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = '0';
	edid[21 * 2 + 1] = '0';
	edid[22 * 2 + 0] = 'A';
	edid[22 * 2 + 1] = 'A';
	edid[256] = '\0';
	memcpy(&output.output_transformation, &asis_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 0);
	ASSERT(eq(output.dpi_x, 0));
	ASSERT(eq(output.dpi_y, 0));

	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = '0';
	edid[21 * 2 + 1] = '0';
	edid[22 * 2 + 0] = '0';
	edid[22 * 2 + 1] = '0';
	edid[256] = '\0';
	memcpy(&output.output_transformation, &asis_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 0);
	ASSERT(eq(output.dpi_x, 0));
	ASSERT(eq(output.dpi_y, 0));

	output.unrotated_output_width = 400;
	output.unrotated_output_height = 500;
	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = 'A';
	edid[21 * 2 + 1] = 'A';
	edid[22 * 2 + 0] = 'A';
	edid[22 * 2 + 1] = 'A';
	edid[256] = '\0';
	memcpy(&output.output_transformation, &asis_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 1);
	ASSERT(eq(output.dpi_x, (double)254 / 100 * 400 / 0xAA));
	ASSERT(eq(output.dpi_y, (double)254 / 100 * 500 / 0xAA));

	output.unrotated_output_width = 400;
	output.unrotated_output_height = 500;
	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = '9';
	edid[21 * 2 + 1] = '9';
	edid[22 * 2 + 0] = '8';
	edid[22 * 2 + 1] = '8';
	edid[256] = '\0';
	memcpy(&output.output_transformation, &asis_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 1);
	ASSERT(eq(output.dpi_x, (double)254 / 100 * 400 / 0x99));
	ASSERT(eq(output.dpi_y, (double)254 / 100 * 500 / 0x88));

	output.unrotated_output_width = 400;
	output.unrotated_output_height = 500;
	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = '9';
	edid[21 * 2 + 1] = '9';
	edid[22 * 2 + 0] = '8';
	edid[22 * 2 + 1] = '8';
	edid[256] = '\0';
	memcpy(&output.output_transformation, &rot90cw_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 1);
	ASSERT(eq(output.dpi_y, (double)254 / 100 * 400 / 0x99));
	ASSERT(eq(output.dpi_x, (double)254 / 100 * 500 / 0x88));

	output.unrotated_output_width = 400;
	output.unrotated_output_height = 500;
	output.dpi_x = 100;
	output.dpi_y = 150;
	memset(edid, '0', sizeof(edid));
	memcpy(edid, "00""FF""FF""FF""FF""FF""FF""00", 8 * 2);
	edid[21 * 2 + 0] = '9';
	edid[21 * 2 + 1] = '9';
	edid[22 * 2 + 0] = '8';
	edid[22 * 2 + 1] = '8';
	edid[400] = '\0';
	memcpy(&output.output_transformation, &rot90cw_matrix, sizeof(struct libfonts_transformation));
	ASSERT(libfonts_get_output_dpi(&output, edid) == 1);
	ASSERT(eq(output.dpi_y, (double)254 / 100 * 400 / 0x99));
	ASSERT(eq(output.dpi_x, (double)254 / 100 * 500 / 0x88));

	return 0;
}


#endif
