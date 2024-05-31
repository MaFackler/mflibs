#ifndef MF_RANDOM_H
#define MF_RANDOM_H
#include <stdlib.h>

#define API static inline

API void MF_RandomInitSeed();
API int MF_RandomInt(int min, int max);
API double MF_RandomDouble(double min, double max);


#if defined(MF_RANDOM_IMPLEMENTATION) || defined(MF_IMPLEMENTATION)

API void MF_RandomInitSeed() {
    srand(time(NULL));
}

API int MF_RandomInt(int min, int max) {
    int res = rand() % (max - min + 1) + min;
    return res;
}

API double MF_RandomDouble(double min, double max) {
    double res = rand() / (RAND_MAX / (max - min));
    return res;
}

#endif

#endif
