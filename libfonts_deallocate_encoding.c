/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


void
libfonts_deallocate_encoding(struct libfonts_encoding *this)
{
	if (this) {
		if (this->mappings) {
			while (this->nmappings)
				libfonts_deallocate_encoding_mapping(&this->mappings[--this->nmappings]);
			free(this->mappings);
			this->mappings = NULL;
		}
		if (this->aliases) {
			while (this->naliases)
				free(this->aliases[--this->naliases]);
			free(this->aliases);
			this->aliases = NULL;
		}
		free(this->name);
		this->name = NULL;
	}
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
