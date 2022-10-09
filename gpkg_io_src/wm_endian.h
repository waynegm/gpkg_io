#pragma once

#include <stdint.h>

enum class Endian : uint8_t
{
    Big = 0,
    Little = 1
};

inline Endian SysEndian()
{
    union {
	uint32_t i;
	uint8_t  c[4];
    } test = {0x01020304};

    return test.c[0]==1 ? Endian::Big : Endian::Little;
}


