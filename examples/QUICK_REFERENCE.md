# EZ Architecture CLI Quick Reference

## Running Test Programs

```bash
# Run a test program
./build/bin/ez_architecture_cli < examples/test_addition.txt

# Run and continue interactively
cat examples/test_addition.txt - | ./build/bin/ez_architecture_cli

# Run all tests
./examples/run_all_tests.sh
```

## CLI Commands

### Program Loading
| Command | Description | Example |
|---------|-------------|---------|
| `load <file>` | Load hex program | `load test_program.hex` |
| `loadstate <file>` | Load saved state | `loadstate state.bin` |
| `reset` | Reset CPU | `reset` |

### Execution
| Command | Description | Example |
|---------|-------------|---------|
| `step` | Execute 1 instruction | `step` |
| `step <n>` | Execute n instructions | `step 5` |
| `stage` | Execute 1 pipeline stage | `stage` |
| `run` | Run until halt | `run` |

### Inspection
| Command | Description | Example |
|---------|-------------|---------|
| `registers` | Show all registers | `registers` |
| `reg <n>` | Show register n | `reg 8` |
| `memory <start> [end]` | Show memory | `memory 0x0 0x100` |
| `pc` | Show program counter | `pc` |
| `watch <expr>` | Add watch | `watch $8` or `watch 0x1000` |

### Assembly/Disassembly
| Command | Description | Example |
|---------|-------------|---------|
| `disasm [addr]` | Disassemble | `disasm 0x0` |
| `asm <instr>` | Assemble | `asm add $t0, $t1, $t2` |

### State Management
| Command | Description | Example |
|---------|-------------|---------|
| `save <file>` | Save CPU state | `save checkpoint.bin` |

### Other
| Command | Description | Example |
|---------|-------------|---------|
| `help` | Show help | `help` |
| `quit` | Exit | `quit` |

## Register Names (MIPS)

| Number | Name | Purpose |
|--------|------|---------|
| $0 | $zero | Always 0 |
| $8-$15 | $t0-$t7 | Temporary |
| $16-$23 | $s0-$s7 | Saved |
| $29 | $sp | Stack pointer |
| $31 | $ra | Return address |

## Watch Expression Format

```bash
# Watch a register
watch $8          # Watch $t0
watch $9          # Watch $t1

# Watch a memory location
watch 0x1000      # Watch memory at 0x1000
watch 0x2000      # Watch memory at 0x2000
```

## Creating .hex Programs

Each line is one 32-bit instruction in hexadecimal:

```
# Comments start with #
20080005  # addi $t0, $zero, 5
2009000A  # addi $t1, $zero, 10
01095020  # add $t2, $t0, $t1
00000000  # halt
```

## MIPS Instruction Reference

### R-Type (Register)
```
add  $d, $s, $t    # $d = $s + $t
sub  $d, $s, $t    # $d = $s - $t
and  $d, $s, $t    # $d = $s & $t
or   $d, $s, $t    # $d = $s | $t
slt  $d, $s, $t    # $d = ($s < $t) ? 1 : 0
```

### I-Type (Immediate)
```
addi $t, $s, imm   # $t = $s + imm
andi $t, $s, imm   # $t = $s & imm
ori  $t, $s, imm   # $t = $s | imm
lw   $t, off($s)   # $t = mem[$s + off]
sw   $t, off($s)   # mem[$s + off] = $t
beq  $s, $t, off   # if ($s == $t) PC += off
bne  $s, $t, off   # if ($s != $t) PC += off
```

### J-Type (Jump)
```
j   addr           # PC = addr
jal addr           # $ra = PC+4; PC = addr
```

## Pipeline Stages

1. **Fetch (IF)** - Fetch instruction from memory
2. **Decode (ID)** - Decode instruction and read registers
3. **Execute (EX)** - Perform ALU operation
4. **Memory (MEM)** - Access memory if needed
5. **Write Back (WB)** - Write result to register

Use `stage` command to step through each stage individually.

## Tips

- Use Ctrl+D (EOF) to exit CLI
- Command history available with up/down arrows (if linenoise enabled)
- Comments in .hex files start with `#`
- Watches automatically display after `step`, `stage`, and `run`
- Use `cat test.txt - | cli` to run script then continue interactively
