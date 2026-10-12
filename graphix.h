#pragma once

#include <SDL3/SDL.h>
#include <cstdint>
#include <string>

namespace graphix {

    /**
     * @brief Structure to hold the state of our rendered application
     *
     * Holds the state for the application and running example
     */
    struct app_state {
        /**
         * @brief Application name
         */
        const char* appName;
        /**
         * @brief Application base file path
         */
        const char* basePath;
        /**
         * @brief Application window
         */
        SDL_Window* window;
        /**
         * @brief Application GPU Device
         */
        SDL_GPUDevice* device;
        /**
         * @brief Change between each frame
         */
        float deltaTime;

    };

    /**
     * @brief Initialize an app with a window and GPU device
     * @param app program to run
     * @param window flags
     * @return
     */
    int graphix_init(app_state* app, SDL_WindowFlags);

    /**
     * @brief Closes the application resource buffers
     * @param app program to close
     */
    void graphix_quit(app_state* app);

    /**
     * @brief Call to initialize an asset loader
     */
    void init_asset_loader();

    /**
     * @brief Initializes a specified shader from its hlsl file.
     * @param device SDL_GPUDevice
     * @param shader_file_name Name of the shader file
     * @param sampler_count Number of samplers defined in shader
     * @param storage_texture_count Number if storage textures defined in shader
     * @param storage_buffer_count Number of storage buffers defined in shader
     * @param uniform_buffer_count Number of uniform buffers defined in shader
     * @return
     */
    SDL_GPUShader* load_shader(
            SDL_GPUDevice* device,
            const char* shader_file_ame,
            std::uint32_t sampler_count,
            std::uint32_t storage_texture_count,
            std::uint32_t storage_buffer_count,
            std::uint32_t uniform_buffer_count

            );


}

namespace graphix::shader_paths {

    /**
     * @return compiled DIXL shader path
     */
    inline std::string compiled_DIXL() 
        {
            std::string str = "%sshaders/compiled/DIXL/%s.dixl";
            return str;
        }

    /**
     * @return compiled MSL shader path
     */
    inline std::string compiled_MSL() 
        {
            std::string str = "%sshaders/compiled/MSL/%s.msl";
            return str;
        }

    /**
     * @return compiled SPIVR shader path
     */
    inline std::string compiled_SPIVR()
    {
        std::string str = "%sshaders/compiled/SPIVR/%s.spv";
        return str;
    }
}
