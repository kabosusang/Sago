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

# Engine
add_subdirectory(Sago)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${PROJECT_NAME})

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/main)
set(SOURCES)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "${SOURCE_DIR}/*.cpp"
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan
SG_Core
)



