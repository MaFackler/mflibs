#pragma once
#include <mf.h>

namespace mf { namespace time {

void sleep_ms(i32 value);

#if defined(MF_TIME_IMPLEMENTATION)

void sleep_ms(i32 value)
{
#ifdef _WIN32
    Sleep(value);
#else
    usleep(value * 1000);
#endif
}

#endif // MF_TIME_IMPLEMENTATION

}} // mf::time
