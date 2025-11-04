cmake_minimum_required(VERSION 3.15)
project(EZ_Architecture VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find SFML
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Enable testing
enable_testing()

# Subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Installation rules (commented out until executables are implemented)
# install(TARGETS ez_architecture_gui ez_architecture_cli
#         RUNTIME DESTINATION bin)
