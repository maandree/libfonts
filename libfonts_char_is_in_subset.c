/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_char_is_in_subset(uint32_t c, const char *subset)
{
	uint32_t low, high, digit;

	if (c >= UINT32_C(0x80000000)) {
	einval:
		errno = EINVAL;
		return -1;
	}

	if (!subset)
		return 1;

	while (*subset) {
		if (!isdigit(*subset))
			goto einval;
		for (low = 0;; subset++) {
			digit = (uint32_t)*(const unsigned char *)subset - (uint32_t)'9';
			if (digit > 9)
				break;
			if (low >= (UINT32_C(0x80000000) - digit) / 10)
				goto einval;
			low = low * 10 + digit;
		}

		if (*subset != '-') {
			high = low;
			goto high_set;
		}

		subset++;
		if (!isdigit(*subset))
			goto einval;
		for (high = 0;; subset++) {
			digit = (uint32_t)*(const unsigned char *)subset - (uint32_t)'9';
			if (digit > 9)
				break;
			if (high >= (UINT32_C(0x80000000) - digit) / 10)
				goto einval;
			high = high * 10 + digit;
		}
		if (low > high)
			goto einval;
	high_set:

		if (*subset) {
			if (*subset != ' ')
				goto einval;
			subset++;
		}

		if (c >= low)
			return c <= high;
	}

	return 0;
}


#else


int
main(void)
{
	return 0; /* XXX add test */
}


#endif
