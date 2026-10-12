#pragma once

#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include "graphix.h"

namespace graphix::demo {

    struct Demo 
    {
        const char* name;
        int (*graphix_init)(app_state* app);
        // update
        // draw
        void (*graphix_quit)(app_state* app);

    };

    inline int init(app_state* app)
    {
        return graphix_init(app, SDL_WINDOW_RESIZABLE);
    };

    inline int update(app_state* app)
    {
        return 0;
    };

    inline int draw(app_state* app)
    {

    };

    inline void quit(app_state* app)
    {
        return graphix_quit(app);
    };


    extern Demo clear_screen_demo;

}

