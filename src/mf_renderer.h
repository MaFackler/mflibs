#pragma once
#include <mf.h>

namespace mf { namespace renderer {

// Signature of render functions
typedef void (FuncRenderRect)(float x, float y, float width, float height);
typedef void (FuncRenderCircle)(float x, float y, float radius);
typedef void (FuncRenderSetColor)(float r, float g, float b, float a);
typedef void (FuncRenderBitmap)(unsigned int bitmap_id, float x, float y, float w, float h);
typedef void (FuncRenderClear)(void);

struct RenderCommandRect {
    float r, g, b, a;
    float x, y, width, height;
};

struct RenderCommandCircle {
    float r, g, b, a;
    float x, y, radius;
};

struct RenderCommandBitmap {
    float x, y, w, h;
    u32 bitmap_id;
};

struct RenderCommandClear {
    float r, g, b, a;
};

struct RenderCommand {
    enum mfr__render_commad_type type;
    union {
        RenderCommandRect rect;
        RenderCommandCircle circle;
        RenderCommandBitmap bitmap;
        RenderCommandCircle clear;
    };
};


struct Renderer2D {
    FuncRenderSetColor *func_set_color;
    FuncRenderClear* func_render_clear;
    FuncRenderRect* func_render_rect;
    FuncRenderCircle* func_render_circle;
    FuncRenderBitmap* func_render_bitmap;

    // NOTE: color and position offset for push commands
    float r, g, b, a;
    float x, y;

    RenderCommand *commands;

    u64 commandsCount;
    u64 memorySize;

    void init(u64 memorySize);
    void destroy();

    void set_color(float r, float g, float b, float a);
    void set_offset(float x, float y);
    void reset_offset();

    void push_rect(float x, float y, float width, float height);
    void push_circle(float x, float y, float radius);
    void push_bitmap(u32 bitmap_id, float x, float y, float w, float h);
    void push_clear();

    void flush();

    RenderCommand *_get_next_command();
};





enum mfr__render_commad_type
{
    MF__RENDER_COMMAND_TYPE_UNDEFINED,
    MF__RENDER_COMMAND_TYPE_CLEAR,
    MF__RENDER_COMMAND_TYPE_RECT,
    MF__RENDER_COMMAND_TYPE_BITMAP,
    MF__RENDER_COMMAND_TYPE_CIRCLE,
};



struct mfr_renderer
{

};

#ifdef MF_RENDERER_IMPLEMENTATION

void Renderer2D::init(u64 memorySize) {
    this->commands = (RenderCommand *) malloc(memorySize);
    this->memorySize = memorySize;
    this->commandsCount = 0;
    this->r = 0;
    this->g = 0;
    this->b = 0;
    this->a = 1;
    this->x = 0;
    this->y = 0;
}

void Renderer2D::destroy() {
    free(this->commands);
}

void Renderer2D::set_color(float r, float g, float b, float a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void Renderer2D::set_offset(float x, float y) {
    this->x = x;
    this->y = y;
}

inline
void Renderer2D::reset_offset() {
    this->set_offset(0.0f, 0.0f);
}

RenderCommand *Renderer2D::_get_next_command() {
    MF_Assert(this->memorySize > 0);
    RenderCommand *res = NULL;
    if ((this->commandsCount + 1) * sizeof(RenderCommand) < this->memorySize) {
        res = (this->commands + this->commandsCount);
        res->type = MF__RENDER_COMMAND_TYPE_UNDEFINED;
        this->commandsCount++;
    } else {
        MF_Assert(!"Not enough memory");
    }
    return res;

}


void Renderer2D::push_rect(float x, float y, float width, float height) {
    RenderCommand *command = this->_get_next_command();
    if (command) {
        command->type = MF__RENDER_COMMAND_TYPE_RECT;

        command->rect.r = this->r;
        command->rect.g = this->g;
        command->rect.b = this->b;

        command->rect.x = this->x + x;
        command->rect.y = this->y + y;
        command->rect.width = width;
        command->rect.height = height;
    }
}

void Renderer2D::push_circle(float x, float y, float radius)
{
    RenderCommand *command = this->_get_next_command();
    if (command) {
        command->type = MF__RENDER_COMMAND_TYPE_CIRCLE;

        command->rect.r = this->r;
        command->rect.g = this->g;
        command->rect.b = this->b;

        command->circle.x = x;
        command->circle.y = y;
        command->circle.radius = radius;
    }
}

void Renderer2D::push_bitmap(u32 bitmap_id, float x, float y, float w, float h) {
    RenderCommand *command = this->_get_next_command();
    if (command) {
        command->type = MF__RENDER_COMMAND_TYPE_BITMAP;

        command->bitmap.x = x;
        command->bitmap.y = y;
        command->bitmap.w = w;
        command->bitmap.h = h;
        command->bitmap.bitmap_id = bitmap_id;
    }
}

void Renderer2D::push_clear() {
    RenderCommand *command = this->_get_next_command();
    if (command) {
        command->type = MF__RENDER_COMMAND_TYPE_CLEAR;

        command->clear.r = this->r;
        command->clear.g = this->g;
        command->clear.b = this->b;
        command->clear.a = this->a;
    }
}

void Renderer2D::flush() {
    for (size_t i = 0; i < this->commandsCount; ++i) {
        RenderCommand *command = &this->commands[i];
        switch (command->type) {
            case MF__RENDER_COMMAND_TYPE_RECT: {
                this->func_set_color(command->rect.r,
                                command->rect.g,
                                command->rect.b,
                                command->rect.a);

                if (this->func_render_rect)
                    this->func_render_rect(command->rect.x,
                                           command->rect.y,
                                           command->rect.width,
                                           command->rect.height);

            } break;
            case MF__RENDER_COMMAND_TYPE_CLEAR: {
                this->func_set_color(command->clear.r,
                                command->clear.g,
                                command->clear.b,
                                command->clear.a);
                if (this->func_render_clear)
                    this->func_render_clear();
            } break;
            case MF__RENDER_COMMAND_TYPE_CIRCLE: {
                this->func_set_color(command->circle.r,
                                command->circle.g,
                                command->circle.b,
                                command->circle.a);
                this->func_render_circle(command->circle.x,
                                         command->circle.y,
                                         command->circle.radius);
            } break;
            case MF__RENDER_COMMAND_TYPE_BITMAP: {
                if (this->func_render_bitmap)
                    this->func_render_bitmap(command->bitmap.bitmap_id,
                                             command->bitmap.x,
                                             command->bitmap.y,
                                             command->bitmap.w,
                                             command->bitmap.h);

            } break;
            default:
                MF_Assert(!"Unhandled command");
        }
    }
    this->commandsCount = 0;
}

#endif // MF_RENDERER_IMPLEMENTATION

}}  // mf::renderer
