# EZ Architecture Examples - Summary

This document summarizes all the test programs and templates created for the EZ Architecture CLI.

## Created Files

### Documentation
- **README.md** - Complete guide to using the examples
- **QUICK_REFERENCE.md** - Quick reference card for CLI commands
- **TEMPLATE_ANNOTATED.md** - Template with explanatory comments (for reference)
- **SUMMARY.md** - This file

### Templates
- **TEMPLATE.txt** - Minimal working template for creating custom test scripts

### Test Programs - Machine Code (.hex files)
- **test_memory.hex** - Memory load/store operations
- **test_branch.hex** - Conditional branching with beq
- **test_loop.hex** - Counting loop with bne

### Test Scripts - Automation (.txt files)
- **test_addition.txt** - Simple arithmetic with watch expressions
- **test_memory.txt** - Memory operations demonstration
- **test_branch.txt** - Branch instruction testing
- **test_loop.txt** - Loop execution with counter
- **test_stages.txt** - Pipeline stage-by-stage execution
- **test_run.txt** - Quick run to completion
- **test_saveload.txt** - State save and restore
- **test_disasm.txt** - Disassembly demonstration
- **test_assembly.txt** - Assembly to machine code conversion

### Utilities
- **run_all_tests.sh** - Batch test runner script (executable)

## Quick Start

```bash
# Run a single test
./build/bin/ez_architecture_cli < examples/test_addition.txt

# Run all tests
./examples/run_all_tests.sh

# Create your own test
cp examples/TEMPLATE.txt examples/my_test.txt
# Edit my_test.txt (no comments!)
./build/bin/ez_architecture_cli < examples/my_test.txt
```

## Test Program Categories

### Beginner Level
1. **test_addition.txt** - Start here! Simple register operations
2. **test_run.txt** - Quick program execution
3. **test_disasm.txt** - Understanding machine code

### Intermediate Level
4. **test_memory.txt** - Memory operations (load/store)
5. **test_branch.txt** - Conditional control flow
6. **test_stages.txt** - Pipeline visualization

### Advanced Level
7. **test_loop.txt** - Iterative execution
8. **test_saveload.txt** - State management
9. **test_assembly.txt** - Creating machine code

## Features Demonstrated

| Feature | Test Programs |
|---------|---------------|
| Watch expressions | All test programs |
| Register operations | test_addition, test_memory |
| Memory operations | test_memory |
| Branching | test_branch, test_loop |
| Pipeline stages | test_stages |
| Run to completion | test_run, test_loop |
| State save/load | test_saveload |
| Disassembly | test_disasm |
| Assembly | test_assembly |

## Machine Code Programs

### test_memory.hex
- Demonstrates: Load/store word operations
- Instructions: addi, sw, lw
- Memory addresses: 0x1000

### test_branch.hex
- Demonstrates: Conditional branching
- Instructions: addi, beq
- Behavior: Skips instructions when condition is true

### test_loop.hex
- Demonstrates: Loop execution
- Instructions: addi, bne
- Behavior: Counts from 0 to 5

## Usage Patterns

### Pattern 1: Step-by-Step Debugging
```
load program.hex
watch $8
watch $9
step
registers
step
registers
```

### Pattern 2: Watch Memory Changes
```
load program.hex
watch 0x1000
watch 0x2000
memory 0x1000 0x2010
run
memory 0x1000 0x2010
```

### Pattern 3: Pipeline Analysis
```
load program.hex
stage
stage
stage
stage
stage
registers
```

### Pattern 4: Save Checkpoints
```
load program.hex
step 5
save checkpoint1.bin
step 5
save checkpoint2.bin
loadstate checkpoint1.bin
```

## Interactive vs. Scripted

### Scripted Execution
- Automated testing
- Reproducible results
- Quick experimentation
- No user input required

```bash
./build/bin/ez_architecture_cli < test_script.txt
```

### Interactive After Script
- Run script first
- Then continue manually
- Best of both worlds

```bash
cat examples/test_addition.txt - | ./build/bin/ez_architecture_cli
```

## Important Notes

1. **No Comments in Scripts**: The CLI does not parse comments. Only include commands.
2. **Watch Expressions**: Automatically display after step/stage/run commands
3. **Empty Lines**: Empty lines are ignored and safe to use
4. **Exit Codes**: All test scripts properly exit with `quit` command
5. **File Paths**: Use relative or absolute paths for loading programs

## Next Steps

1. **Try the examples**: Start with test_addition.txt
2. **Modify templates**: Copy TEMPLATE.txt and customize
3. **Create programs**: Write your own .hex files
4. **Combine features**: Mix and match different commands
5. **Share examples**: Add your own test programs to this directory

## Benefits

✓ **Time Saving**: No need to retype commands repeatedly  
✓ **Reproducible**: Same inputs produce same outputs  
✓ **Educational**: Learn by example  
✓ **Flexible**: Easily modify and extend  
✓ **Testable**: Verify functionality automatically  

## Contributing

To add new examples:
1. Create a .hex file with your program (if needed)
2. Create a .txt file with test commands (no comments!)
3. Test it: `./build/bin/ez_architecture_cli < your_test.txt`
4. Document it in examples/README.md

Happy experimenting!
