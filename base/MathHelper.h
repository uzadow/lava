#pragma once

#define _USE_MATH_DEFINES  // For M_PI
#include <cmath>
#include <cstdint>

namespace lava {

static const float EPSILON = 0.00001f;

bool ispow2(uint32_t n);

uint32_t nextpow2(uint32_t n);

int safeCeil(float d);

bool almostEqual(float d1, float d2, float epsilon=EPSILON);

float invSqrt(float x);

template<class T>
T sqr(T d)
{
    return d*d;
}

template<class T>
int sgn(T val)
{
    return int(val/fabs(val));
}


}
