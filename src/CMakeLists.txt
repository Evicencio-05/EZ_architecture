# Linenoise library (optional)
if(USE_LINENOISE)
    add_library(linenoise STATIC
        ${CMAKE_SOURCE_DIR}/external/linenoise.c
    )
    target_include_directories(linenoise PUBLIC
        ${CMAKE_SOURCE_DIR}/external
    )
    # Disable warnings for external code
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(linenoise PRIVATE -w)
    endif()
endif()

# Core library
add_library(ez_arch_core STATIC
    core/register_file.cpp
    core/memory.cpp
    core/alu.cpp
    core/instruction.cpp
    core/decoder.cpp
    core/types.cpp
    core/cpu.cpp
    cli/command_parser.cpp
    cli/output_formatter.cpp
    cli/input_handler.cpp
)

target_include_directories(ez_arch_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

if(USE_LINENOISE)
    target_compile_definitions(ez_arch_core PUBLIC USE_LINENOISE)
    target_link_libraries(ez_arch_core PUBLIC linenoise)
endif()

add_executable(ez_architecture_gui
    gui/main_gui.cpp
    gui/cpu_visualizer.cpp
    gui/register_view.cpp
    gui/memory_view.cpp
    gui/instruction_view.cpp
    gui/datapath_view.cpp
    gui/button.cpp
)

target_link_libraries(ez_architecture_gui
    ez_arch_core
    SFML::Graphics
    SFML::Window
    SFML::System
)

# CLI executable
add_executable(ez_architecture_cli
    cli/main_cli.cpp
    cli/command_parser.cpp
    cli/output_formatter.cpp
)

target_link_libraries(ez_architecture_cli
    ez_arch_core
)
