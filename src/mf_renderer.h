#ifndef MF_RENDERER_H
#define MF_RENDERER_H

#include <stddef.h>
#include <assert.h>

// Signature of render functions
typedef void (func_render_rect)(float x, float y, float width, float height);
typedef void (func_render_circle)(float x, float y, float radius);
typedef void (func_render_set_color)(float r, float g, float b, float a);
typedef void (func_render_bitmap)(unsigned int bitmap_id, float x, float y, float w, float h);
typedef void (func_render_clear)(void);


typedef struct MFR_Renderer MFR_Renderer;
void mfr_init(MFR_Renderer *renderer, size_t memorySize);
void mfr_destroy(MFR_Renderer *renderer);

void mfr_set_color(MFR_Renderer *renderer, float r, float g, float b, float a);
void mfr_set_offset(MFR_Renderer *renderer, float x, float y);
void mfr_reset_offset(MFR_Renderer *renderer);
// Push Commands
void mfr_push_rect(MFR_Renderer *renderer, float x, float y, float width, float height);
void mfr_push_circle(MFR_Renderer *renderer, float x, float y, float radius);
void mfr_push_bitmap(MFR_Renderer *renderer, unsigned int bitmap_id, float x, float y, float w, float h);
void mfr_push_clear(MFR_Renderer *renderer);
// Flush buffer
void mfr_flush(MFR_Renderer *renderer);


enum MFR__RenderCommandType {
    MF__RENDER_COMMAND_TYPE_UNDEFINED,
    MF__RENDER_COMMAND_TYPE_CLEAR,
    MF__RENDER_COMMAND_TYPE_RECT,
    MF__RENDER_COMMAND_TYPE_BITMAP,
    MF__RENDER_COMMAND_TYPE_CIRCLE,
};

typedef struct {
    float r, g, b, a;
    float x, y, width, height;
} mfr__render_command_rect;

typedef struct {
    float r, g, b, a;
    float x, y, radius;
} mfr__render_command_circle;

typedef struct {
    float x, y, w, h;
    unsigned int bitmap_id;
} mfr__render_command_bitmap;

typedef struct {
    float r, g, b, a;
} mfr__render_command_clear;


typedef struct {
    enum MFR__RenderCommandType type;
    union {
        mfr__render_command_rect rect;
        mfr__render_command_circle circle;
        mfr__render_command_bitmap bitmap;
        mfr__render_command_clear clear;
    };

} mfr__render_command;

struct MFR_Renderer {
    func_render_set_color *set_color;
    func_render_clear* render_clear;
    func_render_rect* render_rect;
    func_render_circle* render_circle;
    func_render_bitmap* render_bitmap;

    // NOTE: color and position offset for push commands
    float r, g, b, a;
    float x, y;

    mfr__render_command *commands;
    size_t commandsCount;
    size_t memorySize;

};

#if defined(MF_RENDERER_IMPLEMENTATION) | defined(MF_IMPLEMENTATION)

void mfr_init(MFR_Renderer *renderer, size_t memorySize) {
    renderer->commands = (mfr__render_command*) malloc(memorySize);
    renderer->memorySize = memorySize;
    renderer->commandsCount = 0;
    renderer->r = 0;
    renderer->g = 0;
    renderer->b = 0;
    renderer->a = 1;
    renderer->x = 0;
    renderer->y = 0;
}

void mfr_destroy(MFR_Renderer *renderer) {
    free(renderer->commands);
}

void mfr_set_color(MFR_Renderer *renderer, float r, float g, float b, float a) {
    renderer->r = r;
    renderer->g = g;
    renderer->b = b;
    renderer->a = a;
}

void mfr_set_offset(MFR_Renderer *renderer, float x, float y) {
    renderer->x = x;
    renderer->y = y;
}

void mfr_reset_offset(MFR_Renderer *renderer) {
    mfr_set_offset(renderer, 0.0f, 0.0f);
}

