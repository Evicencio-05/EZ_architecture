# Enable testing
enable_testing()

# Find Google Test (optional, can be replaced with other frameworks)
# Uncomment when you have a testing framework installed
# find_package(GTest REQUIRED)

# Test executables
add_executable(test_alu test_alu.cpp)
target_link_libraries(test_alu ez_arch_core)

add_executable(test_memory test_memory.cpp)
target_link_libraries(test_memory ez_arch_core)

add_executable(test_register_file test_register_file.cpp)
target_link_libraries(test_register_file ez_arch_core)

add_executable(test_instruction test_instruction.cpp)
target_link_libraries(test_instruction ez_arch_core)

add_executable(test_cpu test_cpu.cpp)
target_link_libraries(test_cpu ez_arch_core)

# Add tests
add_test(NAME ALUTest COMMAND test_alu)
add_test(NAME MemoryTest COMMAND test_memory)
add_test(NAME RegisterFileTest COMMAND test_register_file)
add_test(NAME InstructionTest COMMAND test_instruction)
add_test(NAME CPUTest COMMAND test_cpu)
