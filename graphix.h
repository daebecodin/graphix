#pragma once

#include <SDL3/SDL.h>
#include <cstdint>

namespace graphix {

    /*
     * Holds the state for the application and running example
     */
    struct app_state {
        const char* appName;
        const char* basePath;
        SDL_Window* window;
        SDL_GPUDevice* device;
        float deltaTime;

    };

    /*
     * Initialize an example with a window and GPU device
     */
    int graphix_init(app_state* app, SDL_WindowFlags);

    /*
     * Closes the application buffers
     */
    void graphix_quit(app_state* app);

    /*
     * Call to initialize an asset loader
     */
    void init_asset_loader();

    /*
     * Initializes a specified shader from its hlsl file.
     * It is initialized with ...
     */
    SDL_GPUShader* load_shader(
            SDL_GPUDevice* device,
            const char* shaderFileName,
            std::uint32_t samplerCount,
            std::uint32_t uniformBufferCount,
            std::uint32_t storageBufferCount,
            std::uint32_t storageTextureCount

            );


}
