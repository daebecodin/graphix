#pragma once

#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include "graphix.h"

namespace graphix::demo {

    struct Demo 
    {
        const char* name;
        int (*graphix_init)(app_state* app);
        int (*graphix_resfresh)(app_state* app);
        int (*graphix_display)(app_state* app);
        void (*graphix_quit)(app_state* app);

    };

    inline int init(graphix::app_state* app)
    {
        return graphix_init(app, SDL_WINDOW_RESIZABLE);
    };

    inline int graphix_refresh(graphix::app_state* app)
    {
        return 0;
    };

    inline int graphix_display(graphix::app_state* app)
    {
        return 0;

    };

    inline void quit(graphix::app_state* app)
    {
        return graphix_quit(app);
    };


    extern Demo clear_screen_demo;
    extern Demo basic_triangle_demo;
    extern Demo vbuff_triangle_demo;

}

