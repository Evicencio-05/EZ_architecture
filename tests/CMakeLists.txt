add_executable(ez_architecture_tests
    test_alu.cpp
    test_cpu.cpp
    test_command_parser.cpp
    test_instruction.cpp
    test_memory.cpp
    test_register_file.cpp
)

target_link_libraries(ez_architecture_tests PRIVATE
    ez_arch_core
    gtest_main
)

target_compile_features(ez_architecture_tests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(ez_architecture_tests)