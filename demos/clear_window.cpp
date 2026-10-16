
#include <SDL3/SDL.h>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
#include "graphix.h"

#include "demo.h"

namespace gpx = graphix;
namespace gpx_demo = graphix::demo;

/*
 * Initialize world
 */
static int init_grpahix(gpx::app_state* app) 
{
    return gpx::graphix_init(app, SDL_WINDOW_RESIZABLE);

} 

/*
 * Not needed
 */
static int graphix_refresh(gpx::app_state* app) 
{
    return 0;
}

/*
 * @brief Displaying window
 *
 * We create the command buffer device for use of a texture. 
 *
 * Once both are created we initialize the swap-chain for the render pass
 *
 * After texture configuration, we initialize the render pass with parameters
 *
 * Close and delete buffers when no longer in use
 *
 * @param app Colored Window
 */
static int graphix_display(gpx::app_state* app) 
{
    // initialize buffer
    SDL_GPUCommandBuffer* gpx_buffer = SDL_AcquireGPUCommandBuffer(app -> gpx_device);

    if (!gpx_buffer)
    {
        SDL_Log("Failed to GPU Command Buffer: %s", SDL_GetError());
        return -1;
    }

    // initialize texture
    SDL_GPUTexture *gpx_swapchain_texture;
    if(!SDL_WaitAndAcquireGPUSwapchainTexture(gpx_buffer, app -> gpx_window, &gpx_swapchain_texture, nullptr, nullptr))
    {
        SDL_Log("Failed to acquire Swap Chain Buffer: %s", SDL_GetError());
        return -1;
    }

    if (gpx_swapchain_texture != nullptr) 
    {
        // texture builder for render pass
        SDL_GPUColorTargetInfo gpx_color_info {};

        // color target for render pass
        gpx_color_info.texture = gpx_swapchain_texture;

        // window-rgba code, but ignored by load op
        // add conversion function
        gpx_color_info.clear_color = SDL_FColor 
        { 
            254.0f / 255.0f, 
            40.0f / 255.0f, 
            145.0f / 255.0f, 
            0.8f 
        };
        
        // Set texture contents at each pass
        gpx_color_info.load_op = SDL_GPU_LOADOP_CLEAR;

        // During render pass, the texture is stored
        gpx_color_info.store_op = SDL_GPU_STOREOP_STORE;

        // start the pass in buffer, apply texture
        SDL_GPURenderPass* gpx_render_pass = SDL_BeginGPURenderPass(gpx_buffer, &gpx_color_info, 1, nullptr);

        // End
        SDL_EndGPURenderPass(gpx_render_pass);
    }

    // clear resources
    SDL_SubmitGPUCommandBuffer(gpx_buffer);

    return 0;
} 

/*
 * Close app
 */
static void quit_graphix(gpx::app_state* app)
{
    gpx::graphix_quit(app);
}

gpx_demo::Demo gpx_demo::clear_screen_demo 
{ 
    "CLear Screen", 
        ::init_grpahix, 
        ::graphix_refresh, 
        ::graphix_display, 
        ::quit_graphix
};
