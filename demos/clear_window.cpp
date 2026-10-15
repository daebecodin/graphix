
#include <SDL3/SDL.h>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
#include "graphix.h"

#include "demo.h"

namespace gpx = graphix;
namespace gpx_demo = graphix::demo;

// initialize world
static int init_grpahix(gpx::app_state* app) 
{
    return gpx::graphix_init(app, SDL_WINDOW_RESIZABLE);

} 

// idk
static int graphix_refresh(gpx::app_state* app) 
{
    return 0;
}

/*
 * Here we initialize and supply a command buffer for use in creating the swap-chain object 
 *
 * Once both are initialized we create our render pass object for per frame execution
 *
 * Ending application on quit
 */
static int graphix_display(gpx::app_state* app) 
{
    SDL_GPUCommandBuffer* gpx_buffer = SDL_AcquireGPUCommandBuffer(app -> gpx_device);

    if (!gpx_buffer)
    {
        SDL_Log("Failed to GPU Command Buffer: %s", SDL_GetError());
        return -1;
    }

    SDL_GPUTexture *gpx_swapchain_texture;
    if(!SDL_WaitAndAcquireGPUSwapchainTexture(gpx_buffer, app -> gpx_window, &gpx_swapchain_texture, nullptr, nullptr))
    {
        SDL_Log("Failed to acquire Swap Chain Buffer: %s", SDL_GetError());
        return -1;
    }

    if (gpx_swapchain_texture != nullptr) 
    {
        SDL_GPUColorTargetInfo gpx_color_info {};
        gpx_color_info.texture = gpx_swapchain_texture;
        gpx_color_info.clear_color = SDL_FColor { 0.3f, 0.4f, 0.5f, 1.0f };
        gpx_color_info.load_op = SDL_GPU_LOADOP_CLEAR;
        gpx_color_info.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* gpx_render_pass = SDL_BeginGPURenderPass(gpx_buffer, &gpx_color_info, 1, nullptr);

        SDL_EndGPURenderPass(gpx_render_pass);
    }

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
