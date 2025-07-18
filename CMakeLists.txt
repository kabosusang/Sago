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


set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Sego/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Sego/include)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${INCLUDE_DIR}/*.h"
    "${INCLUDE_DIR}/*.hpp"
    "${INCLUDE_DIR}/*.hxx"
)

set(SOURCES)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "${SOURCE_DIR}/*.cpp"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_sources(${PROJECT_NAME} PRIVATE ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${INCLUDE_DIR}>  # 构建时包含
  # $<INSTALL_INTERFACE:include>       # 安装后包含（可选）
)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)



