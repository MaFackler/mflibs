#include "mf_lib.h"
MF_LIB_LINUX("m")

#include <assert.h>
#include <mf.h>
#define MF_IMPLEMENTATION
#include <mf_vector.h>
#define MF_MATH_SHORT_NAMES
#include <mf_math.h>
#include <mf_random.h>
#include <limits.h>


#define T_MAX INT_MAX

typedef enum MaterialType {
    MATERIAL_TYPE_LAMBERTIAN,
    MATERIAL_TYPE_METAL,
    MATERIAL_TYPE_DIALECTRIC,
} MaterialType;


typedef struct MaterialLambertian {
    Vec3 color;
} MaterialLambertian;

typedef struct MaterialMetal {
    Vec3 color;
    double fuzz;
} MaterialMetal;

typedef struct MaterialDialectic {
    double ir;
} MaterialDialectic;

typedef struct Material {
    MaterialType type;
    union {
        MaterialLambertian lambertian;
        MaterialMetal metal;
        MaterialDialectic dialectic;
    };
} Material;

Material MaterialMakeLambertian(Vec3 color) {
    Material res = {};
    res.type = MATERIAL_TYPE_LAMBERTIAN;
    res.lambertian.color = color;
    return res;
}

Material MaterialMakeMetal(Vec3 color, double fuzz) {
    Material res = {};
    res.type = MATERIAL_TYPE_METAL;
    res.metal.color = color;
    res.metal.fuzz = fuzz;
    return res;
}

Material MaterialMakeDialectric(double ir) {
    Material res = {};
    res.type = MATERIAL_TYPE_DIALECTRIC;
    res.dialectic.ir = ir;
    return res;
}

typedef struct Camera {
    Vec3 origin;
    Vec3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 u, v, w;
    double lens_radius;
} Camera;

double RandomDouble() {
    return MF_RandomDouble(0.0, 1.0);
}

Vec3 v3_random() {
    Vec3 res = (Vec3) {
        RandomDouble(),
        RandomDouble(),
        RandomDouble()
    };
    return res;
}

Vec3 v3_random_min_max(double min, double max) {
    Vec3 res = (Vec3) {
        MF_RandomDouble(min, max),
        MF_RandomDouble(min, max),
        MF_RandomDouble(min, max)
    };
    return res;
}

Vec3 random_in_unit_Sphere() {
    while (true) {
        Vec3 p = v3_random_min_max(-1, 1);
        if (MFM_Vec3LengthSquared(p) >= 1) continue;
        return p;
    }
}

Vec3 random_in_unit_disk() {
    while (true) {
        Vec3 p = (Vec3) {MF_RandomDouble(-1, 1), MF_RandomDouble(-1, 1), 0};
        if (MFM_Vec3LengthSquared(p) >= 1) continue;
        return p;
    }
}


void camera_init(Camera *c, Vec3 lookfrom, Vec3 lookat, Vec3 vup, double vfov, double aspect_ratio, double aperture, double focus_dist) {

    double theta = MFM_DegToRad(vfov);
    double h = tan(theta/2);
    double viewport_height = 2.0 * h;
    double viewport_width = aspect_ratio * viewport_height;

    c->w = MFM_Vec3Normalize(MFM_Vec3Sub(lookfrom, lookat));
    c->u = MFM_Vec3Normalize(MFM_Vec3Cross(vup, c->w));
    c->v = MFM_Vec3Cross(c->w, c->u);

    c->origin = lookfrom;
    c->horizontal = MFM_Vec3Scale(c->u, focus_dist * viewport_width);
    c->vertical = MFM_Vec3Scale(c->v, focus_dist * viewport_height);
    c->lower_left_corner = MFM_Vec3Sub(MFM_Vec3Sub(MFM_Vec3Sub(c->origin, MFM_Vec3Scale(c->horizontal, 1.0f/2.0f)), MFM_Vec3Scale(c->vertical, 1.0f/2.0f)), MFM_Vec3Scale(c->w, focus_dist));
    c->lens_radius = aperture / 2;
}

typedef struct Ray {
   Vec3 origin; 
   Vec3 direction;
} Ray;

