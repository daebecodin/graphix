#include <SDL3/SDL.h>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
#include <array>
#include <iostream>
#include "graphix.h"
#include "demo.h"

// buffer for outline
static SDL_GPUGraphicsPipeline* outline_pipeline;

// buffer for fill-n
static SDL_GPUGraphicsPipeline* fill_pipeline;

static SDL_GPUBuffer* vertex_buffer;

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
    SDL_GPUShader* vertex_shader = gpx::load_shader(app -> gpx_device, "PositionColor.vert", 0, 0, 0, 0);
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
     * Because the api is written in C, when initializing structs, 
     * they use C compound literals to create 
     * an unnamed array which stays alive while the block is alive.
     *
     * Since C++ does not support this feature, we must initialize the struct as an array of 
     * stucts elsewhere to be passed to the graphics pipeline when configuring the info properties
     *
     *
     * These are 
     */

    SDL_GPUColorTargetDescription color_target_descriptions [1] =
    {{
         .format = SDL_GetGPUSwapchainTextureFormat(app -> gpx_device, app -> gpx_window)

     }};

    SDL_GPUVertexBufferDescription vertex_buffer_description [1] =
    {{
         .slot = 0,
         .pitch = sizeof(gpx::position_color_vertex),
         .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
         .instance_step_rate = 0

     }};

    SDL_GPUVertexAttribute vertex_attributes [2] =
    {
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = 0

        },
        {
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
            .offset = sizeof(float) * 3

        }
    };

    // vertex layout of the graphics pipeline
    SDL_GPUVertexInputState input_state = 
    {
        .vertex_buffer_descriptions = vertex_buffer_description,
        .num_vertex_buffers = 1,
        .vertex_attributes = vertex_attributes,
        .num_vertex_attributes = 2
    };

    /*
     * Pipeline configuration
     */
    SDL_GPUGraphicsPipelineCreateInfo gpx_pipeline_info =
    {
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
        .vertex_input_state = input_state,

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
    outline_pipeline = SDL_CreateGPUGraphicsPipeline(app -> gpx_device, &gpx_pipeline_info);
    if (!outline_pipeline)
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
     * Creating Vertex Buffer
     */
    SDL_GPUBufferCreateInfo vertex_info = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(graphix::position_color_vertex) * 3,
        .props = 0
    };

    vertex_buffer = SDL_CreateGPUBuffer(
            app -> gpx_device,
            &vertex_info
            );

    /*
     * Transfer Buffer Configuration
     */
    SDL_GPUTransferBufferCreateInfo transfer_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(graphix::position_color_vertex) * 3,
        .props = 0
    };

    /*
     * Transfer Buffer initialization
     * Where all the data lives
     */
    SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(
            app -> gpx_device,
            &transfer_info
            );

    /*
     * Associates the data to the transfer buffer
     * Returns a pointer to the transfer buffer
     */
    auto* transfer_data = static_cast<graphix::position_color_vertex*>(SDL_MapGPUTransferBuffer(
                app -> gpx_device,
                transfer_buffer,
                false
                ));

    /*
     * Writing rendering data to the data transfer buffer though its pointer
     *
     * first 3 properties are corner positions
     * last 4 properties are rgba codes
     *
     * This data can be anywhere
     */
    transfer_data[0] = graphix::position_color_vertex{-1, -1, 0, 0, 0, 225, 255};
    transfer_data[1] = graphix::position_color_vertex{ 1, -1, 0, 0, 0, 225, 255};
    transfer_data[2] = graphix::position_color_vertex{ 0,  1, 0, 0, 0, 255, 255};

    /*
     * Unmapping sets the data into the transfer buffer
     *
     * Use when data assignment to the buffer is done
     * This invalidates the data data pointer
     */
    SDL_UnmapGPUTransferBuffer(app -> gpx_device, transfer_buffer);


    /*
     * Data is now in the transfer buffer 
     * This begins the data upload to the command buffer
     */
    SDL_GPUCommandBuffer* upload_cmdbf = SDL_AcquireGPUCommandBuffer(app -> gpx_device);
    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(upload_cmdbf);

    /*
     * representing our transfer buffer
     *
     * will no longer be valid once released from memory
     */
    SDL_GPUTransferBufferLocation tbuf_location = {
        transfer_buffer,
        0
    };

    /*
     * representing our vertex buffer
     */
    SDL_GPUBufferRegion buf_region = {
        vertex_buffer,
        0,
        sizeof(graphix::position_color_vertex) * 3
    };

    /*
     * transports data from transfer buffer to vertex buffer
     */
    SDL_UploadToGPUBuffer(
            copy_pass,
            &tbuf_location,
            &buf_region,
            false
    );

    /*
     * Once the data transfer is done, we submit the buffer
     * with all of out display commands to the GPU.
     *
     * The buffer used to transfer data to the vertex buffer is then
     * destroyed when no longer needed
     *
     */
    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(upload_cmdbf);
    SDL_ReleaseGPUTransferBuffer(app -> gpx_device, transfer_buffer);


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
     *
     * It contains the data from the transfer buffer
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
         * Begin Render Pass
         *
         * Her
         */

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbf, &color_info, 1, nullptr);
        SDL_BindGPUGraphicsPipeline(renderPass, use_wireframe_mode ? outline_pipeline : fill_pipeline);

        /*
         * Binding the vertex buffer to the command buffer
         */
        SDL_GPUBufferBinding vertex_buffer_bindings = 
        {
            vertex_buffer,
            0
        };
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertex_buffer_bindings, 1);


        if (use_small_viewport) 
        {
            SDL_SetGPUViewport(renderPass, &graphix_viewport);
        }
        if (use_scissor_rect) 
        {
            SDL_SetGPUScissor(renderPass, &scissor_rect);

        }

        /*
         * SDL_DrawGPUPrimitives takes the data from the render pass object
         * and uses it as bounds for drawing
         *
         * 3 veritecies
         * 1 instance
         *
         *
         */
        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

        /*
         * Unbinds all render state of the render pass
         *
         * The render pass handle is now invalid
         */
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

    /*
     * freeing pipelines from memory
     */
    SDL_ReleaseGPUGraphicsPipeline(app -> gpx_device, outline_pipeline);
    SDL_ReleaseGPUGraphicsPipeline(app -> gpx_device, fill_pipeline);

    /*
     * freeing vertex buffer from memory
     */
    SDL_ReleaseGPUBuffer(app -> gpx_device, vertex_buffer);

    use_wireframe_mode = false;
    use_small_viewport = false;
    use_scissor_rect = false;
    gpx::graphix_quit(app);
}

gpx_demo::Demo gpx_demo::vbuff_triangle_demo =
{
    "BasicTriangle", ::init, ::refresh, ::draw, ::quit 
};




