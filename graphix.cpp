#include "graphix.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

#define windowWidth 1280
#define windowHeight 720

namespace gpx = graphix;

int graphix::graphix_init(gpx::app_state* app, SDL_WindowFlags windowFlags) {

    /*
     * Initializing the application GPU Device
     * We allow it it provide shaders for vulkan, directX, and metal
     * Debug setting enabled
     * I set the preferred GPU driver to NULL so SDL pick best option
     */
    app -> device = SDL_CreateGPUDevice(
            SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_METALLIB | 
            SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_SPIRV,
            true,
            NULL
            );

    /*
     * GPU device creation error handling
     */
    if ( app -> device == NULL )
    {
        SDL_Log("GPU Device Creation Failure: %s",  SDL_GetError());
        return -1;
    }

    /*
     * Application Window Initialization
     * windows is created with a name, dimensions, and window dimension flags
     */

    app -> window = SDL_CreateWindow(
            app -> appName,
            windowWidth, 
            windowHeight,  
            windowFlags
            );

    /*
     * Window Creation Error Handling
     */
    if ( app -> window == NULL )
    {
        SDL_Log("Application Window Creation Failure: %s", SDL_GetError());
        return -1;
    }

    /*
     * Claiming the application window and providing a swap buffer for it
     */
    if ( !SDL_ClaimWindowForGPUDevice( app -> device, app -> window ) )
    {
        SDL_Log("Cannot Claim Window: %s", SDL_GetError());
        return -1;
    }

    return 0;
}

/*
 * On application quit, we want to destroy our resources in reverse order of creation
 */
void graphix::graphix_quit(gpx::app_state* app) 
{
    SDL_ReleaseWindowFromGPUDevice( app -> device, app -> window );
    SDL_DestroyWindow( app -> window );
    SDL_DestroyGPUDevice( app -> device );
}

/*
 * Returns a file path to the applications root directory
 */
static const char* base_path;
void graphix::init_asset_loader()
{
    base_path = SDL_GetBasePath();
}

/*
 * 
 */
SDL_GPUShader* load_shader(
        SDL_GPUDevice* device,
        const char* shaderFileName,
        std::uint32_t samplerCount,
        std::uint32_t uniformBufferCount,
        std::uint32_t storageBufferCount,
        std::uint32_t storageTextureCount
        ) {
            // Verifying Shader Stage
            SDL_GPUShaderStage stage;

            if ( SDL_strstr(shaderFileName, ".vert") )
            {
                stage = SDL_GPU_SHADERSTAGE_VERTEX;
            }

            if ( SDL_strstr(shaderFileName, ".frag") )
            {
                stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
            }

            else 
            {
                SDL_Log("Invalid Shader Stage!: %s", SDL_GetError());
                return NULL;
            }
}