Ray camera_get_ray(Camera *c, double s, double t) {
    //Ray r = {c->origin, c->lower_left_corner + c->horizontal * u + c->vertical * v - c->origin};
    Vec3 rd = MFM_Vec3Scale(random_in_unit_disk(), c->lens_radius);
    Vec3 offset = MFM_Vec3Add(MFM_Vec3Scale(c->u, rd.x), MFM_Vec3Scale(c->v, rd.y));
    return (Ray) {
        MFM_Vec3Add(c->origin, offset),
        MFM_Vec3Sub(MFM_Vec3Sub(MFM_Vec3Add(MFM_Vec3Add(c->lower_left_corner, MFM_Vec3Scale(c->horizontal, s)), MFM_Vec3Scale(c->vertical, t)), c->origin), offset)
    };
}


Vec3 ray_at(Ray *r, double t) {
    return MFM_Vec3Add(r->origin, MFM_Vec3Scale(r->direction, t));
}

typedef u32 material_id;

typedef struct HitRecord {
    Vec3 point;
    Vec3 normal;
    double t;
    bool front_face;
    material_id mid;
} HitRecord;

void hit_record_set_face_normal(HitRecord *h, Ray *r, Vec3 outward_normal) {
    h->front_face = MFM_Vec3Dot(r->direction, outward_normal) < 0;
    h->normal = h->front_face ? outward_normal : MFM_Vec3Negate(outward_normal);
}


typedef struct Sphere {
    Vec3 center;
    double radius;
    material_id mid;
} Sphere;

bool Sphere_hit(Sphere *s, Ray *r, double t_min, double t_max, HitRecord *record) {
    Vec3 oc = MFM_Vec3Sub(r->origin, s->center);
    double a = MFM_Vec3LengthSquared(r->direction);
    double half_b = MFM_Vec3Dot(oc, r->direction);
    double c = MFM_Vec3LengthSquared(oc) - s->radius * s->radius;
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
    Vec3 outward_normal = MFM_Vec3Scale(MFM_Vec3Sub(record->point, s->center), 1.0f / s->radius);
    hit_record_set_face_normal(record, r, outward_normal);
    record->mid = s->mid;
    return true;
}

typedef struct World {
    Material *materials;
    Sphere *Spheres;
} World;

