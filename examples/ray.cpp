#include <assert.h>
#define MF_IMPLEMENTATION
#include <mf.h>
#define MF_VECTOR_IMPLEMENTATION
#include <mf_vector.h>
#define MF_MATH_IMPLEMENTATION
#include <mf_math.h>
#include <limits>

typedef mfm_v3<double> v3;


double T_MAX = std::numeric_limits<double>::infinity();

enum MaterialType {
    MATERIAL_TYPE_LAMBERTIAN,
    MATERIAL_TYPE_METAL,
    MATERIAL_TYPE_DIALECTRIC,
};


struct material_lambertian {
    v3 color;
};

struct material_metal {
    v3 color;
    double fuzz;
};

struct material_dialectic {
    double ir;
};

struct material {
    MaterialType type;
    union {
        material_lambertian lambertian;
        material_metal metal;
        material_dialectic dialectic;
    };
};

material material_create_lambertian(v3 color) {
    material res = {};
    res.type = MATERIAL_TYPE_LAMBERTIAN;
    res.lambertian.color = color;
    return res;
}

material material_create_metal(v3 color, double fuzz) {
    material res = {};
    res.type = MATERIAL_TYPE_METAL;
    res.metal.color = color;
    res.metal.fuzz = fuzz;
    return res;
}

material material_create_dialectic(double ir) {
    material res = {};
    res.type = MATERIAL_TYPE_DIALECTRIC;
    res.dialectic.ir = ir;
    return res;
}

struct camera {
    v3 origin;
    v3 lower_left_corner;
    v3 horizontal;
    v3 vertical;
    v3 u, v, w;
    double lens_radius;
};

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

v3 random_in_unit_disk() {
    while (true) {
        v3 p = v3{mf_random_double(-1, 1), mf_random_double(-1, 1), 0};
        if (mfm_v3_length_squared(p) >= 1) continue;
        return p;
    }
}


void camera_init(camera *c, v3 lookfrom, v3 lookat, v3 vup, double vfov, double aspect_ratio, double aperture, double focus_dist) {

    double theta = mfm_to_rad(vfov);
    double h = tan(theta/2);
    double viewport_height = 2.0 * h;
    double viewport_width = aspect_ratio * viewport_height;

    c->w = mfm_v3_normalize(lookfrom - lookat);
    c->u = mfm_v3_normalize(mfm_v3_cross(vup, c->w));
    c->v = mfm_v3_cross(c->w, c->u);

    c->origin = lookfrom;
    c->horizontal = focus_dist * viewport_width * c->u;
    c->vertical = focus_dist * viewport_height * c->v;
    c->lower_left_corner = c->origin - c->horizontal / 2.0 - c->vertical / 2.0 - focus_dist * c->w;
    c->lens_radius = aperture / 2;
}

struct ray {
   v3 origin; 
   v3 direction;
};

ray camera_get_ray(camera *c, double s, double t) {
    //ray r = {c->origin, c->lower_left_corner + c->horizontal * u + c->vertical * v - c->origin};
    v3 rd = c->lens_radius * random_in_unit_disk();
    v3 offset = c->u * rd.x + c->v * rd.y;
    return {c->origin + offset, c->lower_left_corner + s * c->horizontal + t * c->vertical - c->origin - offset};
}


v3 ray_at(ray *r, double t) {
    return r->origin + r->direction * t;
}

typedef u32 material_id;

struct hit_record {
    v3 point;
    v3 normal;
    double t;
    bool front_face;
    material_id mid;
};

inline void hit_record_set_face_normal(hit_record *h, ray *r, v3 outward_normal) {
    h->front_face = mfm_v3_dot(r->direction, outward_normal) < 0;
    h->normal = h->front_face ? outward_normal : mfm_v3_negate(outward_normal);
}


struct sphere {
    v3 center;
    double radius;
    material_id mid;
};

