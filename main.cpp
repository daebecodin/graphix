#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <charconv>
#include <vector>
#include <string_view>
#include "demo.h"
#include "graphix.h"

namespace gpx_demo = graphix::demo;

int main(int argc, char* argv[])
{
    std::vector<gpx_demo::Demo*> demos =
    {
        &gpx_demo::clear_screen_demo,
        &gpx_demo::basic_triangle_demo,
        &gpx_demo::vbuff_triangle_demo
    };

    if (argc != 2)
    {
        SDL_Log("Usage: %s <demo number: 0=clear, 1=triangle, 2=vertex buffer triangle>", argv[0]);
        return 1;
    }

    int index = 0;
    std::string_view argument = argv[1];
    auto [end, error] = std::from_chars(argument.data(), argument.data() + argument.size(), index);
    if (error != std::errc{} || end != argument.data() + argument.size() ||
        index < 0 || static_cast<std::size_t>(index) >= demos.size())
    {
        SDL_Log("Invalid demo number: %s (choose 0, 1, or 2)", argv[1]);
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    graphix::app_state app {};

    auto& demo = *demos[index];
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
