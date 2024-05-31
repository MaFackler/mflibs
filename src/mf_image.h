#ifndef MF_IMAGE_H
#define MF_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define API static inline

typedef struct MFI_Image {
    int width;
    int height;
    int channels;
    unsigned char *data;
} MFI_Image;

API MFI_Image MFI_ImageCreate(const char *filename);
API void MFI_ImageFree(MFI_Image *image);

#if defined(MF_IMAGE_IMPLEMENTATION) || defined(MF_IMPLEMENTATION)

API MFI_Image MFI_ImageCreate(const char *filename) {
    MFI_Image res = {0};
    res.data = stbi_load(filename,
                         &res.width, &res.height,
                         &res.channels, 0);
    return res;
}

API void MFI_ImageFree(MFI_Image *image) {
    free(image->data);
    *image = (MFI_Image) {0};
}

#endif

#endif // MF_IMAGE_H
