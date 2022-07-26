#pragma once
#include <mf.h>

namespace mf { namespace random {


double f64_range(f64 min, f64 max);

#if defined(MF_RANDOM_IMPLEMENTATION)


f64 f64_range(f64 min, f64 max)
{
    f64 res = rand() / (RAND_MAX + 1.0);
    res = min + (max - min) * res;
    return res;
}

#endif // defined(MF_RANDOM_IMPLEMENTATION)

}} // mf::random;
