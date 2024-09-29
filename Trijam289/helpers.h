#pragma once

inline float Clamp(float x, float min, float max) {
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

inline int Clamp(int x, int min, int max) {
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

inline float Clamp(float x, int min, int max) {
	return Clamp(x, (float)min, (float)max);
}

inline float Dist(float ax, float ay, float bx, float by) {
	float xd = ax - bx;
	float yd = ay - by;
	return sqrtf(xd * xd + yd * yd);
}

inline float Dist(float x, float y) {
	return sqrtf(x * x + y * y);
}