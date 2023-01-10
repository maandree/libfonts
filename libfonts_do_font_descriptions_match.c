/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_do_font_descriptions_match(const char *desc, const char *spec)
{
	struct libfonts_font_description desc_decoded;
	struct libfonts_font_description spec_decoded;

	if (!spec)
		return !desc;
	if (!desc)
		return 0;

	if (strlen(desc) > 255 || strlen(spec) > 255) {
	fallback:
		desc_decoded.private_font_name = desc;
		spec_decoded.private_font_name = spec;
	} else {
		if (libfonts_decode_font_description(&desc_decoded, desc) ||
		    libfonts_decode_font_description(&spec_decoded, spec))
			goto fallback;
	}

	return libfonts_do_decoded_font_descriptions_match(&desc_decoded, &spec_decoded);
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
