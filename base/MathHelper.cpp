
#include "MathHelper.h"

#include <math.h>

using namespace std;

namespace lava {

bool ispow2(int n)
{
    return ((n & (n-1)) == 0);
}

int nextpow2(int n)
{
    n--;
    n |= n >> 1;  // handle  2 bit numbers
    n |= n >> 2;  // handle  4 bit numbers
    n |= n >> 4;  // handle  8 bit numbers
    n |= n >> 8;  // handle 16 bit numbers
    n |= n >> 16; // handle 32 bit numbers
    n++;
    return n;
}

int safeCeil(float d) 
{
    if (fabs(d-int(d)) < EPSILON) {
        return int(d);
    } else {
        return int(d)+1;
    }
}

float invSqrt(float x)
{
#if 0
    // TODO: This gives incorrect results on Athlon X2, gcc 4.2.
    float xhalf = 0.5f*x;
    int i = *(int*)&x;         // get bits for floating value
    i = 0x5f3759d5 - (i>>1);   // give initial guess y0
    x = *(float*)&i;           // convert bits back to float
    x *= 1.5f - xhalf*x*x;     // newton step, repeating this step
                               // increases accuracy
    x *= 1.5f - xhalf*x*x;
    return x;
#endif
    return 1/sqrt(x);
}

bool almostEqual(float d1, float d2, float epsilon)
{
    return (fabs(d1-d2)<epsilon);
}

}
