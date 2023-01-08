/* See LICENSE file for copyright and license details. */
#include "common.h"
#ifndef TEST


int
libfonts_parse_uint32__(uint32_t *outp, const char *value)
{
	uint32_t out = 0, digit;
	value += *value == '+';
	if (!*value)
		return 0;
	while (isdigit(*value)) {
		digit = (*value++ & 15);
		if (out > (UINT32_C(0xFFFFFFFF) - digit) / 10)
			return 0;
		out = out * 10 + digit;
	}
	if (*value)
		return 0;
	*outp = out;
	return 1;
}


#else


int
main(void)
{
	uint32_t res;

#define F(S)\
	do {\
		res = 999;\
		ASSERT(libfonts_parse_uint32__(&res, S) == 0);\
		ASSERT(res == 999);\
	} while (0)

#define T(N)\
	do {\
		res = (uint32_t)(UINT32_C(N) ^ UINT32_C(0x87654321));\
		ASSERT(libfonts_parse_uint32__(&res, #N) == 1);\
		ASSERT(res == UINT32_C(N));\
		res ^= UINT32_C(0x87654321);\
		ASSERT(libfonts_parse_uint32__(&res, "+"#N) == 1);\
		ASSERT(res == UINT32_C(N));\
		F("-"#N);\
	} while (0)

	T(0);
	T(10);
	T(12);
	T(1235);
	T(53251);
	T(4294967294);
	T(4294967295);

	F("4294967296");
	F("11111111111");
	F("+");
	F("100x");
	F("a");

	return 0;
}


#endif
