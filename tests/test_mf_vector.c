#include <mf_test.h>

#include <mf_vector.h>

TEST(mf_vec) {
    int* arr = NULL;
    CHECK_EQ(mf_vec_size(arr), 0);
    CHECK_EQ(mf__vec_capacity(arr), 0);

    mf_vec_push(arr, 1);
    CHECK_EQ(mf_vec_size(arr), 1);
    CHECK_EQ(mf__vec_capacity(arr), 2);

    mf_vec_push(arr, 2);
    CHECK_EQ(mf_vec_size(arr), 2);
    CHECK_EQ(mf__vec_capacity(arr), 2);

    *mf_vec_add(arr) = 3;
    CHECK_EQ(mf_vec_size(arr), 3);
    CHECK_EQ(mf__vec_capacity(arr), 6);

    CHECK_EQ(arr[0], 1);
    CHECK_EQ(arr[1], 2);
    CHECK_EQ(arr[2], 3);

    mf_vec_for(arr) {
        int dummy = *it;
    }

    mf_vec_clear(arr);
    CHECK_EQ(mf_vec_size(arr), 0);
}

