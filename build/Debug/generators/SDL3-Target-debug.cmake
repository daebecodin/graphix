# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(sdl_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(sdl_FRAMEWORKS_FOUND_DEBUG "${sdl_FRAMEWORKS_DEBUG}" "${sdl_FRAMEWORK_DIRS_DEBUG}")

set(sdl_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET sdl_DEPS_TARGET)
    add_library(sdl_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET sdl_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${sdl_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${sdl_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:SDL3::Headers;opengl::opengl>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### sdl_DEPS_TARGET to all of them
conan_package_library_targets("${sdl_LIBS_DEBUG}"    # libraries
                              "${sdl_LIB_DIRS_DEBUG}" # package_libdir
                              "${sdl_BIN_DIRS_DEBUG}" # package_bindir
                              "${sdl_LIBRARY_TYPE_DEBUG}"
                              "${sdl_IS_HOST_WINDOWS_DEBUG}"
                              sdl_DEPS_TARGET
                              sdl_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "sdl"    # package_name
                              "${sdl_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${sdl_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT SDL3::SDL3 #############

        set(sdl_SDL3_SDL3_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(sdl_SDL3_SDL3_FRAMEWORKS_FOUND_DEBUG "${sdl_SDL3_SDL3_FRAMEWORKS_DEBUG}" "${sdl_SDL3_SDL3_FRAMEWORK_DIRS_DEBUG}")

        set(sdl_SDL3_SDL3_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET sdl_SDL3_SDL3_DEPS_TARGET)
            add_library(sdl_SDL3_SDL3_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET sdl_SDL3_SDL3_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'sdl_SDL3_SDL3_DEPS_TARGET' to all of them
        conan_package_library_targets("${sdl_SDL3_SDL3_LIBS_DEBUG}"
                              "${sdl_SDL3_SDL3_LIB_DIRS_DEBUG}"
                              "${sdl_SDL3_SDL3_BIN_DIRS_DEBUG}" # package_bindir
                              "${sdl_SDL3_SDL3_LIBRARY_TYPE_DEBUG}"
                              "${sdl_SDL3_SDL3_IS_HOST_WINDOWS_DEBUG}"
                              sdl_SDL3_SDL3_DEPS_TARGET
                              sdl_SDL3_SDL3_LIBRARIES_TARGETS
                              "_DEBUG"
                              "sdl_SDL3_SDL3"
                              "${sdl_SDL3_SDL3_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET SDL3::SDL3
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_LIBRARIES_TARGETS}>
                     )

        if("${sdl_SDL3_SDL3_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET SDL3::SDL3
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         sdl_SDL3_SDL3_DEPS_TARGET)
        endif()

        set_property(TARGET SDL3::SDL3 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET SDL3::SDL3 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET SDL3::SDL3 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_LIB_DIRS_DEBUG}>)
        set_property(TARGET SDL3::SDL3 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET SDL3::SDL3 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${sdl_SDL3_SDL3_COMPILE_OPTIONS_DEBUG}>)


    ########## COMPONENT SDL3::Headers #############

        set(sdl_SDL3_Headers_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(sdl_SDL3_Headers_FRAMEWORKS_FOUND_DEBUG "${sdl_SDL3_Headers_FRAMEWORKS_DEBUG}" "${sdl_SDL3_Headers_FRAMEWORK_DIRS_DEBUG}")

        set(sdl_SDL3_Headers_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET sdl_SDL3_Headers_DEPS_TARGET)
            add_library(sdl_SDL3_Headers_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET sdl_SDL3_Headers_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'sdl_SDL3_Headers_DEPS_TARGET' to all of them
        conan_package_library_targets("${sdl_SDL3_Headers_LIBS_DEBUG}"
                              "${sdl_SDL3_Headers_LIB_DIRS_DEBUG}"
                              "${sdl_SDL3_Headers_BIN_DIRS_DEBUG}" # package_bindir
                              "${sdl_SDL3_Headers_LIBRARY_TYPE_DEBUG}"
                              "${sdl_SDL3_Headers_IS_HOST_WINDOWS_DEBUG}"
                              sdl_SDL3_Headers_DEPS_TARGET
                              sdl_SDL3_Headers_LIBRARIES_TARGETS
                              "_DEBUG"
                              "sdl_SDL3_Headers"
                              "${sdl_SDL3_Headers_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET SDL3::Headers
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_LIBRARIES_TARGETS}>
                     )

        if("${sdl_SDL3_Headers_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET SDL3::Headers
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         sdl_SDL3_Headers_DEPS_TARGET)
        endif()

        set_property(TARGET SDL3::Headers APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET SDL3::Headers APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET SDL3::Headers APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_LIB_DIRS_DEBUG}>)
        set_property(TARGET SDL3::Headers APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET SDL3::Headers APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${sdl_SDL3_Headers_COMPILE_OPTIONS_DEBUG}>)


    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET sdl::sdl APPEND PROPERTY INTERFACE_LINK_LIBRARIES SDL3::SDL3)
    set_property(TARGET sdl::sdl APPEND PROPERTY INTERFACE_LINK_LIBRARIES SDL3::Headers)

########## For the modules (FindXXX)
set(sdl_LIBRARIES_DEBUG sdl::sdl)
