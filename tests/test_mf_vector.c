#include <mf_test.h>

#include <mf_vector.h>

TEST(MF_Vec) {
    int* arr = NULL;
    CHECK_EQ(MF_VecLen(arr), 0);
    CHECK_EQ(MF_VecCapacity(arr), 0);

    MF_VecPush(arr, 1);
    CHECK_EQ(MF_VecLen(arr), 1);
    CHECK_EQ(MF_VecCapacity(arr), 2);

    MF_VecPush(arr, 2);
    CHECK_EQ(MF_VecLen(arr), 2);
    CHECK_EQ(MF_VecCapacity(arr), 2);

    *MF_VecAdd(arr) = 3;
    CHECK_EQ(MF_VecLen(arr), 3);
    CHECK_EQ(MF_VecCapacity(arr), 6);

    CHECK_EQ(arr[0], 1);
    CHECK_EQ(arr[1], 2);
    CHECK_EQ(arr[2], 3);

    MF_VecFor(arr) {
        int dummy = *it;
    }

    MF_VecClear(arr);
    CHECK_EQ(MF_VecLen(arr), 0);
}

