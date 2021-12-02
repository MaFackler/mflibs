#define MF_VECTOR_IMPLEMENTATION
#include "../src/mf_vector.h"

TEST("mf_vec") {
    int *arr = NULL;
    MFT_CHECK_INT(mf_vec_size(arr), 0);
    MFT_CHECK_INT(mf__stretchy_capacity(arr), 0);

    mf_vec_push(arr, 1);
    MFT_CHECK_INT(mf_vec_size(arr), 1);
    MFT_CHECK_INT(mf__stretchy_capacity(arr), 2);

    mf_vec_push(arr, 2);
    MFT_CHECK_INT(mf_vec_size(arr), 2);
    MFT_CHECK_INT(mf__stretchy_capacity(arr), 2);

    *mf_vec_add(arr) = 3;
    MFT_CHECK_INT(mf_vec_size(arr), 3);
    MFT_CHECK_INT(mf__stretchy_capacity(arr), 6);

    MFT_CHECK_INT(arr[0], 1);
    MFT_CHECK_INT(arr[1], 2);
    MFT_CHECK_INT(arr[2], 3);

    mf_vec_for(arr) {
        //printf("%d\n", *it); 
    }

    mf_vec_clear(arr);
    MFT_CHECK_INT(mf_vec_size(arr), 0);
}
