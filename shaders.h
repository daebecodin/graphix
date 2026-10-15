#pragma once
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <cstdint>
#include <string>
#include "graphix.h"


namespace graphix::shader_paths {

    /**
     * @return compiled DIXL shader path
     */
    inline std::string compiled_DIXL() 
    {
        std::string str = "%sshaders/compiled/DXIL/%s.dixl";
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
        std::string str = "%sshaders/compiled/SPVIR/%s.spv";
        return str;
    }
}
