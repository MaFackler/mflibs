#include <mf.h>
#define MF_MATH_IMPLEMENTATION
#include <mf_math.h>

typedef mfm_v3 v3;

struct hit_record {
    v3 point;
    v3 normal;
    double t;
};

struct ray {
   v3 origin; 
   v3 direction;
};

struct sphere {
    v3 center;
    double radius;
};

double sphere_hit(sphere *s, ray r) {
    v3 oc = r.origin - s->center;
    float a = mfm_v3_length_squared(r.direction);
    float half_b = mfm_v3_dot(oc, r.direction);
    float c = mfm_v3_length_squared(oc) - s->radius * s->radius;
    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return -1;
    } else {
        return (- half_b - sqrt(discriminant)) / a;
    }
}

v3 ray_at(ray *r, double t) {
    return r->origin + r->direction * t;
}

void print_color(v3 c) {
    i32 r = (int) (255.999 * c.r);
    i32 g = (int) (255.999 * c.g);
    i32 b = (int) (255.999 * c.b);
    printf("%d %d %d\n", r, g, b);
}



inline
v3 ray_color(ray r) {
    sphere s = {v3{0, 0, -1}, 0.5};
    double t = sphere_hit(&s, r);
    if (t > 0.0) {
        v3 n = mfm_v3_normalize(ray_at(&r, t) - v3{0, 0, -1});
        return v3{n.x + 1, n.y + 1, n.z + 1} * 0.5;
    }
    v3 unit_direction = mfm_v3_normalize(r.direction);
    t = 0.5 * (unit_direction.y + 1.0f);
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
