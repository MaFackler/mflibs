#include <mf.h>
#include <mf_test_def.h>

TEST("MF_Max") {
    CHECK(MF_Max(-1, 10), 10);
    CHECK(MF_Max(1, 10), 10);
}

TEST("MF_Min") {
    CHECK(MF_Min(-1, 10), -1);
    CHECK(MF_Min(1, 10), 1);
}

TEST("mf_next_power_of_2") {
    CHECK(mf_next_power_of_2(0), 1);
    CHECK(mf_next_power_of_2(7), 8);
    CHECK(mf_next_power_of_2(8), 16);
    CHECK(mf_next_power_of_2(9), 16);
}
