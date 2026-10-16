#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "demo.h"
#include "graphix.h"

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    graphix::app_state app {};
    auto& demo = graphix::demo::basic_triangle_demo;
    app.appName = demo.name;
    app.basePath = SDL_GetBasePath();
    graphix::init_asset_loader();

    if (demo.graphix_init(&app) < 0)
    {
        SDL_Log("Failed to initialize demo: %s", SDL_GetError());
        if (app.gpx_window)
        {
            SDL_DestroyWindow(app.gpx_window);
        }
        if (app.gpx_device)
        {
            SDL_DestroyGPUDevice(app.gpx_device);
        }
        SDL_Quit();
        return 1;
    }

    bool running = true;
    int exit_code = 0;
    Uint64 last_time = SDL_GetTicksNS();

    while (running)
    {
        // These flags represent a new press this frame, not a held key.
        app.left_pressed = false;
        app.right_pressed = false;
        app.down_pressed = false;
        app.up_pressed = false;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                 event.window.windowID == SDL_GetWindowID(app.gpx_window)))
            {
                running = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat &&
                event.key.windowID == SDL_GetWindowID(app.gpx_window))
            {
                switch (event.key.key)
                {
                    case SDLK_LEFT: app.left_pressed = true; break;
                    case SDLK_RIGHT: app.right_pressed = true; break;
                    case SDLK_DOWN: app.down_pressed = true; break;
                    case SDLK_UP: app.up_pressed = true; break;
                    default: break;
                }
            }
        }

        if (!running)
        {
            break;
        }

        Uint64 current_time = SDL_GetTicksNS();
        app.deltaTime = static_cast<float>(current_time - last_time) / 1.0e9f;
        last_time = current_time;

        if (demo.graphix_resfresh(&app) < 0 || demo.graphix_display(&app) < 0)
        {
            SDL_Log("Demo frame failed: %s", SDL_GetError());
            exit_code = 1;
            break;
        }
    }

    demo.graphix_quit(&app);
    SDL_Quit();
    return exit_code;
}
