#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include "graphix.h"

static SDL_GPUGraphicsPipeline* draw_pipeline;
static SDL_GPUSampler* sapmpler;
static SDL_GPUTexture* texture;
static SDL_GPUBuffer* vertex_buffer;



/*
 * Initialization
 *
 *  1.  Load vertex and fragment shaders
 *  2.  Call CreateComputePipelineFromShader to create compute shader
 *      - this is our fill pipeline
 *  3.  Call SDL_CreateGPUGraphicsPipeline to create draw pipeline,
 *      then release shaders
 *
 *  4.  Call SDL_GetWindowSizeInPixels for window dimensions
 *  5.  Call SDL_CreateGPUTexture to initialize the texture
 *  6.  Call SDL_CreateGPUSampler to initialize the sampler
 *  7.  Call SDL_CreateGPUBuffer to initialize the vertex buffer
 *  8.  Call SDL_CreateGPUTransferBuffer to initialize the transfer buffer
 *  9.  Call SDL_MapGPUTransferBuffer to create a data transfer object to associate your data to the transfer buffer
 *  10. Data Stuff
 *  11. Call SDL_UnmapGPUTransferBuffer to send our data to the transfer buffer associated with the gpu
 *  12. Call SDL_AcquireGPUCommandBuffer and SDL_BeginGPUCopyPass to begin data upload to the gpu
 *  13. Call SDL_UploadToGPUBuffer to upload the transfer buffer data to the vertex buffer
 *  14. Call SDL_EndGPUCopyPass
 *  15. Call SDL_BeginGPUComputePass
 */

static int init(graphix::app_state* app)
{
    int app_result = graphix::graphix_init(app, SDL_WINDOW_RESIZABLE);
    if (app_result < 0)
    {
        return app_result;
    }
}

/*
 * Update
 */

/*
 * Draw
 */

/*
 * Quit
 */
