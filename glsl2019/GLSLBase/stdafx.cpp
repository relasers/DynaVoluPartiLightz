#include "stdafx.h"

std::random_device						g_rn;
std::mt19937_64							g_rnd(g_rn());
std::uniform_int_distribution<int>		g_irange(INT_MIN, INT_MAX);
std::uniform_real_distribution<float>	g_frange(FLT_MIN, FLT_MAX);

int RAND_INT() { return g_irange(g_rnd); }
int RAND_INT(int min, int max) {
	std::uniform_int_distribution<int> irange(min, max);
	return irange(g_rnd);
}
float RAND_FLOAT() { return g_frange(g_rnd); }
float RAND_FLOAT(float min, float max) {
	std::uniform_real_distribution<float> frange(min, max);
	return frange(g_rnd);
}
