cmake_minimum_required(VERSION 3.27)
project(sdl_playground LANGUAGES CXX)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/SDLPlayground.cmake")

find_package(SDL3 CONFIG REQUIRED)

sdl_playground_add_all_executables()
