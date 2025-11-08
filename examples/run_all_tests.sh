#!/bin/bash
# Batch test runner for EZ Architecture examples
# Run all test programs and show their output

echo "======================================"
echo "EZ Architecture Test Suite"
echo "======================================"
echo ""

# Check if CLI exists
if [ ! -f ./build/bin/ez_architecture_cli ]; then
    echo "Error: CLI not found at ./build/bin/ez_architecture_cli"
    echo "Please build the project first: ./build.sh"
    exit 1
fi

# Counter for tests
total=0
passed=0
failed=0

# Run each test file
for test in examples/test_*.txt; do
    # Skip template and README
    if [[ "$test" == *"TEMPLATE"* ]]; then
        continue
    fi
    
    total=$((total + 1))
    echo "--------------------------------------"
    echo "Running: $test"
    echo "--------------------------------------"
    
    # Run the test and capture exit code
    if ./build/bin/ez_architecture_cli < "$test" > /dev/null 2>&1; then
        echo "✓ PASSED"
        passed=$((passed + 1))
    else
        echo "✗ FAILED (exit code: $?)"
        failed=$((failed + 1))
    fi
    echo ""
done

echo "======================================"
echo "Test Results"
echo "======================================"
echo "Total:  $total"
echo "Passed: $passed"
echo "Failed: $failed"
echo ""

if [ $failed -eq 0 ]; then
    echo "All tests passed! ✓"
    exit 0
else
    echo "Some tests failed."
    exit 1
fi
