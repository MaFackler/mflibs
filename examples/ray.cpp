#include <mf.h>
#define MF_MATH_IMPLEMENTATION
#include <mf_math.h>

typedef mfm_v3 v3;


struct ray {
   v3 origin; 
   v3 direction;
};

void print_color(v3 c) {
    i32 r = (int) (255.999 * c.r);
    i32 g = (int) (255.999 * c.g);
    i32 b = (int) (255.999 * c.b);
    printf("%d %d %d\n", r, g, b);
}

bool hit_sphere(v3 center, float radius, ray r) {
    v3 oc = r.origin - center;
    float a = mfm_v3_dot(r.direction, r.direction);
    float b = 2.0f * mfm_v3_dot(oc, r.direction);
    float c = mfm_v3_dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return discriminant > 0.0f;
}


inline
v3 ray_color(ray r) {
    if (hit_sphere(v3{0.0f, 0.0f, -1}, 0.5f, r))
        return v3{1.0f, 1.0f, 0.0f};
    v3 unit_direction = mfm_v3_normalize(r.direction);
    float t = 0.5 * (unit_direction.y + 1.0f);
    return v3{1.0f, 1.0f, 1.0f} * (1.0f-t) + v3{0.5, 0.7f, 1.0f} * t;
}

int main() {
    const float aspect_ratio = 16.0f / 9.0f;
    const u32 image_width = 1600;
    const u32 image_height = image_width / aspect_ratio;

    float viewport_height = 2.0f;
    float viewport_width = aspect_ratio * viewport_height;
    float focal_length = 1.0f;

    v3 origin = {0.0f, 0.0f, 0.0f};
    v3 horizontal = v3{viewport_width, 0.0f, 0.0f};
    v3 vertical = v3{0.0f, viewport_height, 0.0f};
    v3 lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - v3{0, 0, focal_length};

    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (i32 y = image_height - 1; y >= 0; --y) {
        fprintf(stderr, "\rRemaining: %d", y);
        fflush(stderr);
        for (i32 x = 0; x < image_width; ++x) {
            float u = float(x) / (float) (image_width - 1);
            float v = float(y) / (float) (image_height - 1);
            ray r = {origin, lower_left_corner + horizontal * u + vertical * v - origin};
            v3 color = ray_color(r);
            print_color(color);
        }
    }
}
