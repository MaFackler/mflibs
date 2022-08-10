namespace example_planets {

#define G_CONST 10000.0f

struct Planet {
    v2 pos;
    v2 velocity;
    v3 color;
    float radius, mass;
};

v2 calc_acceleration_to(Planet &p1, Planet &p2) {
    v2 force_direction = (p2.pos - p1.pos).normalize();
    f32 distance_squared = (p2.pos).distance_squared(p1.pos);
    v2 force = force_direction * ((G_CONST * p1.mass * p2.mass) / distance_squared);
    v2 res = force / p1.mass;
    return res;
}


void my_render_bitmap(unsigned int bitmap_id, float x, float y, float w, float h) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    texture_bind(bitmap_id);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y + h);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y + h);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y);
    glEnd();
    texture_bind(0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

struct ExamplePlanets: IExample {

    Planet planets[2];

    void init(IPlatform &p, Renderer2D &r) {
        glEnable(GL_TEXTURE_2D);

        planets[0].pos.x = 800;
        planets[0].pos.y = 450;
        planets[0].mass = 100;
        planets[0].radius = 100;
        planets[0].color = v3{1.0f, 1.0f, 0.0f};

        planets[1].pos.x = 1000;
        planets[1].pos.y = 450;
        planets[1].mass = 100;
        planets[1].radius = 30;
        planets[1].velocity = {0, 80.0f};
        planets[1].color = v3{0.3f, 0.3f, 0.3f};
    }

    void update(IPlatform &p) {
        Planet &p1 = planets[0];
        Planet &p2 = planets[1];

        if (p.input.keys['S'].down) {
            p1.pos.y -= 1.0f;
        }

        v2 acceleration = calc_acceleration_to(p2, p1);
        p2.velocity.x += acceleration.x * p.timer.deltaSec;
        p2.velocity.y += acceleration.y * p.timer.deltaSec;

        for (auto &planet: planets) {
            planet.pos.x += planet.velocity.x * p.timer.deltaSec;
            planet.pos.y += planet.velocity.y * p.timer.deltaSec;
        }
    }

    void render(IPlatform &p, Renderer2D &r) {
        viewport_bottom_up(p.window.width, p.window.height);
        // Render
        // clear
        r.set_color(0.1f, 0.1f, 0.1f, 1.0f);
        r.push_clear();
        // rectangle
        //mfr_set_offset(&renderer, 10.0f, 0.0f);
        //mfr_push_rect(&renderer, 10, 10, 30, 30);
        for (auto &planet: planets) {
            r.set_color(planet.color.x, planet.color.y, planet.color.z, 1.0f);
            r.push_circle(planet.pos.x, planet.pos.y, planet.radius);
            //r.push_rect(planet.pos.x, planet.pos.y, planet.radius, planet.radius);
        }
    }

    void shutdown(IPlatform &p) {
    }
};

}
