/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


void
libfonts_deallocate_encoding_mapping(struct libfonts_encoding_mapping *this)
{
	if (this) {
		if (this->entries) {
			while (this->nentries)
				libfonts_deallocate_encoding_mapping_entry(&this->entries[--this->nentries]);
			free(this->entries);
			this->entries = NULL;
		}
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
