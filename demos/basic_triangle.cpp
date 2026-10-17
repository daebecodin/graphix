#include <SDL3/SDL.h>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
#include <array>
#include "graphix.h"
#include "demo.h"

// buffer for outline
static SDL_GPUGraphicsPipeline* outline_pipeliine;

// buffer for fill-n
static SDL_GPUGraphicsPipeline* fill_pipeline;

// view port
static SDL_GPUViewport graphix_viewport { 160, 120, 320, 240, 0.1f, 1.0f };

static SDL_Rect scissor_rect { 320, 240, 320, 240 };

static bool use_wireframe_mode = false;
static bool use_small_viewport = false;
static bool use_scissor_rect = false;

namespace gpx = graphix;
namespace gpx_demo = graphix::demo;


/*
 * Initializing both vertex and fragment shaders to our pipeline configuration.
 * 
 * Initializing both fill and outline pipelines with the demos GPU device
 *
 * Freeing shaders from memory when no longer needed
 */
static int init(gpx::app_state* app) 
{
    // initializing with 0 window properties
    int app_result = gpx::graphix_init(app, 0);

    /*
     * failed window creation
     */
    if (app_result < 0)
    {
        return app_result;
    }

    /*
     * Create Shaders
     */
    SDL_GPUShader* vertex_shader = gpx::load_shader(app -> gpx_device, "RawTriangle.vert", 0, 0, 0, 0);
    if (vertex_shader == nullptr) 
    {
        SDL_Log("Failed to initialize vertex shader: %s", SDL_GetError());
        return -1;
    }

    SDL_GPUShader* fragment_shader = gpx::load_shader(app -> gpx_device, "SolidColor.frag", 0, 0, 0, 0);
    if (fragment_shader == nullptr) 
    {
        SDL_Log("Failed to initialize fragment shader: %s", SDL_GetError());
        return -1;
    }


    /*
     * Because the api is written in C, when initializing descriptions, 
     * specifically the format field, They use C compound literals to create 
     * an unnamed array which stays alive while the block is alive.
     *
     * Since C++ does not support this feature, we must initialize format in an array of 
     * descriptions elsewhere to be passed to target_info when creating the info properties
     * for your pipelines.
     */
    SDL_GPUColorTargetDescription color_target_descriptions [1] =
    {{
         .format = SDL_GetGPUSwapchainTextureFormat(app -> gpx_device, app -> gpx_window)

     }};

    /*
     * Pipeline configuration
     */
    SDL_GPUGraphicsPipelineCreateInfo gpx_pipeline_info =
    {
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,

        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = 
        {
            .color_target_descriptions = color_target_descriptions,
            .num_color_targets = 1
        }
    };


    /*
     * Applying rasterize type LINE to the outline buffer
     * Then initializing a rasterizing pipeline
     */
    gpx_pipeline_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_LINE;
    outline_pipeliine = SDL_CreateGPUGraphicsPipeline(app -> gpx_device, &gpx_pipeline_info);
    if (!outline_pipeliine)
    {
        SDL_Log("Failed to initialize outline pipeline: %s", SDL_GetError());
        return -1;
    }

    /*
     * Applying rasterize type FILL to the outline buffer
     * Then initializing a rasterizing pipeline
     */
    gpx_pipeline_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    fill_pipeline = SDL_CreateGPUGraphicsPipeline(app -> gpx_device, &gpx_pipeline_info);
    if (!fill_pipeline)
    {
        SDL_Log("Failed to initialize fill pipeline: %s", SDL_GetError());
        return -1;
    }

    /*
     * Releasing the shaders from memory when safe to do so
     *
     * Do not reference these shaders beyond this point
     */
    SDL_ReleaseGPUShader(app -> gpx_device, fragment_shader);
    SDL_ReleaseGPUShader(app -> gpx_device, vertex_shader);

    /*
     * View Port Prompts
     */
    SDL_Log("Press Left to toggle wireframe mode");
    SDL_Log("Press Down to toggle small viewport");
    SDL_Log("Press Right to toggle scissor rect");

    return 0;
}

/*
 * Updating application state
 */
static int refresh(gpx::app_state* app) 
{
    if (app -> left_pressed)
    {
        use_wireframe_mode = !use_wireframe_mode;
    }

    if (app -> down_pressed)
    {
        use_small_viewport = !use_small_viewport;
    }

    if (app -> right_pressed)
    {
        use_scissor_rect = !use_scissor_rect;
    }

    return 0;
}

/*
 * Take the rendering instruction form pipelines
 */
static int draw(gpx::app_state* app)
{
    /*
     * Initialize application command buffer
     */
    SDL_GPUCommandBuffer* cmdbf = SDL_AcquireGPUCommandBuffer(app -> gpx_device);
    if (cmdbf == nullptr)
    {
        SDL_Log("Failed to collect rendering instruction: %s", SDL_GetError());
        return -1;
    }

    /*
     * Waits for a swap chain texture to be collected
     * from the application rendering pipeline
     */
    SDL_GPUTexture* swapchain_texture;
    if (!SDL_AcquireGPUSwapchainTexture(cmdbf, app -> gpx_window, &swapchain_texture, nullptr, nullptr))
    {
        SDL_Log("Failed to collect rendering instruction: %s", SDL_GetError());
        return -1;
    }

    // window color
    SDL_FColor clear_color = {
        0.70f,
        0.88f, 
        0.960f, 
        1.0f
    };

    /*
     * Texture Configuration
     */
    if (swapchain_texture != nullptr)
    {
        SDL_GPUColorTargetInfo color_info {};
        color_info.texture = swapchain_texture;
        color_info.clear_color = clear_color;

        // When the window loads, we load it with a single color
        color_info.load_op = SDL_GPU_LOADOP_CLEAR;
        // On every render pass we will store the resulting texture
        color_info.store_op = SDL_GPU_STOREOP_STORE;

        /*
         * Rendering
         *
         * Here we pass our fill and line pipelines to the render pass for rendering each draw frame
         */
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbf, &color_info, 1, nullptr);
        SDL_BindGPUGraphicsPipeline(renderPass, use_wireframe_mode ? outline_pipeliine : fill_pipeline);

        if (use_small_viewport) 
        {
            SDL_SetGPUViewport(renderPass, &graphix_viewport);
        }
        if (use_scissor_rect) 
        {
            SDL_SetGPUScissor(renderPass, &scissor_rect);
        }

        // render 3 lines for the triangle
        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
        SDL_EndGPURenderPass(renderPass);
    }

    /*
     * Once render commands are set,
     * we send the buffer to be processed by the GPU
     */
    SDL_SubmitGPUCommandBuffer(cmdbf);

    return 0;
}

static void quit(gpx::app_state* app)
{

    SDL_ReleaseGPUGraphicsPipeline(app -> gpx_device, outline_pipeliine);
    SDL_ReleaseGPUGraphicsPipeline(app -> gpx_device, fill_pipeline);

    use_wireframe_mode = false;
    use_small_viewport = false;
    use_scissor_rect = false;
     gpx::graphix_quit(app);
}

gpx_demo::Demo gpx_demo::basic_triangle_demo =
{
    "BasicTriangle", ::init, ::refresh, ::draw, ::quit 
};




