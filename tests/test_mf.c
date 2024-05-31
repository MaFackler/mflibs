#include <mf.h>
#include <mf_test.h>

TEST(MF_MAX) {
    CHECK_EQ(MF_Max(-1, 10), 10);
    CHECK_EQ(MF_Max(1, 10), 10);
}

TEST(MF_MIN) {
    CHECK_EQ(MF_Min(-1, 10), -1);
    CHECK_EQ(MF_Min(1, 10), 1);
}

typedef struct Teststruct {
    i32 a;
    i32 b;
    i32 c;
    i32 d;
} Teststruct;

TEST(MF_MemberOffset) {
    Teststruct s;
    CHECK_EQ(MF_MemberOffset(s, s.a), 0);
    CHECK_EQ(MF_MemberOffset(s, s.b), 4);
    CHECK_EQ(MF_MemberOffset(s, s.c), 8);
    CHECK_EQ(MF_MemberOffset(s, s.d), 12);
}
