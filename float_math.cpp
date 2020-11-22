#include "float_math.h"

float float_min(float a, float b)
{
    if(a < b)
        return a;
    else
        return b;
}

float float_max(float a, float b)
{
    if(a > b)
        return a;
    else
        return b;
}

float float_abs(float a)
{
    if(a < 0)
        return -a;
    return a;
}

float RandomFloat(float min, float max) {
    float random = ((float) rand()) / (float)INT32_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

float FastSqr( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return 1.0f / y;
}
