#define MF_IMPLEMENTATION
#include "mf.h"



TEST("MF_Max, MF_Min, MF_Clamp") {
    MFT_CHECK_INT(MF_Max(-1, 10), 10);
    MFT_CHECK_INT(MF_Max(1, 10), 10);

    MFT_CHECK_INT(MF_Min(-1, 10), -1);
    MFT_CHECK_INT(MF_Min(1, 10), 1);
}

