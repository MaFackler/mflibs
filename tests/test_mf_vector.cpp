#include <mf_test_def.h>

#define MF_VECTOR_IMPLEMENTATION
#include <mf_vector.h>

TEST("mf_vec") {
    mf_vec_int arr = NULL;
    CHECK(mf_vec_size(arr), 0);
    CHECK(mf__vec_capacity(arr), 0);

    mf_vec_push(arr, 1);
    CHECK(mf_vec_size(arr), 1);
    CHECK(mf__vec_capacity(arr), 2);

    mf_vec_push(arr, 2);
    CHECK(mf_vec_size(arr), 2);
    CHECK(mf__vec_capacity(arr), 2);

    *mf_vec_add(arr) = 3;
    CHECK(mf_vec_size(arr), 3);
    CHECK(mf__vec_capacity(arr), 6);

    CHECK(arr[0], 1);
    CHECK(arr[1], 2);
    CHECK(arr[2], 3);

    mf_vec_for(arr) {
        //printf("%d\n", *it); 
    }

    mf_vec_clear(arr);
    CHECK(mf_vec_size(arr), 0);
}

