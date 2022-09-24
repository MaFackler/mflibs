#include <mf.h>
#include <mf_test_def.h>

#if 0
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
#endif

struct Teststruct {
    i32 a;
    i32 b;
    i32 c;
    i32 d;
};

TEST("Struct member offset") {
    Teststruct s;
    CHECK(MF_MemberOffset(s, s.a), 0);
    CHECK(MF_MemberOffset(s, s.b), 4);
    CHECK(MF_MemberOffset(s, s.c), 8);
    CHECK(MF_MemberOffset(s, s.d), 12);
}
