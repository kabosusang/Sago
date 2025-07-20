macro(add_core_module MODULE_NAME)
    file(GLOB MODULE_SOURCES "${MODULE_NAME}/*.cpp" "${MODULE_NAME}/*.c")
    list(APPEND PLATFORM_SOURCES ${MODULE_SOURCES})
endmacro()

add_core_module(window)

add_library(SG_Platform  STATIC 
${PLATFORM_SOURCES})

if(NOT PLATFORM_SOURCES)
    message(FATAL_ERROR "No source files found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

#sdl3
set(SDL_TEST_LIBRARY OFF)
add_subdirectory(
    ${PROJECT_ROOT_DIR}/3rdparty/sdl3 
    ${CMAKE_BINARY_DIR}/sdl3-build
)


target_include_directories(SG_Platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_ROOT_DIR}/3rdparty/sdl3/include
)

target_link_libraries(SG_Platform 
PRIVATE SDL3::SDL3 SG_Core)










