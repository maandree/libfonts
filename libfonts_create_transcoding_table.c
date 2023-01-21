/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


static int
set_range(uint32_t low, uint32_t high, uint32_t target, uint32_t target_inc, struct libfonts_transcoding_table *table)
{
	uint32_t i;

	if (low < table->first_valid) {
		if (target != LIBFONTS_UNDEFINED_CHAR)
			return -1;
		low = table->first_valid;
	}

	if (high > table->last_valid) {
		if (target != LIBFONTS_UNDEFINED_CHAR)
			return -1;
		high = table->last_valid;
	}

	i = low;
	do {
		LIBFONTS_LOOKUP_TRANSCODING_TABLE(table, i) = target;
		target += target_inc;
	} while (i++ < high);

	return 0;
}

struct libfonts_transcoding_table *
libfonts_create_transcoding_table(const struct libfonts_encoding *encoding, size_t mapping)
{
	const struct libfonts_encoding_mapping *map;
	size_t size, entry;
	uint16_t size_rows, size_cols;
	uint8_t first_row, first_col;
	struct libfonts_transcoding_table *ret;
	uint32_t i;
	int r;

	if (!encoding ||
	    mapping > encoding->nmappings ||
	    !encoding->mappings ||
	    (encoding->mappings[mapping].nentries && !encoding->mappings[mapping].entries) ||
	    !encoding->size_rows)
		goto einvalid;

	map = &encoding->mappings[mapping];
	size_rows = encoding->size_rows;
	size_cols = encoding->size_cols;
	first_row = encoding->first_index_row;
	first_col = encoding->first_index_col;

#if SIZE_MAX <= UINT32_MAX
	if ((size_t)size_rows - (size_t)first_row > SIZE_MAX / (size_cols ? (size_t)size_cols : 1))
		goto enomem;
#endif
	size = ((size_t)size_rows - (size_t)first_row) * (size_cols ? (size_t)size_cols : 1);
	if (size > (SIZE_MAX - offsetof(struct libfonts_transcoding_table, table)) / sizeof(uint32_t))
		goto enomem;
	ret = malloc(size * sizeof(uint32_t) + offsetof(struct libfonts_transcoding_table, table));
	if (!ret)
		goto enomem;

	ret->first_valid = (uint32_t)first_row;
	ret->last_valid = (uint32_t)size_rows - 1;
	if (size_cols) {
		ret->first_valid <<= 8;
		ret->first_valid |= (uint32_t)first_col;
		ret->last_valid <<= 8;
		ret->last_valid |= (uint32_t)size_cols - 1;
	}

	ret->offset = encoding->size_cols ? (uint32_t)first_row << 8 : (uint32_t)first_col;
	ret->multiplier = (uint32_t)size_cols;

	for (i = ret->first_valid; i <= ret->last_valid; i++)
		LIBFONTS_LOOKUP_TRANSCODING_TABLE(ret, i) = i;

	for (entry = 0; entry < map->nentries; entry++) {
		if (map->entries[entry].type == LIBFONTS_ENCODING_MAPPING_ENTRY_UNDEFINED_RANGE) {
			set_range(map->entries[entry].undefined_range.low_source,
			          map->entries[entry].undefined_range.high_source,
			          LIBFONTS_UNDEFINED_CHAR, 0, ret);

		} else if (map->entries[entry].type == LIBFONTS_ENCODING_MAPPING_ENTRY_REMAPPED_RANGE) {
			r = set_range(map->entries[entry].remapped_range.low_source,
			              map->entries[entry].remapped_range.high_source,
			              map->entries[entry].remapped_range.low_target, 0, ret);
			if (r)
				goto invalid;

		} else {
			goto invalid;
		}
	}

	return ret;

enomem:
	errno = ENOMEM;
	return NULL;

invalid:
	free(ret);
einvalid:
	errno = EINVAL;
	return NULL;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
