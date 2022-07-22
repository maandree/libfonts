/* See LICENSE file for copyright and license details. */
#include "common.h"


static int
invert(struct libfonts_transformation *out, const struct libfonts_transformation *in)
{
	double m[3][3], t;
	int i, j;

#define SWAP_ROWS(A, B)\
	do {\
		for (i = 0; i < 6; i++) {\
			t = m[A][i];\
			m[A][i] = m[B][i];\
			m[B][i] = t;\
		}\
	} while (0)

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			m[i][j] = in->m[i][j];
			m[i][j + 3] = 0;
		}
		m[i][i + 3] = 1;
	}

	if (eq(m[0][0], 0)) {
		if (!eq(m[1][0], 0))
			SWAP_ROWS(0, 1);
		else if (!eq(m[2][0], 0))
			SWAP_ROWS(0, 2);
		else
			return 0;
	}
	for (i = 0; i < 6; i++)
		m[0][i] /= m[0][0];
	for (i = 0; i < 6; i++)
		m[1][i] -= m[1][0] * m[0][i];
	for (i = 0; i < 6; i++)
		m[2][i] -= m[2][0] * m[0][i];

	if (eq(m[1][1], 0)) {
		if (!eq(m[2][1], 0))
			SWAP_ROWS(1, 2);
		else
			return 0;
	}
	for (i = 1; i < 6; i++)
		m[1][i] /= m[1][1];
	for (i = 1; i < 6; i++)
		m[2][i] -= m[2][1] * m[1][i];
	for (i = 1; i < 6; i++)
		m[0][i] -= m[0][1] * m[1][i];

	if (eq(m[2][2], 0))
		return 0;
	for (i = 2; i < 6; i++)
		m[2][i] /= m[2][2];
	for (i = 2; i < 6; i++)
		m[1][i] -= m[1][1] * m[2][i];
	for (i = 2; i < 6; i++)
		m[0][i] -= m[0][1] * m[2][i];

	return 1;
}


int
libfonts_get_output_dpi(struct libfonts_output *output, const char *edid)
{
	struct libfonts_transformation invtrans;
	int width, height;
	char width1, width2, height1, height2;
	double x[2], y[2];

	if (edid) {
		output->dpi_x = 0;
		output->dpi_y = 0;

		if (strncasecmp(edid, "00""FF""FF""FF""FF""FF""FF""00", 2 * 8) || strlen(edid) < 256)
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

		output->dpi_x = (double)output->unrotated_output_width  / (double)width  * 2.54;
		output->dpi_y = (double)output->unrotated_output_height / (double)height * 2.54;
	}

	if (!invert(&invtrans, &output->output_transformation)) {
		output->dpi_x = 0;
		output->dpi_y = 0;
		return 0;
	}

	transform(&x[0], &y[0], 0, 0, &invtrans);
	transform(&x[1], &y[1], output->dpi_x, output->dpi_y, &invtrans);

	output->dpi_x = x[1] > x[0] ? x[1] - x[0] : x[0] - x[1];
	output->dpi_y = y[1] > y[0] ? y[1] - y[0] : y[0] - y[1];

	return 1;
}
