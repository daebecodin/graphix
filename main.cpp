#include <SDL3/SDL.h>
#include <vector>

#include "demo.h"

namespace gpx_demo = graphix::demo;

static std::vector<gpx_demo::Demo> demos;


int main()
{
    demos.push_back(gpx_demo::clear_screen_demo);
    return 0;

}
