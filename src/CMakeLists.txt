# Core library
add_library(ez_arch_core STATIC
    core/register_file.cpp
    core/memory.cpp
    core/alu.cpp
    core/instruction.cpp
    core/cpu.cpp
    core/decoder.cpp
)

target_include_directories(ez_arch_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# GUI executable
add_executable(ez_architecture_gui
    gui/main_gui.cpp
    gui/cpu_visualizer.cpp
    gui/register_view.cpp
    gui/memory_view.cpp
    gui/instruction_view.cpp
)

target_link_libraries(ez_architecture_gui
    ez_arch_core
    sfml-graphics
    sfml-window
    sfml-system
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
