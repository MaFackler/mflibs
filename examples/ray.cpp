#define MF_IMPLEMENTATION
#include <mf.h>
#define MF_VECTOR_IMPLEMENTATION
#include <mf_vector.h>
#define MF_MATH_IMPLEMENTATION
#include <mf_math.h>
#include <limits>

typedef mfm_v3<double> v3;


double T_MAX = std::numeric_limits<double>::infinity();

struct camera {
    v3 origin;
    v3 lower_left_corner;
    v3 horizontal;
    v3 vertical;
};

void camera_init(camera *c, v3 origin, double width, double height) {
    double focal_length = 1.0;
    c->origin = origin;
    c->horizontal = v3{width, 0.0, 0.0};
    c->vertical = v3{0.0, height, 0.0};
    c->lower_left_corner = origin - c->horizontal / 2.0 - c->vertical / 2.0 - v3{0, 0, focal_length};
}

struct ray {
   v3 origin; 
   v3 direction;
};

ray camera_get_ray(camera *c, double u, double v) {
    ray r = {c->origin, c->lower_left_corner + c->horizontal * u + c->vertical * v - c->origin};
    return r;
}


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
    double a = mfm_v3_length_squared(r.direction);
    double half_b = mfm_v3_dot(oc, r.direction);
    double c = mfm_v3_length_squared(oc) - s->radius * s->radius;
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

void write_color_old(v3 c, int samples_per_pixel) {
    double r = c.r;
    double g = c.g;
    double b = c.b;

    double scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    int out_r = 256 * mf_clamp(r, 0.0, 0.999);
    int out_g = 256 * mf_clamp(g, 0.0, 0.999);
    int out_b = 256 * mf_clamp(b, 0.0, 0.999);
    printf("%d %d %d\n", out_r, out_g, out_b);
}

#if 0
void print_color(v3 c) {
    i32 r = (int) (255.999 * c.r);
    i32 g = (int) (255.999 * c.g);
    i32 b = (int) (255.999 * c.b);
    printf("%d %d %d\n", r, g, b);
}
#else
void write_color(v3 c, int samples_per_pixel) {
    double scale = 1.0f / samples_per_pixel;
    c.r = sqrt(scale * c.r);
    c.g = sqrt(scale * c.g);
    c.b = sqrt(scale * c.b);
    i32 out_r = (int) (255.999 * mf_clamp(c.r, 0.0, 0.999));
    i32 out_g = (int) (255.999 * mf_clamp(c.g, 0.0, 0.999));
    i32 out_b = (int) (255.999 * mf_clamp(c.b, 0.0, 0.999));
    printf("%d %d %d\n", out_r, out_g, out_b);
}
#endif


inline double random_double() {
    return mf_random_double(0.0, 1.0);
}

v3 v3_random() {
    v3 res = v3{random_double(), random_double(), random_double()};
    return res;
}

v3 v3_random_min_max(double min, double max) {
    v3 res = v3{mf_random_double(min, max), mf_random_double(min, max), mf_random_double(min, max)};
    return res;
}

v3 random_in_unit_sphere() {
    while (true) {
        v3 p = v3_random_min_max(-1, 1);
        if (mfm_v3_length_squared(p) >= 1) continue;
        return p;
    }
}

inline
v3 ray_color(ray r, hittable *word, int depth) {
    hit_record record = {0};
    if (depth <= 0)
        return v3{0};
    mf_vec_for (word->spheres) {
        bool hit = sphere_hit(it, r, 0, T_MAX, &record);
        if (hit) {
            v3 target = record.point + record.normal + random_in_unit_sphere();
            return ray_color(ray{record.point, target - record.point}, word, depth - 1) * 0.5;
        }
    }
    v3 unit_direction = mfm_v3_normalize(r.direction);
    double t = 0.5 * (unit_direction.y + 1.0f);
    return v3{1.0f, 1.0f, 1.0f} * (1.0f-t) + v3{0.5, 0.7f, 1.0f} * t;
}

int main() {
    const float aspect_ratio = 16.0f / 9.0f;
    const u32 image_width = 400;
    const u32 image_height = image_width / aspect_ratio;
    const int samples_per_pixel = 100;
    const int max_depth = 50;


    float viewport_height = 2.0f;
    float viewport_width = aspect_ratio * viewport_height;
    float focal_length = 1.0f;


    hittable word = {0};
    mf_vec_push(word.spheres, ((sphere){v3{0, 0, -1}, 0.5}));
    mf_vec_push(word.spheres, ((sphere){v3{0, -100.5, -1}, 100}));
    camera cam = {0};
    camera_init(&cam, v3{0.0, 0.0, 0.0}, viewport_width, viewport_height); 

    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (i32 y = image_height - 1; y >= 0; --y) {
        fprintf(stderr, "\rRemaining: %d", y);
        fflush(stderr);
        for (i32 x = 0; x < image_width; ++x) {
            v3 color = v3{0.0, 0.0, 0.0};
            for (int s = 0; s < samples_per_pixel; ++s) {
                double u = ((double) x + random_double()) / (double) (image_width - 1);
                double v = ((double) y + random_double()) / (double) (image_height - 1);
                ray r = camera_get_ray(&cam, u, v);
                color = color + ray_color(r, &word, max_depth);
            }
            write_color(color, samples_per_pixel);
        }
    }
}
