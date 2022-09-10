#include <mf.h>
#include <mf_test_def.h>

TEST("MF_Max") {
    MFT_CHECK_INT(MF_Max(-1, 10), 10);
    MFT_CHECK_INT(MF_Max(1, 10), 10);
}

TEST("MF_Min") {
    MFT_CHECK_INT(MF_Min(-1, 10), -1);
    MFT_CHECK_INT(MF_Min(1, 10), 1);
}

TEST("mf_next_power_of_2") {
    MFT_CHECK_U64(mf_next_power_of_2(0), 1);
    MFT_CHECK_U64(mf_next_power_of_2(7), 8);
    MFT_CHECK_U64(mf_next_power_of_2(8), 16);
    MFT_CHECK_U64(mf_next_power_of_2(9), 16);
}
