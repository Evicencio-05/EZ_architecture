# EZ Architecture CLI Program Template (Annotated)

**IMPORTANT:** The CLI does not support comments in script files. Comments starting with `#` will be treated as commands and cause errors. This file is for reference only.

For a working template, see [TEMPLATE.txt](TEMPLATE.txt) which contains only commands.

## Template Structure

### STEP 1: Load your program
```
load your_program.hex
```
Replace `your_program.hex` with your actual program file path.

### STEP 2: Set up watch expressions (optional)
```
watch $8
watch $9
watch $10
```
- Watch specific registers using `watch $N` where N is the register number
- Watch memory locations using `watch 0xADDRESS`
- Examples:
  - `watch $8` - Watch register $t0
  - `watch $9` - Watch register $t1
  - `watch 0x1000` - Watch memory at address 0x1000

### STEP 3: View initial state (optional)
```
registers
pc
memory 0x0 0x100
```
- `registers` - View all registers
- `reg 8` - View specific register ($8)
- `pc` - View program counter
- `memory 0x0 0x100` - View memory range

### STEP 4: Execute your program
```
step
```
Execution options:
- `step` - Execute one instruction
- `step 5` - Execute 5 instructions
- `stage` - Execute one pipeline stage
- `run` - Run until halt

### STEP 5: Inspect state after execution
```
registers
pc
```
View the CPU state after execution. Watch expressions will be displayed automatically after `step`, `stage`, or `run` commands.

### STEP 6: Continue execution (optional)
```
step 3
run
```
Continue with more steps or run to completion.

### STEP 7: Save state (optional)
```
save my_state.bin
```
Save the current CPU state to a file for later restoration.

### STEP 8: Exit
```
quit
```
Exit the simulator.

## Complete Example

Here's a complete working template without comments:

```
load your_program.hex
watch $8
watch $9
registers
step
registers
step
registers
run
registers
quit
```

## Creating Your Own Scripts

1. Copy [TEMPLATE.txt](TEMPLATE.txt) to a new file
2. Replace `your_program.hex` with your program path
3. Add/remove watch expressions as needed
4. Add commands for execution and inspection
5. **DO NOT** add comment lines (lines starting with `#`)
6. Run with: `./build/bin/ez_architecture_cli < your_script.txt`

## Available Commands

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
