#define MF_TEST_ACTIVE
#include "mf_test.h"


#define MF_IMPLEMENTATION
#include "mf.h"
#define MF_MATH_IMPLEMENTATION
#include "mf_math.h"
#define MF_FILE_IMPLEMENTATION
#include "mf_file.h"
#define MF_STRING_IMPLEMENTATION
#include "mf_string.h"
#define MF_VECTOR_IMPLEMENTATION
#include "mf_vector.h"

int main() {
    mft_run();
}
