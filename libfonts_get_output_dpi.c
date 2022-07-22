/* See LICENSE file for copyright and license details. */
#include "common.h"


int
libfonts_get_output_dpi(struct libfonts_output *output, const char *edid)
{
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

	transform(&x[0], &y[0], 0, 0, &output->output_transformation);
	transform(&x[1], &y[1], output->dpi_x, output->dpi_y, &output->output_transformation);

	output->dpi_x = x[1] > x[0] ? x[1] - x[0] : x[0] - x[1];
	output->dpi_y = y[1] > y[0] ? y[1] - y[0] : y[0] - y[1];

	return 1;
}
