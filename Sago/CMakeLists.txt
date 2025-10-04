cmake_minimum_required(VERSION 3.20)
project(SGEngine)

option(SAGA_BUILD_SHARED "build shared engine library" OFF)
option(SAGA_BUILD_STATIC "build static engine library" ON)

set(SAGA_ENGINE_NAME SagaEngine)

find_package(Vulkan REQUIRED)
message(STATUS "Vulkan include dirs: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan libraries: ${Vulkan_LIBRARIES}")

set(ENGINE_SOURCES "")

macro(add_engine_module MODULE_NAME)
    file(GLOB MODULE_SOURCES "${MODULE_NAME}/*.cpp" "${MODULE_NAME}/*.c")
    list(APPEND ENGINE_SOURCES ${MODULE_SOURCES})
endmacro()

#core
add_engine_module(core/io/log)
add_engine_module(core/util)
## memory
add_engine_module(core/memory)
add_engine_module(core/memory/pool)
add_engine_module(core/memory/lockfree)
add_engine_module(core/memory/buffer)

#drivers
add_engine_module(drivers/vulkan)
add_engine_module(drivers/vulkan/util)
add_engine_module(drivers/vulkan/tools)
add_engine_module(drivers/vulkan/memory)
add_engine_module(drivers/vulkan/extensions)
add_engine_module(drivers/vulkan/pipelines)
add_engine_module(drivers/vulkan/pipelines/shaders)
add_engine_module(drivers/vulkan/renderpass)
add_engine_module(drivers/vulkan/commands)


#context
add_engine_module(context)
add_engine_module(context/controller)
add_engine_module(context/renderer)


message(STATUS "PROJECT_ROOT_DIR = ${PROJECT_ROOT_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(
    ${PROJECT_ROOT_DIR}/3rdparty/volk
    ${CMAKE_BINARY_DIR}/3rdparty-build/volk  
    )
#platform
add_subdirectory(
    ${PROJECT_ROOT_DIR}/platform/External
    ${CMAKE_BINARY_DIR}/platform-External/External  
)
#glm
add_subdirectory(
    ${PROJECT_ROOT_DIR}/3rdparty/glm
    ${CMAKE_BINARY_DIR}/3rdparty-build/glm  
)

#vma
add_subdirectory(
    ${PROJECT_ROOT_DIR}/3rdparty/vma
    ${CMAKE_BINARY_DIR}/3rdparty-build/vma  
)


option(GLM_BUILD_LIBRARY "Build dynamic/static library" OFF)
option(GLM_BUILD_TESTS "Build the test programs" OFF)
option(GLM_BUILD_INSTALL "Generate the install target" OFF)
option(GLM_ENABLE_CXX_20 "Enable C++ 20" ON)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86" OR CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
    option(GLM_ENABLE_SIMD_SSE2 "Enable SSE2 optimizations" ON)  
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    option(GLM_ENABLE_SIMD_NEON "Enable ARM NEON optimizations" ON)
endif()

option(GLM_ENABLE_FAST_MATH "Enable fast math optimizations" ON)



macro(attach_config target_name)

    target_include_directories(${target_name} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}.
    )

    target_link_libraries(${target_name} PUBLIC
        Vulkan::Vulkan
        glm::glm
        volk::volk
        VulkanMemoryAllocator
    )
    
    target_link_libraries(${target_name} PRIVATE
        PlatformInterface
        SagaPlatform
    )
    
    target_compile_features(${target_name} PUBLIC cxx_std_20)

    if (ANDROID)
        target_compile_definitions(${target_name} PUBLIC SAGA_PLATFORM_ANDROID)
    elseif(WIN32)
        target_compile_definitions(${target_name} PUBLIC SAGA_PLATFORM_WIN32)
    elseif(LINUX)
        target_compile_definitions(${target_name} PUBLIC SAGA_PLATFORM_LINUX)
    endif()
endmacro()

if (SAGA_BUILD_STATIC)
    set(ENGINE_STATIC_NAME ${SAGA_ENGINE_NAME}-static)
    add_library(${ENGINE_STATIC_NAME} STATIC ${ENGINE_SOURCES})
    attach_config(${ENGINE_STATIC_NAME})
    message("build static SAGA engine")
    
elseif(SAGA_BUILD_SHARED)
    set(ENGINE_SHARED_NAME ${SAGA_ENGINE_NAME}-shared)
    add_library(${ENGINE_SHARED_NAME} SHARED ${ENGINE_SOURCES})
    target_compile_definitions(${ENGINE_SHARED_NAME} PUBLIC SAGA_DEF_DLLEXPORT)
    attach_config(${ENGINE_SHARED_NAME})
    message("build shared SAGA engine")
endif()
