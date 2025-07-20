macro(add_core_module MODULE_NAME)
    file(GLOB MODULE_SOURCES "${MODULE_NAME}/*.cpp" "${MODULE_NAME}/*.c")
    list(APPEND DRIVER_SOURCES ${MODULE_SOURCES})
endmacro()

add_core_module(vulkan)

add_library(SG_Driver  STATIC 
${DRIVER_SOURCES})

if(NOT DRIVER_SOURCES)
    message(FATAL_ERROR "No source files found in ${CMAKE_CURRENT_SOURCE_DIR}/io")
endif()

target_include_directories(SG_Driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

find_package(Vulkan REQUIRED)
target_link_libraries(SG_Driver PRIVATE Vulkan::Vulkan)

