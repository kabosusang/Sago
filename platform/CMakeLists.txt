macro(add_core_module MODULE_NAME)
    file(GLOB MODULE_SOURCES "${MODULE_NAME}/*.cpp" "${MODULE_NAME}/*.c")
    list(APPEND PLATFORM_SOURCES ${MODULE_SOURCES})
endmacro()

add_core_module(window)

add_library(SagaPlatform  STATIC 
${PLATFORM_SOURCES})

if(NOT PLATFORM_SOURCES)
    message(FATAL_ERROR "No source files found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

#sdl3
set(SDL_CAMERA OFF)
set(SDL_EXAMPLES OFF)
set(SDL_GPU OFF)
set(SDL_RENDER OFF)
set(SDL_HAPTIC OFF)
set(SDL_AUDIO OFF)
set(SDL_TESTS OFF)
set(SDL_TEST_LIBRARY OFF)
set(SDL_INSTALL OFF)
set(SDL_DISABLE_INSTALL ON)
set(SDL_DISABLE_UNINSTALL ON)
if (ANDROID)
    set(SDL_SHARED ON)
    set(SDL_STATIC OFF)
else()
    set(SDL_SHARED OFF)
    set(SDL_STATIC ON)
endif()

add_subdirectory(
    ${PROJECT_ROOT_DIR}/3rdparty/sdl3 
    ${CMAKE_BINARY_DIR}/sdl3-build
)

target_include_directories(SagaPlatform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_ROOT_DIR}/3rdparty/sdl3/include
)

target_link_libraries(
SagaPlatform 
PRIVATE
SagaEngine-static
SDL3::SDL3)










