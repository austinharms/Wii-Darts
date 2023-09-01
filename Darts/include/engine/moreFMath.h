#ifndef DARTS_MORE_F_MATH_H_
#define DARTS_MORE_F_MATH_H_
#include "Core.h"
WD_NODISCARD static WD_INLINE float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

WD_NODISCARD static WD_INLINE float clamp(float x, float min, float max) {
	if (x > max) return max;
	if (x < min) return min;
	return x;
}
#endif // !DARTS_MORE_F_MATH_H_
