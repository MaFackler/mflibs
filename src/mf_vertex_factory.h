#ifndef MF_VERTEX_FACTORY
#define MF_VERTEX_FACTORY

#define API static inline

typedef struct MFV_VertexF3F2 {
    float pos[3];
    float uv[2];
} MFV_VertexF3F2;

typedef struct MFV_IndicesTriangle {
    unsigned int a;
    unsigned int b;
    unsigned int c;
} MFV_IndicesTriangle;


API MFV_IndicesTriangle* MFV_IndicesCubeCreate(unsigned int *nindices);
API MFV_VertexF3F2* MFV_VertexF3F2CubeCreate(unsigned int *nvertices);

#if defined(MF_VERTEX_FACTORY_IMPLEMENTATION) || defined(MF_IMPLEMENTATION)

API MFV_IndicesTriangle* MFV_IndicesCubeCreate(unsigned int *nindices) {
    *nindices = 12;
    MFV_IndicesTriangle *res = malloc(sizeof(MFV_IndicesTriangle) * (*nindices));
    res[0] = (MFV_IndicesTriangle) {0, 1, 2};
    res[1] = (MFV_IndicesTriangle) {0, 2, 3};
    res[2] = (MFV_IndicesTriangle) {4, 5, 6};
    res[3] = (MFV_IndicesTriangle) {4, 6, 7};
    res[4] = (MFV_IndicesTriangle) {8, 9, 10};
    res[5] = (MFV_IndicesTriangle) {8, 10, 11};
    res[6] = (MFV_IndicesTriangle) {12, 13, 14};
    res[7] = (MFV_IndicesTriangle) {12, 14, 15};
    res[8] = (MFV_IndicesTriangle) {16, 17, 18};
    res[9] = (MFV_IndicesTriangle) {16, 18, 19};
    res[10] = (MFV_IndicesTriangle) {20, 21, 22};
    res[11] = (MFV_IndicesTriangle) {20, 22, 23};
    return res;
}

API MFV_VertexF3F2* MFV_VertexF3F2CubeCreate(unsigned int *nvertices) {
    float s = 0.5f;
    *nvertices = 24;
    MFV_VertexF3F2 *res = malloc(sizeof(MFV_VertexF3F2) * (*nvertices));
    res[0] = (MFV_VertexF3F2) {-s, -s, s, 0.0f, 0.0f};
    res[1] = (MFV_VertexF3F2) {s, -s, s, 1.0f,  0.0f};
    res[2] = (MFV_VertexF3F2) {s, s, s, 1.0f, 1.0f};
    res[3] = (MFV_VertexF3F2) {-s, s, s, 0.0f, 1.0f};
        // back
    res[4] = (MFV_VertexF3F2) {s, -s, -s, 0.0f, 0.0f};
    res[5] = (MFV_VertexF3F2) {-s, -s, -s, 1.0f, 0.0f};
    res[6] = (MFV_VertexF3F2) {-s, s, -s, 1.0f, 1.0f};
    res[7] = (MFV_VertexF3F2) {s, s, -s, 0.0f, 1.0f};
        // left
    res[8] = (MFV_VertexF3F2) {-s, -s, -s, 0.0f, 0.0f};
    res[9] = (MFV_VertexF3F2) {-s, -s, s, 1.0f, 0.0f};
    res[10] = (MFV_VertexF3F2) {-s, s, s, 1.0f, 1.0f};
    res[11] = (MFV_VertexF3F2) {-s, s, -s, 0.0f, 1.0f};
        // right
    res[12] = (MFV_VertexF3F2) {s, -s, s, 0.0f, 0.0f};
    res[13] = (MFV_VertexF3F2) {s, -s, -s, 1.0f, 0.0f};
    res[14] = (MFV_VertexF3F2) {s, s, -s, 1.0f, 1.0f};
    res[15] = (MFV_VertexF3F2) {s, s, s, 0.0f, 1.0f};
        // top
    res[16] = (MFV_VertexF3F2) {-s, s, s, 0.0f, 0.0f};
    res[17] = (MFV_VertexF3F2) {s, s, s, 1.0f, 0.0f};
    res[18] = (MFV_VertexF3F2) {s, s, -s, 1.0f, 1.0f};
    res[19] = (MFV_VertexF3F2) {-s, s, -s, 0.0f, 1.0f};
        // bottom
    res[20] = (MFV_VertexF3F2) {-s, -s, -s, 0.0f, 0.0f};
    res[21] = (MFV_VertexF3F2) {s, -s, -s, 1.0f, 0.0f};
    res[22] = (MFV_VertexF3F2) {s, -s, s, 1.0f, 1.0f};
    res[23] = (MFV_VertexF3F2) {-s, -s, s, 0.0f, 1.0f};
    return res;
}

#endif // IMPLEMENTATION

#endif // MF_VERTEX_FACTORY
