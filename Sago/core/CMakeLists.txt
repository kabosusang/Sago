macro(add_core_module MODULE_NAME)
    file(GLOB MODULE_SOURCES "${MODULE_NAME}/*.cpp" "${MODULE_NAME}/*.c")
    list(APPEND CORE_SOURCES ${MODULE_SOURCES})
endmacro()

add_core_module(io/log)
add_core_module(util)

add_library(SG_Core  STATIC 
${CORE_SOURCES})

if(NOT CORE_SOURCES)
    message(FATAL_ERROR "No source files found in ${CMAKE_CURRENT_SOURCE_DIR}/io")
endif()

target_include_directories(SG_Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

