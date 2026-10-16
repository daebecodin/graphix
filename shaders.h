#pragma once
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <cstdint>
#include <string>
#include "graphix.h"


namespace graphix::shader_paths {

    /**
     * @return compiled DXIL shader path
     */
    inline std::string compiled_DXIL()
    {
        std::string str = "%sshaders/compiled/DXIL/%s.dxil";
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
     * @return compiled SPIRV shader path
     */
    inline std::string compiled_SPIRV()
    {
        std::string str = "%sshaders/compiled/SPIRV/%s.spv";
        return str;
    }
}