mfr__render_command *mfr__get_next_command(MFR_Renderer *renderer) {
    assert(renderer->memorySize > 0);
    mfr__render_command *res = NULL;
    if ((renderer->commandsCount + 1) * sizeof(mfr__render_command) < renderer->memorySize) {
        res = (renderer->commands + renderer->commandsCount);
        res->type = MF__RENDER_COMMAND_TYPE_UNDEFINED;
        renderer->commandsCount++;
    } else {
        assert(!"Not enough memory");
    }
    return res;

}


void mfr_push_rect(MFR_Renderer *renderer, float x, float y, float width, float height) {
    mfr__render_command *command = mfr__get_next_command(renderer);
    if (command) {
        command->type = MF__RENDER_COMMAND_TYPE_RECT;

        command->rect.r = renderer->r;
        command->rect.g = renderer->g;
        command->rect.b = renderer->b;

        command->rect.x = renderer->x + x;
        command->rect.y = renderer->y + y;
        command->rect.width = width;
        command->rect.height = height;
    }
}

void mfr_push_circle(MFR_Renderer *renderer, float x, float y, float radius) {
    mfr__render_command *command = mfr__get_next_command(renderer);
    if (command) {
        command->type = MF__RENDER_COMMAND_TYPE_CIRCLE;

        command->rect.r = renderer->r;
        command->rect.g = renderer->g;
        command->rect.b = renderer->b;

        command->circle.x = x;
        command->circle.y = y;
        command->circle.radius = radius;
    }
}

void mfr_push_bitmap(MFR_Renderer *renderer, unsigned int bitmap_id, float x, float y, float w, float h) {
    mfr__render_command *command = mfr__get_next_command(renderer);
    if (command) {
        command->type = MF__RENDER_COMMAND_TYPE_BITMAP;

        command->bitmap.x = x;
        command->bitmap.y = y;
        command->bitmap.w = w;
        command->bitmap.h = h;
        command->bitmap.bitmap_id = bitmap_id;
    }
}

void mfr_push_clear(MFR_Renderer *renderer)
{
    mfr__render_command *command = mfr__get_next_command(renderer);
    if (command) {
        command->type = MF__RENDER_COMMAND_TYPE_CLEAR;

        command->clear.r = renderer->r;
        command->clear.g = renderer->g;
        command->clear.b = renderer->b;
        command->clear.a = renderer->a;
    }
}

void mfr_flush(MFR_Renderer *renderer) {
    for (size_t i = 0; i < renderer->commandsCount; ++i) {
        mfr__render_command *command = &renderer->commands[i];
        switch (command->type) {
            case MF__RENDER_COMMAND_TYPE_RECT: {
                renderer->set_color(command->rect.r,
                                    command->rect.g,
                                    command->rect.b,
                                    command->rect.a);

                if (renderer->render_rect)
                    renderer->render_rect(command->rect.x,
                                          command->rect.y,
                                          command->rect.width,
                                          command->rect.height);

            } break;
            case MF__RENDER_COMMAND_TYPE_CLEAR: {
                renderer->set_color(command->clear.r,
                                    command->clear.g,
                                    command->clear.b,
                                    command->clear.a);
                if (renderer->render_clear)
                    renderer->render_clear();
            } break;
            case MF__RENDER_COMMAND_TYPE_CIRCLE: {
                renderer->set_color(command->circle.r,
                                    command->circle.g,
                                    command->circle.b,
                                    command->circle.a);
                renderer->render_circle(command->circle.x,
                                        command->circle.y,
                                        command->circle.radius);
            } break;
            case MF__RENDER_COMMAND_TYPE_BITMAP: {
                if (renderer->render_bitmap)
                    renderer->render_bitmap(command->bitmap.bitmap_id,
                                            command->bitmap.x,
                                            command->bitmap.y,
                                            command->bitmap.w,
                                            command->bitmap.h);

            } break;
            default:
                assert(!"Unhandled command");
        }
    }
    renderer->commandsCount = 0;
}

#endif // MF_RENDERER_IMPLEMENTATION

#endif // MF_RENDERER_H
