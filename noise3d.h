#ifndef NOISE3D_H
#define NOISE3D_H

#include <math.h>

static inline float noise3d(float x, float y, float z){
    x = x * 2 - 1;
    y = y * 2 - 1;
    z = z * 2 - 1;

    float d = sqrtf(x*x + y*y + z*z) / sqrtf(2);
    d = 1.0 - d;
    d = d * 2 - 1;

    return d;
}

#endif
