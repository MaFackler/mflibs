#ifndef MF_RENDERER_H
#define MF_RENDERER_H

#include <assert.h>

/*



USAGE:

#define MF_RENDERER_IMPLEMENTATION
#include "mf_renderer.h"

void my_set_color(float r, float g, float b, float a)
{
    //...
}

void my_rectangle_renderer(float x, float y, float width, float height)
{
    //....
}

int main()
{
    mfr_renderer renderer;

    // override rendering functions
    // signatures of rendering functions are described under this example
    // Functions starting with func_*
    renderer->func_render_set_color = my_set_color
    renderer->func_render_rect = my_rectangle_renderer
    // ...

    mfr_init(&renderer, 1024 * 1024 * 1024);


    mfr_set_color(&renderer, 1.0f, 0.0f, 0.0f, 1.0f);
    mfr_set_offset(&renderer, 10.0f, 0.0f);

    mfr_push_clear(&renderer);
    mfr_push_rect(&renderer, 0, 0, 100, 100);

    mfr_flush(&renderer);
    
}

*/
#ifdef __cplusplus
#define mf_inline inline
#else
#define mf_inline
#endif



// Signature of render functions
typedef void (func_render_rect)(float x, float y, float width, float height);
typedef void (func_render_set_color)(float r, float g, float b, float a);
typedef void (func_render_clear)(void);


typedef struct mfr_renderer mfr_renderer;
void mfr_init(mfr_renderer *renderer, size_t memorySize);
void mfr_set_color(mfr_renderer *renderer, float r, float g, float b, float a);
void mfr_set_offset(mfr_renderer *renderer, float x, float y);
void mfr_reset_offset(mfr_renderer *renderer);
// Push Commands
void mfr_push_rect(mfr_renderer *renderer, float x, float y, float width, float height);
void mfr_push_clear(mfr_renderer *renderer);
// Flush buffer
void mfr_flush(mfr_renderer *renderer);



enum mfr__render_commad_type
{
    MF__RENDER_COMMAND_TYPE_UNDEFINED,
    MF__RENDER_COMMAND_TYPE_CLEAR,
    MF__RENDER_COMMAND_TYPE_RECT,
};

typedef struct
{
    float r, g, b, a;
    float x, y, width, height;
} mfr__render_command_rect;

typedef struct
{
    float r, g, b, a;
} mfr__render_command_clear;

typedef struct
{
    enum mfr__render_commad_type type;
    union
    {
        mfr__render_command_rect rect;
        mfr__render_command_clear clear;
    };

} mfr__render_command;

struct mfr_renderer
{
    func_render_set_color *set_color;
    func_render_clear* render_clear;
    func_render_rect* render_rect;

    // NOTE: color and position offset for push commands
    float r, g, b, a;
    float x, y;

    mfr__render_command *commands;
    size_t commandsCount;
    size_t memorySize;

};


#ifdef MF_RENDERER_IMPLEMENTATION


void mfr_init(mfr_renderer *renderer, size_t memorySize)
{
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

inline
void mfr_set_color(mfr_renderer *renderer, float r, float g, float b, float a)
{
    renderer->r = r;
    renderer->g = g;
    renderer->b = b;
    renderer->a = a;
}

inline
void mfr_set_offset(mfr_renderer *renderer, float x, float y)
{
    renderer->x = x;
    renderer->y = y;
}

inline
void mfr_reset_offset(mfr_renderer *renderer)
{
    mfr_set_offset(renderer, 0.0f, 0.0f);
}

mf_inline
mfr__render_command *mfr__get_next_command(mfr_renderer *renderer)
{
    assert(renderer->memorySize > 0);
    mfr__render_command *res = NULL;
    if ((renderer->commandsCount + 1) * sizeof(mfr__render_command) < renderer->memorySize)
    {
        res = (renderer->commands + renderer->commandsCount);
        res->type = MF__RENDER_COMMAND_TYPE_UNDEFINED;
        renderer->commandsCount++;
    }
    else
    {
        assert(!"Not enough memory");
    }
    return res;

}


void mfr_push_rect(mfr_renderer *renderer, float x, float y, float width, float height)
{
    mfr__render_command *command = mfr__get_next_command(renderer);
    if (command)
    {
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

void mfr_push_clear(mfr_renderer *renderer)
{
    mfr__render_command *command = mfr__get_next_command(renderer);
    if (command)
    {
        command->type = MF__RENDER_COMMAND_TYPE_CLEAR;

        command->clear.r = renderer->r;
        command->clear.g = renderer->g;
        command->clear.b = renderer->b;
        command->clear.a = renderer->a;
    }
}

void mfr_flush(mfr_renderer *renderer)
{
    for (size_t i = 0; i < renderer->commandsCount; ++i)
    {
        mfr__render_command *command = &renderer->commands[i];
        switch (command->type)
        {
            case MF__RENDER_COMMAND_TYPE_RECT:
            {
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
            case MF__RENDER_COMMAND_TYPE_CLEAR:
            {
                renderer->set_color(command->clear.r,
                                    command->clear.g,
                                    command->clear.b,
                                    command->clear.a);
                if (renderer->render_clear)
                    renderer->render_clear();
            } break;
            default:
                assert(!"Unhandled command");
        }
    }
    renderer->commandsCount = 0;
}

#endif // MF_RENDERER_IMPLEMENTATION

#endif // MF_RENDERER_H
