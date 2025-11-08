# EZ Architecture Examples

This directory contains pre-configured test programs and scripts to help you experiment with the EZ Architecture MIPS simulator without manually entering commands every time.

## Quick Start

Run any example by piping it to the CLI:

```bash
cat examples/test_addition.txt | ./build/bin/ez_architecture_cli
```

Or use input redirection:

```bash
./build/bin/ez_architecture_cli < examples/test_addition.txt
```

## Available Test Programs

### Basic Operations

- **test_addition.txt** / test_program.hex
  - Simple arithmetic operations (addition)
  - Demonstrates basic register operations with watch expressions
  - Good starting point for beginners

- **test_memory.txt** / test_memory.hex
  - Load and store operations
  - Watches memory locations and registers
  - Shows how data moves between registers and memory

### Control Flow

- **test_branch.txt** / test_branch.hex
  - Conditional branching with `beq`
  - Demonstrates how branches change program flow
  - Shows PC jumping over instructions

- **test_loop.txt** / test_loop.hex
  - Simple counting loop using `bne`
  - Demonstrates repeated execution
  - Shows iterative program behavior

### Execution Modes

- **test_stages.txt**
  - Demonstrates pipeline stage-by-stage execution
  - Helps understand the 5-stage pipeline (Fetch, Decode, Execute, Memory, Writeback)
  - Educational tool for learning pipeline concepts

- **test_run.txt**
  - Executes program to completion without stepping
  - Useful for quick testing
  - Shows final state with watch expressions

### Advanced Features

- **test_saveload.txt**
  - Demonstrates saving and loading CPU state
  - Useful for debugging and checkpointing
  - Shows state persistence

- **test_disasm.txt**
  - Disassembles instructions to see assembly code
  - Helps understand instruction encoding
  - Shows relationship between machine code and assembly

- **test_assembly.txt**
  - Assembles MIPS instructions to machine code
  - Useful for creating programs
  - Interactive assembly learning

## Creating Your Own Test Programs

**IMPORTANT:** The CLI does not support comments in script files. Lines starting with `#` will be treated as unknown commands. Only include actual commands in your script files.

Use **TEMPLATE.txt** as a starting point for creating custom test programs, or see **TEMPLATE_ANNOTATED.md** for a version with explanatory comments.

### Step 1: Copy the template

```bash
cp examples/TEMPLATE.txt examples/my_test.txt
```

### Step 2: Edit your test program

Open `examples/my_test.txt` and customize it:

1. **Load a program**: Replace `your_program.hex` with your actual program file
2. **Add watches**: Add `watch $X` for registers or `watch 0xADDR` for memory
3. **Execute**: Use `step`, `step N`, `stage`, or `run` commands
4. **Inspect**: Use `registers`, `memory`, `pc`, `disasm` to view state
5. **Save** (optional): Use `save` to checkpoint your state

### Step 3: Run your test

```bash
./build/bin/ez_architecture_cli < examples/my_test.txt
```

## Creating Programs (.hex files)

Machine code programs are stored as hex files with one instruction per line.

### Format

```
# Comments start with #
01234567  # Each line is one 32-bit instruction in hex
89ABCDEF
00000000  # Halt instruction
```

### Getting Machine Code

You can use the `asm` command to convert assembly to machine code:

```bash
echo "asm add \$t0, \$t1, \$t2" | ./build/bin/ez_architecture_cli
```

Then copy the output into your .hex file.

### Example Program Creation

Create `examples/my_program.hex`:

```
# Add two numbers
20080005  # addi $t0, $zero, 5
2009000A  # addi $t1, $zero, 10
01095020  # add $t2, $t0, $t1
00000000  # halt
```

Create `examples/my_program.txt`:

```
load examples/my_program.hex
watch $8
watch $9
watch $10
run
quit
```

Run it:

```bash
./build/bin/ez_architecture_cli < examples/my_program.txt
```

## Tips and Tricks

### Interactive Mode After Script

If you want to run a script and then continue interactively:

```bash
cat examples/test_addition.txt - | ./build/bin/ez_architecture_cli
```

The `-` tells cat to read from stdin after the file, allowing you to continue typing commands.

### Batch Testing

Create a script to run multiple tests:

```bash
#!/bin/bash
for test in examples/test_*.txt; do
    echo "Running $test..."
    ./build/bin/ez_architecture_cli < "$test"
    echo "---"
done
```

### Watch Multiple Registers

```
watch $8
watch $9
watch $10
watch $11
```

Then every `step` or `run` command will show all watched values.

### Debug with Save States

```
load my_program.hex
step 5
save checkpoint1.bin
step 5
save checkpoint2.bin
loadstate checkpoint1.bin  # Go back to earlier state
```

### Combine Multiple Programs

```
load program1.hex
run
save state1.bin
reset
load program2.hex
run
save state2.bin
```

## Command Reference

Quick reference for CLI commands:

| Command | Description |
|---------|-------------|
| `load <file>` | Load program from hex file |
| `step [n]` | Execute n instructions (default: 1) |
| `stage` | Execute one pipeline stage |
| `run` | Run until halt |
| `registers` | Show all registers |
| `reg <n>` | Show specific register |
| `memory <start> [end]` | Show memory range |
| `pc` | Show program counter |
| `disasm [addr]` | Disassemble instruction |
| `asm <instruction>` | Assemble instruction to hex |
| `watch <expr>` | Add watch ($reg or 0xaddr) |
| `save <file>` | Save CPU state |
| `loadstate <file>` | Load CPU state |
| `reset` | Reset CPU |
| `quit` | Exit simulator |

## Contributing Examples

Have an interesting test program? Add it to this directory with:
- A `.hex` file with the machine code
- A `.txt` file with the automated test commands
- A comment block explaining what the program does

Happy experimenting!
