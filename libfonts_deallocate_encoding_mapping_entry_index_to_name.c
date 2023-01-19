/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


void
libfonts_deallocate_encoding_mapping_entry_index_to_name(struct libfonts_encoding_mapping_entry_index_to_name *this)
{
	if (this) {
		free(this->target);
		this->target = NULL;
	}
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
