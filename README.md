# EZ_Architecture

A MIPS computer architecture learning tool that helps visualize how MIPS instructions work internally, step by step.

## Features

- **GUI Mode**: Interactive visualization using SFML showing CPU pipeline stages, registers, and memory
- **CLI Mode**: Text-based descriptions and step-by-step execution
- **Pipeline Visualization**: See each stage (Fetch, Decode, Execute, Memory Access, Write Back)
- **Educational Focus**: Designed for learning computer architecture concepts

## Dependencies

- **CMake** (>= 3.15)
- **C++17** compatible compiler (GCC, Clang)
- **SFML** (>= 2.5) for GUI

## Building

### Quick Build

```bash
./build.sh          # Debug build
./build.sh Release  # Release build
```

### Manual Build

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Running

### GUI Mode

```bash
./build/bin/ez_architecture_gui
```

### CLI Mode

```bash
./build/bin/ez_architecture_cli
```

## Testing

```bash
cd build
ctest --output-on-failure
```

Or run individual tests:

```bash
./build/bin/test_alu
./build/bin/test_cpu
```

## Supported MIPS Instructions

### R-Type Instructions
- `add`, `sub`, `and`, `or`, `slt`

### I-Type Instructions
- `addi`, `andi`, `ori`
- `lw`, `sw` (load/store word)
- `beq`, `bne` (branch equal/not equal)

### J-Type Instructions
- `j`, `jal` (jump/jump and link)

## Development Roadmap

- [x] Implement core CPU components
- [ ] Add instruction decoder
- [ ] Create CLI interface
- [ ] Build GUI visualizer
- [ ] Add comprehensive tests
- [x] Implement all basic MIPS instructions
- [ ] Add pipeline hazard detection
- [ ] Create example programs

## License

Educational project for learning purposes.
