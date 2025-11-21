# Linenoise library (optional)
if(USE_LINENOISE)
  add_library(linenoise STATIC
        ${CMAKE_SOURCE_DIR}/external/linenoise.c
    )
  target_include_directories(linenoise PUBLIC ${CMAKE_SOURCE_DIR}/external)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(linenoise PRIVATE -w)
  endif()
endif()

# SFML GUI version (optional)
if(ENABLE_GUI)
  include(FetchContent)

  set(SFML_USE_SYSTEM_DEPS   ON CACHE BOOL "" FORCE)
  set(SFML_BUILD_AUDIO       FALSE CACHE BOOL "" FORCE)
  set(SFML_BUILD_NETWORK     FALSE CACHE BOOL "" FORCE)

  FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
    )
  FetchContent_MakeAvailable(SFML)
endif()

# Core library (shared by CLI, GUI and tests)
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

set_source_files_properties(input_handler.cpp PROPERTIES
    SKIP_LINTING ON
)
target_include_directories(ez_arch_core PUBLIC ${CMAKE_SOURCE_DIR}/include)

if(USE_LINENOISE)
  target_compile_definitions(ez_arch_core PUBLIC USE_LINENOISE)
  target_link_libraries(ez_arch_core PUBLIC linenoise)
endif()

if(ENABLE_GUI)
  target_compile_definitions(ez_arch_core PUBLIC ENABLE_GUI)
  target_link_libraries(ez_arch_core PUBLIC SFML::Graphics SFML::Window SFML::System)

  add_executable(ez_architecture_gui
        gui/main_gui.cpp
        gui/cpu_visualizer.cpp
        gui/register_view.cpp
        gui/memory_view.cpp
        gui/instruction_view.cpp
        gui/datapath_view.cpp
        gui/button.cpp
        gui/instruction_cache.cpp
        gui/instruction_builder_view.cpp
        gui/instruction_queue_view.cpp
    )
  target_link_libraries(ez_architecture_gui PRIVATE ez_arch_core)
endif()

# CLI executable
add_executable(ez_architecture_cli
    cli/main_cli.cpp
)
target_link_libraries(ez_architecture_cli PRIVATE ez_arch_core)