bool world_hit(World *myworld, Ray *r, double t_min, double t_max, HitRecord *rec) {
    HitRecord temp_rec = {0};
    bool hit_anything = false;
    double closest_so_far = t_max;

    MF_VecFor(myworld->Spheres) {
        if (Sphere_hit(it, r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }
    return hit_anything;
}


material_id world_add_material(World *myworld, Material m) {
    material_id res = MF_VecLen(myworld->materials);
    MF_VecPush(myworld->materials, m);
    return res;
}

void world_add_Sphere(World *myworld, Vec3 origin, double radius, u32 mid) {
    MF_VecPush(myworld->Spheres, ((Sphere){origin, radius, mid}));
}


void write_color_old(Vec3 c, int samples_per_pixel) {
    double r = c.x;
    double g = c.y;
    double b = c.z;

    double scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // int out_r = 256 * MFM_Clamp(r, 0.0, 0.999);
    // int out_g = 256 * MFM_Clamp(g, 0.0, 0.999);
    // int out_b = 256 * MFM_Clamp(b, 0.0, 0.999);
    // printf("%d %d %d\n", out_r, out_g, out_b);
}

#if 0
void print_color(Vec3 c) {
    i32 r = (int) (255.999 * c.r);
    i32 g = (int) (255.999 * c.g);
    i32 b = (int) (255.999 * c.b);
    printf("%d %d %d\n", r, g, b);
}
#else
void write_color(Vec3 c, int samples_per_pixel) {
    double scale = 1.0f / samples_per_pixel;
    c.x = sqrt(scale * c.x);
    c.y = sqrt(scale * c.y);
    c.z = sqrt(scale * c.z);
    i32 out_r = (int) (255.999 * MFM_Clamp(c.x, 0.0, 0.999));
    i32 out_g = (int) (255.999 * MFM_Clamp(c.y, 0.0, 0.999));
    i32 out_b = (int) (255.999 * MFM_Clamp(c.z, 0.0, 0.999));
    printf("%d %d %d\n", out_r, out_g, out_b);
}
#endif



Vec3 reflect(Vec3 a, Vec3 b) {
    return MFM_Vec3Sub(a, MFM_Vec3Scale(b, MFM_Vec3Dot(a, b) * 2.0));
}

Vec3 refract(Vec3 a, Vec3 b, double etai_over_etat) {
    double cos_thata = fmin(MFM_Vec3Dot(MFM_Vec3Negate(a), b), 1.0f);
    Vec3 r_out_perp = MFM_Vec3Scale(MFM_Vec3Add(a, MFM_Vec3Scale(b, cos_thata)), etai_over_etat);
    Vec3 r_out_parallel = MFM_Vec3Scale(b, -sqrt(fabs(1.0f - MFM_Vec3LengthSquared(r_out_perp))));
    return MFM_Vec3Add(r_out_perp, r_out_parallel);
}

typedef bool (*scatter_func)(Ray *r, HitRecord *rec, Material *m, Vec3 *attenuation, Ray *scattered);

bool scatter_mirror(Ray *r, HitRecord *rec, Material *m, Vec3 *attenuation, Ray *scattered) {
    Vec3 reflected = reflect(MFM_Vec3Normalize(r->direction), rec->normal);
    *scattered = (Ray) {rec->point, MFM_Vec3Add(reflected, (MFM_Vec3Scale(random_in_unit_Sphere(), m->metal.fuzz)))};
    *attenuation = m->metal.color;
    return MFM_Vec3Dot(scattered->direction, rec->normal) > 0;
}

bool scatter_lambertian(Ray *r, HitRecord *rec, Material *m, Vec3 *attenuation, Ray *scattered) {
    Vec3 scatter_direction = MFM_Vec3Add(rec->point, MFM_Vec3Add(rec->normal, random_in_unit_Sphere()));
    if (MFM_Vec3NearZero(scatter_direction))
        scatter_direction = rec->normal;
    *scattered = (Ray) {rec->point, scatter_direction};
    *attenuation = m->lambertian.color;
    return true;
}

double reflectance(double cosine, double ref_idx) {
    double r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool scatter_dialectric(Ray *r, HitRecord *rec, Material *m, Vec3 *attenuation, Ray *scattered) {
    *attenuation = (Vec3) {1.0, 1.0, 1.0};
    double refraction_ratio = rec->front_face ? (1.0/m->dialectic.ir) : m->dialectic.ir;
    Vec3 unit_direction = MFM_Vec3Normalize(r->direction);
    double cos_theta = fmin(MFM_Vec3Dot(MFM_Vec3Negate(unit_direction), rec->normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    Vec3 direction = {};
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > RandomDouble())
        direction = reflect(unit_direction, rec->normal);
    else
        direction = refract(unit_direction, rec->normal, refraction_ratio);

    *scattered = (Ray) {rec->point, direction};
    return true;
}

Vec3 ray_color(Ray r, World *myworld, int depth) {
    HitRecord record = {0};
    if (depth <= 0)
        return (Vec3) {0};

    bool hit = world_hit(myworld, &r, 0.0001, T_MAX, &record);
    if (hit) {
        Ray scattered = {0};
        Vec3 attenuation = {0};
        Material *m = &myworld->materials[record.mid];
        scatter_func func = NULL;
        if (m->type == MATERIAL_TYPE_LAMBERTIAN) {
            func = scatter_lambertian;
        } else if (m->type == MATERIAL_TYPE_METAL) {
            func = scatter_mirror;
        } else if (m->type == MATERIAL_TYPE_DIALECTRIC) {
            func = scatter_dialectric;
        }
        if (func && func(&r, &record, m, &attenuation, &scattered)) {
            return MFM_Vec3Mul(attenuation, ray_color(scattered, myworld, depth - 1));
        }
        return (Vec3) {0};
    }
    Vec3 unit_direction = MFM_Vec3Normalize(r.direction);
    double t = 0.5 * (unit_direction.y + 1.0f);
    return MFM_Vec3Add(MFM_Vec3Scale((Vec3) {1.0f, 1.0f, 1.0f}, (1.0f-t)), MFM_Vec3Scale((Vec3) {0.5, 0.7f, 1.0f}, t));
}

void init_random_world(World *myworld) {
    Material material_ground = MaterialMakeLambertian((Vec3){0.5, 0.5, 0.5});
    material_id material_ground_id = world_add_material(myworld, material_ground);
    world_add_Sphere(myworld, (Vec3){0, -1000, 0}, 1000, material_ground_id);

    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            double choose_mat = RandomDouble();
            Vec3 center = (Vec3) {a + 0.9 * RandomDouble(), 0.2, b + 0.9 * RandomDouble()};

            if (MFM_Vec3Length(MFM_Vec3Sub(center, (Vec3) {4, 0.2, 0})) > 0.9) {
                if (choose_mat < 0.8) {
                    Material material_temp = MaterialMakeLambertian(v3_random());
                    material_id material_temp_id = world_add_material(myworld, material_temp);
                    world_add_Sphere(myworld, center, 0.2, material_temp_id);
                } else if (choose_mat < 0.95) {
                    Material material_temp = MaterialMakeMetal(v3_random(), 0.5);
                    material_id material_temp_id = world_add_material(myworld, material_temp);
                    world_add_Sphere(myworld, center, 0.2, material_temp_id);
                } else {
                    Material material_temp = MaterialMakeDialectric(1.5);
                    material_id material_temp_id = world_add_material(myworld, material_temp);
                    world_add_Sphere(myworld, center, 0.2, material_temp_id);
                }
            }
        }
    }

    Material material1 = MaterialMakeDialectric(1.5);
    material_id material1_id = world_add_material(myworld, material1);
    world_add_Sphere(myworld, (Vec3) {0, 1, 0}, 1.0, material1_id);

    Material material2 = MaterialMakeLambertian((Vec3) {0.4, 0.2, 0.1});
    material_id material2_id = world_add_material(myworld, material2);
    world_add_Sphere(myworld, (Vec3) {-4, 1, 0}, 1.0, material2_id);

    Material material3 = MaterialMakeMetal((Vec3) {0.7, 0.6, 0.5}, 0.0);
    material_id material3_id = world_add_material(myworld, material3);
    world_add_Sphere(myworld, (Vec3) {4, 1, 0}, 1.0, material3_id);
}


int main() {
    const float aspect_ratio = 16.0f / 9.0f;
    const u32 image_width = 900;
    const u32 image_height = image_width / aspect_ratio;
    const int samples_per_pixel = 2;
    const int max_depth = 50;

    World myworld = {0};
#if 0
    Material material_ground = MaterialMakeLambertian({0.8, 0.8, 0.0});
    Material material_center = MaterialMakeLambertian({0.1, 0.2, 0.5});
    Material material_left = MaterialMakeDialectric(1.5);
    Material material_right = MaterialMakeMetal({0.8, 0.6, 0.2}, 0.0);

    auto material_ground_id = world_add_material(&myworld, material_ground);
    auto material_center_id = world_add_material(&myworld, material_center);
    auto material_left_id = world_add_material(&myworld, material_left);
    auto material_right_id = world_add_material(&myworld, material_right);

    world_add_Sphere(&myworld, Vec3{0, -100.5, -1}, 100, material_ground_id);
    world_add_Sphere(&myworld, Vec3{0, 0, -1}, 0.5, material_center_id);
    world_add_Sphere(&myworld, Vec3{-1, 0, -1}, -0.4, material_left_id);
    world_add_Sphere(&myworld, Vec3{-1, 0, -1}, 0.5, material_left_id);
    world_add_Sphere(&myworld, Vec3{1, 0, -1}, 0.5, material_right_id);
#else
    init_random_world(&myworld);
#endif

    Camera cam = {0};
    Vec3 lookfrom = (Vec3) {13, 2, 3};
    Vec3 lookat = (Vec3) {0, 0, 0};
    Vec3 vup = (Vec3) {0, 1, 0};
    double dist_to_focus = 10.0;
    double aperture = 0.1;
    camera_init(&cam, lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (i32 y = image_height - 1; y >= 0; --y) {
        fprintf(stderr, "\rRemaining: %d       ", y);
        fflush(stderr);
        for (i32 x = 0; x < image_width; ++x) {
            Vec3 color = (Vec3) {0.0, 0.0, 0.0};
            for (int s = 0; s < samples_per_pixel; ++s) {
                double u = ((double) x + RandomDouble()) / (double) (image_width - 1);
                double v = ((double) y + RandomDouble()) / (double) (image_height - 1);
                Ray r = camera_get_ray(&cam, u, v);
                color = MFM_Vec3Add(color, ray_color(r, &myworld, max_depth));
            }
            write_color(color, samples_per_pixel);
        }
    }
}
