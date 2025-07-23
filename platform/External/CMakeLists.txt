add_library(PlatformInterface INTERFACE)

target_include_directories(PlatformInterface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Interface
)

target_compile_features(PlatformInterface INTERFACE cxx_std_20)