bool sphere_hit(sphere *s, ray *r, double t_min, double t_max, hit_record *record) {
    v3 oc = r->origin - s->center;
    double a = mfm_v3_length_squared(r->direction);
    double half_b = mfm_v3_dot(oc, r->direction);
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
    record->point = ray_at(r, record->t);
    v3 outward_normal = (record->point - s->center) / s->radius;
    hit_record_set_face_normal(record, r, outward_normal);
    record->mid = s->mid;
    return true;
}

struct world {
    material *materials = NULL;
    sphere *spheres = NULL;
};

bool world_hit(world *myworld, ray *r, double t_min, double t_max, hit_record *rec) {
    hit_record temp_rec = {0};
    bool hit_anything = false;
    double closest_so_far = t_max;

    mf_vec_for(myworld->spheres) {
        if (sphere_hit(it, r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }
    return hit_anything;
}


material_id world_add_material(world *myworld, material m) {
    material_id res = mf_vec_size(myworld->materials);
    mf_vec_push(myworld->materials, m);
    return res;
}

void world_add_sphere(world *myworld, v3 origin, double radius, u32 mid) {
    mf_vec_push(myworld->spheres, ((sphere){origin, radius, mid}));
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



v3 reflect(v3 a, v3 b) {
    return a - b * mfm_v3_dot(a, b) * 2.0;
}

v3 refract(v3 a, v3 b, double etai_over_etat) {
    double cos_thata = fmin(mfm_v3_dot(mfm_v3_negate(a), b), 1.0f);
    v3 r_out_perp = etai_over_etat * (a + cos_thata * b);
    v3 r_out_parallel = -sqrt(fabs(1.0f - mfm_v3_length_squared(r_out_perp))) * b;
    return r_out_perp + r_out_parallel;
}

typedef bool (*scatter_func)(ray *r, hit_record *rec, material *m, v3 *attenuation, ray *scattered);

bool scatter_mirror(ray *r, hit_record *rec, material *m, v3 *attenuation, ray *scattered) {
    v3 reflected = reflect(mfm_v3_normalize(r->direction), rec->normal);
    *scattered = ray{rec->point, reflected + (random_in_unit_sphere() * m->metal.fuzz)};
    *attenuation = m->metal.color;
    return mfm_v3_dot(scattered->direction, rec->normal) > 0;
}

bool scatter_lambertian(ray *r, hit_record *rec, material *m, v3 *attenuation, ray *scattered) {
    v3 scatter_direction = rec->point + rec->normal + random_in_unit_sphere();
    if (mfm_v3_near_zero(scatter_direction))
        scatter_direction = rec->normal;
    *scattered = ray{rec->point, scatter_direction};
    *attenuation = m->lambertian.color;
    return true;
}

double reflectance(double cosine, double ref_idx) {
    double r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool scatter_dialectric(ray *r, hit_record *rec, material *m, v3 *attenuation, ray *scattered) {
    *attenuation = v3{1.0, 1.0, 1.0};
    double refraction_ratio = rec->front_face ? (1.0/m->dialectic.ir) : m->dialectic.ir;
    v3 unit_direction = mfm_v3_normalize(r->direction);
    double cos_theta = fmin(mfm_v3_dot(mfm_v3_negate(unit_direction), rec->normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    v3 direction = {};
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        direction = reflect(unit_direction, rec->normal);
    else
        direction = refract(unit_direction, rec->normal, refraction_ratio);

    *scattered = ray{rec->point, direction};
    return true;
}

inline
v3 ray_color(ray r, world *myworld, int depth) {
    hit_record record = {0};
    if (depth <= 0)
        return v3{0};

    bool hit = world_hit(myworld, &r, 0.0001, T_MAX, &record);
    if (hit) {
        ray scattered = {0};
        v3 attenuation = {0};
        material *m = &myworld->materials[record.mid];
        scatter_func func = NULL;
        if (m->type == MATERIAL_TYPE_LAMBERTIAN) {
            func = scatter_lambertian;
        } else if (m->type == MATERIAL_TYPE_METAL) {
            func = scatter_mirror;
        } else if (m->type == MATERIAL_TYPE_DIALECTRIC) {
            func = scatter_dialectric;
        }
        if (func && func(&r, &record, m, &attenuation, &scattered)) {
            return attenuation * ray_color(scattered, myworld, depth - 1);
        }
        return v3{0};
    }
    v3 unit_direction = mfm_v3_normalize(r.direction);
    double t = 0.5 * (unit_direction.y + 1.0f);
    return v3{1.0f, 1.0f, 1.0f} * (1.0f-t) + v3{0.5, 0.7f, 1.0f} * t;
}

void init_random_world(world *myworld) {
    material material_ground = material_create_lambertian({0.5, 0.5, 0.5});
    auto material_ground_id = world_add_material(myworld, material_ground);
    world_add_sphere(myworld, {0, -1000, 0}, 1000, material_ground_id);

    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            double choose_mat = random_double();
            v3 center{a + 0.9 * random_double(), 0.2, b + 0.9 * random_double()};

            if (mfm_v3_length(center - v3{4, 0.2, 0}) > 0.9) {
                if (choose_mat < 0.8) {
                    material material_temp = material_create_lambertian(v3_random());
                    auto material_temp_id = world_add_material(myworld, material_temp);
                    world_add_sphere(myworld, center, 0.2, material_temp_id);
                } else if (choose_mat < 0.95) {
                    material material_temp = material_create_metal(v3_random(), 0.5);
                    auto material_temp_id = world_add_material(myworld, material_temp);
                    world_add_sphere(myworld, center, 0.2, material_temp_id);
                } else {
                    material material_temp = material_create_dialectic(1.5);
                    auto material_temp_id = world_add_material(myworld, material_temp);
                    world_add_sphere(myworld, center, 0.2, material_temp_id);
                }
            }
        }
    }

    material material1 = material_create_dialectic(1.5);
    auto material1_id = world_add_material(myworld, material1);
    world_add_sphere(myworld, {0, 1, 0}, 1.0, material1_id);

    material material2 = material_create_lambertian({0.4, 0.2, 0.1});
    auto material2_id = world_add_material(myworld, material2);
    world_add_sphere(myworld, {-4, 1, 0}, 1.0, material2_id);

    material material3 = material_create_metal({0.7, 0.6, 0.5}, 0.0);
    auto material3_id = world_add_material(myworld, material3);
    world_add_sphere(myworld, {4, 1, 0}, 1.0, material3_id);
}


int main() {
    const float aspect_ratio = 16.0f / 9.0f;
    const u32 image_width = 800;
    const u32 image_height = image_width / aspect_ratio;
    const int samples_per_pixel = 100;
    const int max_depth = 50;


    world myworld = {0};
#if 0
    material material_ground = material_create_lambertian({0.8, 0.8, 0.0});
    material material_center = material_create_lambertian({0.1, 0.2, 0.5});
    material material_left = material_create_dialectic(1.5);
    material material_right = material_create_metal({0.8, 0.6, 0.2}, 0.0);

    auto material_ground_id = world_add_material(&myworld, material_ground);
    auto material_center_id = world_add_material(&myworld, material_center);
    auto material_left_id = world_add_material(&myworld, material_left);
    auto material_right_id = world_add_material(&myworld, material_right);

    world_add_sphere(&myworld, v3{0, -100.5, -1}, 100, material_ground_id);
    world_add_sphere(&myworld, v3{0, 0, -1}, 0.5, material_center_id);
    world_add_sphere(&myworld, v3{-1, 0, -1}, -0.4, material_left_id);
    world_add_sphere(&myworld, v3{-1, 0, -1}, 0.5, material_left_id);
    world_add_sphere(&myworld, v3{1, 0, -1}, 0.5, material_right_id);
#else
    init_random_world(&myworld);
#endif

    camera cam = {0};
    v3 lookfrom{13, 2, 3};
    v3 lookat{0, 0, 0};
    v3 vup{0, 1, 0};
    double dist_to_focus = 10.0;
    double aperture = 0.1;
    camera_init(&cam, lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

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
                color = color + ray_color(r, &myworld, max_depth);
            }
            write_color(color, samples_per_pixel);
        }
    }
}
