#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <cstdint>
#include "graphix.h"
#include "shaders.h"

/**
 * @brief window width
 */
#define windowWidth 1280
/**
 * @brief window height
 */
#define windowHeight 720

namespace gpx = graphix;
namespace gpx_shaders = graphix::shader_paths;

/**
 * @brief call to initialize an application
 * @param app the graphical app to show
 * @param window Flagswindow sizing flags
 * @return 0 Success
 * @return -1 Failure
 */
int graphix::graphix_init(gpx::app_state* app, SDL_WindowFlags windowFlags) {

    /*
     * Initializing the application GPU Device
     * - specify the shader formats the application can use
     * - debug mode true
     * - allow SDL to chose best graphics backend
     */
    app -> gpx_device = SDL_CreateGPUDevice(
            SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_METALLIB | 
            SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_SPIRV,
            true,
            NULL
            );

    /*
     * GPU device creation error handling
     */
    if ( app -> gpx_device == NULL )
    {
        SDL_Log("GPU Device Creation Failure: %s",  SDL_GetError());
        return -1;
    }

    /*
     * Application Window Initialization
     * windows is created with a window-title, window dimensions, and window flags
     */

    app -> gpx_window = SDL_CreateWindow(
            app -> appName,
            windowWidth, 
            windowHeight,  
            windowFlags
            );

    /*
     * Window Creation Error Handling
     */
    if ( app -> gpx_window == NULL )
    {
        SDL_Log("Application Window Creation Failure: %s", SDL_GetError());
        return -1;
    }

    /*
     * Claiming the application window and creating a swap chain for the window
     */
    if ( !SDL_ClaimWindowForGPUDevice( app -> gpx_device, app -> gpx_window ) )
    {
        SDL_Log("Cannot Claim Window: %s", SDL_GetError());
        return -1;
    }

    return 0;
}

/**
 * @brief Call to destroy our resources in reverse order of creation
 * @param app The application which processes you want to close
 *
 * On application quit, we want to release the window from the GPU Device, destroy the window, the destroy the device
 */
void graphix::graphix_quit(gpx::app_state* app) 
{
    SDL_ReleaseWindowFromGPUDevice( app -> gpx_device, app -> gpx_window );
    SDL_DestroyWindow( app -> gpx_window );
    SDL_DestroyGPUDevice( app -> gpx_device );
}

/**
 * Storage for the to the applications base file path
 */
static const char* base_path;

/**
 * @brief initializes the asset path variables
 *
 * Add different assets to load as needed
 */
void graphix::init_asset_loader()
{
    base_path = SDL_GetBasePath();
}

/**
 * @brief Loads a shader with initialized GPU Device context.
 *
 * Determines the shader stage from .vert or .frag in file name.
 *
 * Queries the supported shader formats for the device then finds a matching
 * shader file path, format, and entry point.
 *
 * Loads the shader file from disk and fills SDL_GPUShaderCreateInfo with
 * the shader code, entry point, stage, format and resource counts.
 *
 * Creates a shader object on the GPU device
 *
 * Frees the memory allocated for the shader code after creation attempt
 *
 * @param device Handle representing SDL_GPU contetx
 * @param shader_file_name File name of the source shader
 * @param sampler_count Number of samplers defined in the shader
 * @param storage_texture_count Number of storage textures defined in the shader
 * @param storage_buffer_count Number of storage buffers defined in the shader
 * @param uniform_buffer_count Number of uniform buffers defined in the shader
 *
 * @return SDL_GPUShader* on sucess
 * @return NULL on failure
 */
SDL_GPUShader* gpx::load_shader(
        SDL_GPUDevice* gpx_device,
        const char* shader_file_name,
        std::uint32_t sampler_count,
        std::uint32_t storage_texture_count,
        std::uint32_t storage_buffer_count,
        std::uint32_t uniform_buffer_count
        ) {
    SDL_GPUShaderStage graphix_shader_stage;

    if ( SDL_strstr(shader_file_name, ".vert") )
    {
        graphix_shader_stage = SDL_GPU_SHADERSTAGE_VERTEX;
    }

    else if ( SDL_strstr(shader_file_name, ".frag") )
    {
        graphix_shader_stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    }

    else 
    {
        SDL_Log("Invalid Shader Stage!: %s", SDL_GetError());
        return NULL;
    }

    char shader_full_path[256];
    SDL_GPUShaderFormat supported_shader_formats = SDL_GetGPUShaderFormats(gpx_device);
    SDL_GPUShaderFormat graphix_shader_format = SDL_GPU_SHADERFORMAT_INVALID;
    const char* graphix_entry_point;

    if ( supported_shader_formats & SDL_GPU_SHADERFORMAT_DXIL )
    {
        SDL_snprintf(
                shader_full_path, 
                sizeof(shader_full_path), 
                gpx_shaders::compiled_DXIL().c_str(),
                base_path, shader_file_name
                );
        graphix_shader_format = SDL_GPU_SHADERFORMAT_DXIL;
        graphix_entry_point = "main";
    }
    else if ( supported_shader_formats & SDL_GPU_SHADERFORMAT_SPIRV )
    {
        SDL_snprintf(
                shader_full_path,
                sizeof(shader_full_path),
                gpx_shaders::compiled_SPIRV().c_str(),
                base_path, shader_file_name
                );
        graphix_shader_format = SDL_GPU_SHADERFORMAT_SPIRV;
        graphix_entry_point = "main";
    }
    else if ( supported_shader_formats & SDL_GPU_SHADERFORMAT_MSL )
    {
        SDL_snprintf(
                shader_full_path, 
                sizeof(shader_full_path), 
                gpx_shaders::compiled_MSL().c_str(),
                base_path, shader_file_name
                );
        graphix_shader_format = SDL_GPU_SHADERFORMAT_MSL;
        graphix_entry_point = "main0";
    }
    else 
    {
        SDL_Log("Invalid or missing shader format!: %s", SDL_GetError());
        return NULL;
    }


    std::size_t shader_alloc_size;
    void* shader_code = SDL_LoadFile(shader_full_path, &shader_alloc_size);
    if ( shader_code == NULL )
    {
        SDL_Log("Failed to load shader from disk: %s", SDL_GetError());
        return NULL;
    }

    SDL_GPUShaderCreateInfo graphix_shader_info = {
        .code_size = shader_alloc_size,
        .code = static_cast<const std::uint8_t*>(shader_code),
        .entrypoint = graphix_entry_point,
        .format = graphix_shader_format,
        .stage = graphix_shader_stage,
        .num_samplers = sampler_count,
        .num_storage_textures = storage_texture_count,
        .num_storage_buffers = storage_buffer_count,
        .num_uniform_buffers = uniform_buffer_count,
    };

    SDL_GPUShader* graphix_shader = SDL_CreateGPUShader(gpx_device, &graphix_shader_info);
    if ( graphix_shader == NULL )
    {
        SDL_Log("Shader creation failed: %s", SDL_GetError());
        SDL_free(shader_code);
        return NULL;
    }
     SDL_free(shader_code);
    return graphix_shader;
}
