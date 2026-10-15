include_guard(GLOBAL)

set(
    SDL_PLAYGROUND_CXX_STANDARD
    20
    CACHE STRING
    "C++ language standard used by SDL playground targets"
)

set(CMAKE_CXX_STANDARD "${SDL_PLAYGROUND_CXX_STANDARD}")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

get_filename_component(
    SDL_PLAYGROUND_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/.."
    ABSOLUTE
)

set(SDL_PLAYGROUND_INCLUDE_DIRS
    "${SDL_PLAYGROUND_ROOT}"
    "${SDL_PLAYGROUND_ROOT}/include"
)

if(NOT TARGET graphix_headers)
    add_library(graphix_headers INTERFACE)
    target_include_directories(graphix_headers INTERFACE ${SDL_PLAYGROUND_INCLUDE_DIRS})
endif()

function(sdl_playground_add_executable target_name)
    cmake_parse_arguments(ARG "" "" "SOURCES" ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "${target_name} requires at least one source file")
    endif()

    add_executable("${target_name}" ${ARG_SOURCES})
    target_link_libraries("${target_name}" PRIVATE graphix_headers SDL3::SDL3)
    target_compile_options(
        "${target_name}"
        PRIVATE
            "$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-Wall;-Wextra;-Wpedantic>"
    )
endfunction()

function(sdl_playground_add_all_executables)
    cmake_parse_arguments(ARG "" "" "EXCLUDE" ${ARGN})

    file(
        GLOB_RECURSE source_files
        CONFIGURE_DEPENDS
        "${SDL_PLAYGROUND_ROOT}/*.cpp"
    )

    # Ignore generated sources when the build directory is inside the project.
    list(
        FILTER source_files
        EXCLUDE REGEX "/(impl|build|cmake-build-[^/]*)/"
    )

    foreach(source_file IN LISTS source_files)
        get_filename_component(executable_name "${source_file}" NAME_WLE)
        list(FIND ARG_EXCLUDE "${executable_name}" excluded_index)

        if(excluded_index EQUAL -1)
            if(TARGET "${executable_name}")
                message(
                    FATAL_ERROR
                    "Cannot create executable '${executable_name}': "
                    "another .cpp file has the same filename"
                )
            endif()

            sdl_playground_add_executable(
                "${executable_name}"
                SOURCES "${source_file}"
            )

            file(
                RELATIVE_PATH source_relative_path
                "${SDL_PLAYGROUND_ROOT}"
                "${source_file}"
            )
            get_filename_component(
                source_relative_directory
                "${source_relative_path}"
                DIRECTORY
            )

            if(source_relative_directory)
                set_target_properties(
                    "${executable_name}"
                    PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY
                            "${CMAKE_BINARY_DIR}/${source_relative_directory}"
                )
            endif()
        endif()
    endforeach()
endfunction()
