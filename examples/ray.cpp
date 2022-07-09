#include <mf.h>
#define MF_VECTOR_IMPLEMENTATION
#include <mf_vector.h>
#define MF_MATH_IMPLEMENTATION
#include <mf_math.h>

typedef mfm_v3 v3;

double T_MAX = 100000000;

struct ray {
   v3 origin; 
   v3 direction;
};

v3 ray_at(ray *r, double t) {
    return r->origin + r->direction * t;
}

struct hit_record {
    v3 point;
    v3 normal;
    double t;
    bool front_face;
};

inline void hit_record_set_face_normal(hit_record *h, ray r, v3 outward_normal) {
    h->front_face = mfm_v3_dot(r.direction, outward_normal) < 0;
    h->normal = h->front_face ? outward_normal : mfm_v3_negate(outward_normal);
}


struct sphere {
    v3 center;
    double radius;
};

struct hittable {
    sphere *spheres = NULL;
};

bool sphere_hit(sphere *s, ray r, double t_min, double t_max, hit_record *record) {
    v3 oc = r.origin - s->center;
    float a = mfm_v3_length_squared(r.direction);
    float half_b = mfm_v3_dot(oc, r.direction);
    float c = mfm_v3_length_squared(oc) - s->radius * s->radius;
    double discriminant = half_b * half_b - a * c;

    if (discriminant < 0) {
        return false;
    }

    double sqrtd = sqrt(discriminant);

    double root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    record->t = root;
    record->point = ray_at(&r, record->t);
    v3 outward_normal = (record->point - s->center) / s->radius;
    hit_record_set_face_normal(record, r, outward_normal);
    return true;
}


void print_color(v3 c) {
    i32 r = (int) (255.999 * c.r);
    i32 g = (int) (255.999 * c.g);
    i32 b = (int) (255.999 * c.b);
    printf("%d %d %d\n", r, g, b);
}



inline
v3 ray_color(ray r, hittable *word) {
    hit_record record = {0};
    mf_vec_for (word->spheres) {
        bool hit = sphere_hit(it, r, 0, T_MAX, &record);
        if (hit) {
            return v3{record.normal.x + 1, record.normal.y + 1, record.normal.z + 1} * 0.5;
        }
    }
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

    hittable word = {0};
    mf_vec_push(word.spheres, ((sphere){v3{0, 0, -1}, 0.5}));
    mf_vec_push(word.spheres, ((sphere){v3{0, -100.5, -1}, 100}));

    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (i32 y = image_height - 1; y >= 0; --y) {
        fprintf(stderr, "\rRemaining: %d", y);
        fflush(stderr);
        for (i32 x = 0; x < image_width; ++x) {
            float u = float(x) / (float) (image_width - 1);
            float v = float(y) / (float) (image_height - 1);
            ray r = {origin, lower_left_corner + horizontal * u + vertical * v - origin};
            v3 color = ray_color(r, &word);
            print_color(color);
        }
    }
}
