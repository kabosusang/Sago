cmake_minimum_required(VERSION 3.15)
project(Sago
  VERSION 1.0
  LANGUAGES CXX
)

include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CMAKE_JOB_POOL_COMPILE compile_job_pool)
    set(CMAKE_JOB_POOLS compile_job_pool=${N})
endif()

set(CMAKE_CXX_STANDARD 20)                
set(CMAKE_CXX_STANDARD_REQUIRED ON)       
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)     


set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Engine
add_subdirectory(Sago)

# Platform
add_subdirectory(platform)

# Editor
#add_subdirectory(editor)


# Entry
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${PROJECT_NAME})
file(GLOB_RECURSE SOURCES 
  "main/main.cpp"
  "main/runtime.cpp"
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
SG_Core 
SG_Driver
SG_Platform
)













# Remove Sources
include(platform/cmake/copy_dll.cmake)
copy_sdl_dll(${PROJECT_NAME})

