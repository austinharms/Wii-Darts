#ifndef DARTS_INTS_H_
#define DARTS_INTS_H_
#include "Defines.h"

typedef signed char int8_t;
WD_STATIC_ASSERT(sizeof(int8_t) == 1, "int8_t was not 1 byte");

typedef unsigned char uint8_t;
WD_STATIC_ASSERT(sizeof(uint8_t) == 1, "uint8_t was not 1 byte");

typedef short int16_t;
WD_STATIC_ASSERT(sizeof(int16_t) == 2, "int16_t was not 2 bytes");

typedef unsigned short uint16_t;
WD_STATIC_ASSERT(sizeof(uint16_t) == 2, "uint16_t was not 2 bytes");

typedef int int32_t;
WD_STATIC_ASSERT(sizeof(int32_t) == 4, "int32_t was not 4 bytes");

typedef unsigned int uint32_t;
WD_STATIC_ASSERT(sizeof(uint32_t) == 4, "uint32_t was not 4 bytes");

typedef long long int64_t;
WD_STATIC_ASSERT(sizeof(int64_t) == 8, "int64_t was not 8 bytes");

typedef unsigned long long uint64_t;
WD_STATIC_ASSERT(sizeof(uint64_t) == 8, "uint64_t was not 8 bytes");

typedef uint32_t size_t;
WD_STATIC_ASSERT(sizeof(size_t) == sizeof(void*), "size_t was not equal to size of void ptr");

typedef int32_t ssize_t;
WD_STATIC_ASSERT(sizeof(ssize_t) == sizeof(void*), "ssize_t was not equal to size of void ptr");
#endif // !DARTS_INTS_H_
