# EZ_Architecture

A MIPS computer architecture learning tool that helps visualize how MIPS instructions work internally, step by step.

## Features

- **GUI Mode**: Interactive visualization using SFML showing CPU pipeline stages, registers, and memory
- **CLI Mode**: Text-based descriptions and step-by-step execution
- **Pipeline Visualization**: See each stage (Fetch, Decode, Execute, Memory Access, Write Back)
- **Educational Focus**: Designed for learning computer architecture concepts

## Dependencies

### Required

- **CMake** (>= 4.1.2)
- **C++17** compatible compiler (GCC, Clang, MSVC)

### Optional

- **SFML** - for GUI mode (CLI works without it)
- **linenoise** - included, provides command history and line editing in CLI

## Building

### Quick Build

```bash
./build.sh          # Debug build with all features
./build.sh Release  # Release build
```

### Build Options

You can customize the build with CMake options:

```bash
mkdir build && cd build

# Build with all features (default)
cmake ..

# Build without linenoise (basic input only)
cmake -DUSE_LINENOISE=OFF ..

make -j$(nproc)
```

**What's linenoise?** A lightweight library that adds command history (up/down arrows) and line editing to the CLI. It's included in the project and enabled by default.

## Running

### GUI Mode

```bash
./build/bin/ez_architecture_gui
```

### CLI Mode

```bash
./build/bin/ez_architecture_cli
```

### Example Programs

The `examples/` directory contains ready-to-use test programs:

```bash
# Run a test program
./build/bin/ez_architecture_cli < examples/test_addition.txt

# Run all examples
./examples/run_all_tests.sh
```

See [examples/README.md](examples/README.md) for all available examples and [examples/QUICK_REFERENCE.md](examples/QUICK_REFERENCE.md) for CLI commands.

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
- [x] Add instruction decoder
- [x] Create CLI interface
- [ ] Build GUI visualizer
- [ ] Add tests
- [x] Implement all basic MIPS instructions
- [ ] Add pipeline hazard detection
- [x] Create example programs for CLI
- [ ] Make GUI adapt to instructions

## License

Educational project for learning purposes.
