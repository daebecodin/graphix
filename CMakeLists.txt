cmake_minimum_required(VERSION 3.27)
project(sdl_playground LANGUAGES CXX)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/SDLPlayground.cmake")

find_package(SDL3 CONFIG REQUIRED)

# Conan may name archive tools without adding their directory to the build PATH.
get_filename_component(GRAPHIX_COMPILER_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
foreach(tool IN ITEMS AR RANLIB)
    if(CMAKE_${tool} AND NOT IS_ABSOLUTE "${CMAKE_${tool}}")
        find_program(GRAPHIX_${tool} NAMES "${CMAKE_${tool}}"
            HINTS "${GRAPHIX_COMPILER_BIN}" REQUIRED)
        set(CMAKE_${tool} "${GRAPHIX_${tool}}")
    endif()
endforeach()

# Reusable SDL/GPU setup and shader loading, separate from the demo runner.
add_library(graphix_core STATIC graphix.cpp)
add_library(Graphix::Core ALIAS graphix_core)
target_link_libraries(graphix_core PUBLIC graphix_headers SDL3::SDL3)
target_compile_options(graphix_core PRIVATE
    "$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-Wall;-Wextra;-Wpedantic>"
)

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

# The implementation is compiled once into Graphix::Core.
list(REMOVE_ITEM GRAPHIX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/graphix.cpp")

sdl_playground_add_executable(main SOURCES ${GRAPHIX_SOURCES})
target_link_libraries(main PRIVATE Graphix::Core)

# Refresh runtime assets even when no C++ files need rebuilding.
add_custom_target(copy_shaders ALL
    COMMAND "${CMAKE_COMMAND}" -E make_directory
        "$<TARGET_FILE_DIR:main>/shaders/compiled"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/compiled"
        "$<TARGET_FILE_DIR:main>/shaders/compiled"
    COMMENT "Copying compiled shaders beside the executable"
)
add_dependencies(main copy_shaders)

# Give clangd dependency paths even for files that have no build command yet.
set(SDL_PLAYGROUND_CLANGD_FLAGS "    - '-xc++'\n    - '-std=c++${CMAKE_CXX_STANDARD}'\n")
foreach(include_directory IN LISTS SDL_PLAYGROUND_INCLUDE_DIRS SDL3_INCLUDE_DIRS)
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
