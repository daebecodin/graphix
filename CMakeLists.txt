cmake_minimum_required(VERSION 3.27)
project(sdl_playground LANGUAGES CXX)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/SDLPlayground.cmake")

find_package(SDL3 CONFIG REQUIRED)

file(
    GLOB_RECURSE GRAPHIX_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Keep generated code and downloaded dependencies out of the application.
list(
    FILTER GRAPHIX_SOURCES
    EXCLUDE REGEX "/(impl|build|cmake-build-[^/]*)/"
)

sdl_playground_add_executable(main SOURCES ${GRAPHIX_SOURCES})

# Give clangd dependency paths even for files that have no build command yet.
set(SDL_PLAYGROUND_CLANGD_FLAGS "    - '-xc++'\n    - '-std=c++${CMAKE_CXX_STANDARD}'\n")
foreach(include_directory IN LISTS SDL3_INCLUDE_DIRS)
    string(APPEND SDL_PLAYGROUND_CLANGD_FLAGS "    - '-I${include_directory}'\n")
endforeach()
if(APPLE)
    execute_process(
        COMMAND xcrun --sdk macosx --show-sdk-path
        OUTPUT_VARIABLE SDL_PLAYGROUND_MACOS_SDK
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
    string(APPEND SDL_PLAYGROUND_CLANGD_FLAGS
        "    - '-isysroot'\n    - '${SDL_PLAYGROUND_MACOS_SDK}'\n")
endif()
configure_file(cmake/clangd.in "${CMAKE_CURRENT_SOURCE_DIR}/.clangd" @ONLY)
