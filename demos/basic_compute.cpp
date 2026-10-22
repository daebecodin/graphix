#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include "graphix.h"

static SDL_GPUShader* vertex_shader;
static SDL_GPUShader* fragment_shader;
static SDL_GPUComputePipeline* fill_pipeline;
static SDL_GPUGraphicsPipeline* draw_pipeline;
static SDL_GPUSampler* sampler;
static SDL_GPUTexture* texture;
static SDL_GPUBuffer* vertex_buffer;
static SDL_GPUTransferBuffer* transfer_buffer;
static SDL_GPUComputePass* compute_pass;

namespace gpx = graphix;


static int init(graphix::app_state* app)
{
    int app_result = graphix::graphix_init(app, SDL_WINDOW_RESIZABLE);
    if (app_result < 0)
    {
        return app_result;
    }

    /*
     * Load Vertex Shaders
     */
    vertex_shader = gpx::load_shader(
            app -> gpx_device,
            "TexturedQuad.vert",
            0,
            0,
            0,
            0
            );
    if (vertex_shader == nullptr)
    {
        SDL_Log("Failed to load vertex shader: %s", SDL_GetError());
    }

    /*
     * Load Fragment Shader
     */
    fragment_shader = gpx::load_shader(
            app -> gpx_device,
            "TexturedQuad.frag",
            1,
            0,
            0,
            0
            );
    if (fragment_shader == nullptr)
    {
        SDL_Log("Failed to load fragment shader: %s", SDL_GetError());
    }

    /*
     * Fill Pipeline Configuration
     */
    SDL_GPUComputePipelineCreateInfo compute_info {};
    compute_info.num_readwrite_storage_textures = 1,
        compute_info.threadcount_x = 8;
    compute_info.threadcount_y = 8;
    compute_info.threadcount_z = 1;

    fill_pipeline = gpx::create_compute_pipeline(
            app -> gpx_device,
            "FillTexture.comp",
            &compute_info

);

    /*
     * Draw Pipeline Configuration
     */
    SDL_GPUVertexBufferDescription vbuff_description{};
    vbuff_description.slot = 0;
    vbuff_description.pitch = sizeof(gpx::position_vertex);
    vbuff_description.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

    SDL_GPUVertexAttribute vbuff_attribute_1 {};
    vbuff_attribute_1.location = 0;
    vbuff_attribute_1.buffer_slot = 0;
    vbuff_attribute_1.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vbuff_attribute_1.offset = 0;

    SDL_GPUVertexAttribute vbuff_attribute_2 {};
    vbuff_attribute_2.location = 1;
    vbuff_attribute_2.buffer_slot = 0;
    vbuff_attribute_2.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vbuff_attribute_2.offset = sizeof(float) * 3;

    SDL_GPUVertexAttribute attrs[] = {
        vbuff_attribute_1,
        vbuff_attribute_2
    };

    SDL_GPUVertexInputState input_state {};
    input_state.vertex_buffer_descriptions = &vbuff_description;
    input_state.num_vertex_buffers = 1;
    input_state.vertex_attributes = attrs;
    input_state.num_vertex_attributes = 2;

    SDL_GPUColorTargetDescription color_target_description {};
    color_target_description.format = SDL_GetGPUSwapchainTextureFormat(
            app -> gpx_device,
            app -> gpx_window
            );

    SDL_GPUGraphicsPipelineTargetInfo target_info {};
    target_info.num_color_targets = 1;
    target_info.color_target_descriptions = &color_target_description;



    SDL_GPUGraphicsPipelineCreateInfo graphics_info{};
    graphics_info.vertex_shader = vertex_shader;
    graphics_info.fragment_shader = fragment_shader;
    graphics_info.vertex_input_state = input_state;
    graphics_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    graphics_info.target_info = target_info;

    draw_pipeline = SDL_CreateGPUGraphicsPipeline(
            app -> gpx_device,
            &graphics_info
            );

    /*
     * Release the shaders from memory
     */
    SDL_ReleaseGPUShader(app -> gpx_device, fragment_shader);
    SDL_ReleaseGPUShader(app -> gpx_device, vertex_shader);

    /*
     * Window Dimensions
     *
     * will be applied to the texture dimensions
     */
    int w, h;
    SDL_GetWindowSizeInPixels(app -> gpx_window, &w, &h);

    /*
     * Textures are done in a 2-step process.
     *
     * First by defining the texture
     *
     * Second by defining sampler, who describes how the GPU uses 
     * the texture.
     */
    SDL_GPUTextureCreateInfo texture_info{};
    texture_info.type = SDL_GPU_TEXTURETYPE_2D;
    texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    texture_info.usage = SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    texture_info.width = w;
    texture_info.height = h;
    texture_info.layer_count_or_depth = 1;
    texture_info.num_levels = 1;

    SDL_GPUSamplerCreateInfo sampler_info{};
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;


    texture = SDL_CreateGPUTexture(app -> gpx_device, &texture_info);
    sampler = SDL_CreateGPUSampler(app -> gpx_device, &sampler_info);

    /*
     * Vertex Buffer
     */
    SDL_GPUBufferCreateInfo vertex_info {};
    vertex_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertex_info.size = sizeof(gpx::position_texture_vertex) * 6;

    vertex_buffer = SDL_CreateGPUBuffer(
            app -> gpx_device,
            &vertex_info
            );

    /*
     * Transfer Buffer
     */
    SDL_GPUTransferBufferCreateInfo transfer_info{};
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_info.size = sizeof(gpx::position_texture_vertex) * 6;

    transfer_buffer = SDL_CreateGPUTransferBuffer(
            app -> gpx_device,
            &transfer_info
            );

    /*
     * Map Data To Transfer Buffer
     *
     * We associate the buffer holding our data to our GPU device before
     * writing to the buffer
     *
     * Returns a pointer to the transfer_buffer memory
     */
    auto* transfer_data = static_cast<gpx::position_texture_vertex*>(
            SDL_MapGPUTransferBuffer(
                app -> gpx_device,
                transfer_buffer,
                false
                )
                );

    /*
     * The mapped data is now assigned
     */
    transfer_data[0] = gpx::position_texture_vertex { -1, -1, 0, 0, 0 };
    transfer_data[1] = gpx::position_texture_vertex { 1, -1, 0, 1, 0 };
    transfer_data[2] = gpx::position_texture_vertex { 1,  1, 0, 1, 1 };
    transfer_data[3] = gpx::position_texture_vertex { -1, -1, 0, 0, 0 };
    transfer_data[4] = gpx::position_texture_vertex { 1,  1, 0, 1, 1 };
    transfer_data[5] = gpx::position_texture_vertex { -1,  1, 0, 0, 1 };

    /*
     * Sets the data to the transfer buffer
     */
    SDL_UnmapGPUTransferBuffer(
            app -> gpx_device,
            transfer_buffer
            );

    /*
     * Initiate Data Upload
     *
     * We start by getting the command buffer from the device
     * Then we begin the transfer with the call to copy pass
     */
    SDL_GPUCommandBuffer* cmdbfr = SDL_AcquireGPUCommandBuffer(app -> gpx_device);
    if (cmdbfr == nullptr)
    {
        SDL_Log("Failed to Acquire GPU device Command Buffer: %s", SDL_GetError());
    }

    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmdbfr);

    /*
     * Data Upload
     *
     * To upload the transfer buffer we create 2 objects 
     * representing the source and destination buffers
     *
     * Once uploaded, we end the GPU pass
     */

    SDL_GPUTransferBufferLocation tbuff_location {};
    tbuff_location.transfer_buffer = transfer_buffer;
    tbuff_location.offset = 0;

    SDL_GPUBufferRegion buff_region {};
    buff_region.buffer = vertex_buffer;
    buff_region.offset = 0;
    buff_region.size = sizeof(gpx::position_texture_vertex) * 6;


    SDL_UploadToGPUBuffer(
            copy_pass,
            &tbuff_location,
            &buff_region,
            false
            );

    SDL_EndGPUCopyPass(copy_pass);

    /* Starting the compute pass to let the GPU handle
     * texture writing
     *
     * We associate the fill compute pipeline to the
     * command buffer
     *
     * One associated, we tell the GPU to run the compute shader 
     * once for every pixel on initialization
     *
     * In draw loop, it would be every frame
     */
    SDL_GPUStorageTextureReadWriteBinding texture_binder {};
    texture_binder.texture = texture;


    compute_pass = SDL_BeginGPUComputePass(
            cmdbfr,
            &texture_binder,
            1,
            nullptr,
            0
            );

    SDL_BindGPUComputePipeline(compute_pass, fill_pipeline);

    // Each work-group is 8 x 8 x 1 threads (set in the pipeline create info).
    // Dispatch w / 8 by h / 8 work-groups so we cover one thread per pixel.
    std::uint32_t groupcount_x = w / 8;
    std::uint32_t groupcount_y = h / 8;
    std::uint32_t groupcount_z = 1;
    SDL_DispatchGPUCompute(
            compute_pass,
            groupcount_x,
            groupcount_y,
            groupcount_z
            );

    SDL_EndGPUComputePass(compute_pass);

    /*
     * We submit the command buffer
     * to the GPU to be executed
     */
    SDL_SubmitGPUCommandBuffer(cmdbfr);

    /*
     * Release buffers and pipelines buffers from memory when safe to do so
     */
    SDL_ReleaseGPUTransferBuffer(app -> gpx_device, transfer_buffer);
    SDL_ReleaseGPUComputePipeline(app -> gpx_device, fill_pipeline);

    return 0;
}

/*
 * Update
 */
static int update()
{
    return 0;
}

/*
 * Draw
 */

/*
 * Quit
 */
