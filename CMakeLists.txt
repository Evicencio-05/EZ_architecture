cmake_minimum_required(VERSION 3.31.9)
project(EZ_Architecture VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# LSP Setup
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Build options
option(USE_LINENOISE "Enable linenoise for enhanced CLI" ON)
option(ENABLE_GUI    "Build with SFML GUI version" ON)
option(ENABLE_TESTING "Build and run unit tests" ON)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)

if(ENABLE_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2   # or release-1.15.2
    )
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install rules
if(ENABLE_GUI)
    install(TARGETS ez_architecture_gui RUNTIME DESTINATION bin)
endif()
install(TARGETS ez_architecture_cli RUNTIME DESTINATION bin)

