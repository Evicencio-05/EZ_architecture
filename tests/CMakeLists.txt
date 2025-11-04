find_package(GTest REQUIRED)

# Test executables
add_executable(test_register_file test_register_file.cpp)
target_link_libraries(test_register_file ez_arch_core GTest::gtest GTest::gtest_main)

add_executable(test_memory test_memory.cpp)
target_link_libraries(test_memory ez_arch_core GTest::gtest GTest::gtest_main)

# Add tests
add_test(NAME RegisterFileTest COMMAND test_register_file)
add_test(NAME MemoryTest COMMAND test_memory)
