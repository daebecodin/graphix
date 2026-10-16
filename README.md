# Graphix

A C++20 graphics project using SDL3's GPU API and HLSL shaders.

## Project setup (macOS)

Start with Git, [Homebrew](https://brew.sh/), and the Xcode Command Line Tools.
Install the Command Line Tools if you do not already have them:

```bash
xcode-select --install
```

Install the build tools:

```bash
brew install cmake ninja conan
```

Clone the project and enter its root directory:

```bash
git clone https://github.com/daebecodin/graphix.git
cd graphix
```

The commands below assume you are in the directory containing `conanfile.py`
and `CMakeLists.txt`. After a fresh clone, this is `graphix/`. In the current
local workspace, these files are under `graphix/sdl/`, so run the setup commands
from that `sdl/` directory. Build files and downloaded shadercross sources stay
under its ignored `build/` directory.

### Install project dependencies

This project uses Conan 2 to provide SDL3. On your first Conan setup, create a
default compiler profile:

```bash
conan profile detect
```

If you already have a default profile, keep it. The starter recipe also requires
`engine3d-cmake-utils/4.0` from the following remote. Add it once:

```bash
conan remote add engine3d-conan https://libhal.jfrog.io/artifactory/api/conan/engine3d-conan
```

Install dependencies and generate the CMake configuration:

```bash
conan install . --build=missing -s build_type=Debug -s compiler.cppstd=20
```

Configure CMake using the generated preset:

```bash
cmake --preset conan-debug
```

Build the application with:

```bash
cmake --build --preset conan-debug
```

The executable is `build/Debug/main`. Compile and copy the shaders as described
below before running the triangle demo.

### Rebuild and run during development

Run these commands from the project root after changing C++ files:

```bash
cmake --build build/Debug
./build/Debug/main
```

After adding or changing HLSL shaders, compile them before rebuilding and running:

```bash
export PATH="$PWD/build/SDL_shadercross-build:$PATH"
mkdir -p shaders/compiled/SPIRV shaders/compiled/MSL shaders/compiled/DXIL
(cd shaders/source && bash compiles.sh)
cmake --build build/Debug
./build/Debug/main
```

Build shadercross once using the instructions below before using this block.
Shader compilation is only needed after adding or changing HLSL files, or when
the compiled outputs are missing. Repeat the `export` in each new terminal
session. The directory creation commands are safe to repeat.

### Build the shadercross CLI

[SDL_shadercross](https://github.com/libsdl-org/SDL_shadercross) translates HLSL
to SPIR-V, MSL, and DXIL. The following commands build the CLI locally using
its vendored dependencies, including DirectXShaderCompiler for HLSL support.
These instructions are based on upstream's
[CMake configuration](https://github.com/libsdl-org/SDL_shadercross/blob/main/CMakeLists.txt)
and [macOS build workflow](https://github.com/libsdl-org/SDL_shadercross/blob/main/.github/workflows/main.yml).

Install SDL3 for this separate tool build. Graphix itself still uses
Conan's SDL3 dependency.

```bash
brew install sdl3

git clone --recursive https://github.com/libsdl-org/SDL_shadercross.git build/SDL_shadercross

cmake -S build/SDL_shadercross -B build/SDL_shadercross-build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$(brew --prefix)" \
  -DSDLSHADERCROSS_VENDORED=ON \
  -DSDLSHADERCROSS_CLI=ON

cmake --build build/SDL_shadercross-build --target shadercross --parallel

./build/SDL_shadercross-build/shadercross --help
```

The first build can take a while because it also builds the shader compiler
dependencies. You only need to build the CLI once, unless you update it or
delete its build directory.

### Compile your shaders

From the project root, add the local CLI to the current terminal's `PATH`:

```bash
export PATH="$PWD/build/SDL_shadercross-build:$PATH"
```

This uses your own checkout location. Repeat it in each new terminal session
before running the shader script, or use an existing `shadercross` installation
that is already on your `PATH`.

Put HLSL files in `shaders/source/` with these suffixes:

- `name.vert.hlsl` for vertex shaders.
- `name.frag.hlsl` for fragment shaders.
- `name.comp.hlsl` for compute shaders.

After creating or editing a shader, run these commands from the project root:

```bash
mkdir -p shaders/compiled/SPIRV shaders/compiled/MSL shaders/compiled/DXIL
(cd shaders/source && bash compiles.sh)
```

The script writes each shader to all three output directories. Use the exact
spellings `SPIRV`, `MSL`, and `DXIL`; the directories are not tracked when empty,
so the `mkdir` command is needed for a fresh checkout. With no matching HLSL
files, the script produces no output. Shader compilation is a separate step
from the C++ build. Every CMake build copies the compiled shaders to
`build/Debug/shaders/compiled/`, where the loader expects them. Rebuild after
compiling shaders to refresh that runtime copy.

To compile just one shader, use the CLI directly from the project root:

```bash
./build/SDL_shadercross-build/shadercross shaders/source/example.vert.hlsl \
  -o shaders/compiled/MSL/example.vert.msl
```

Replace `example.vert.hlsl` with your shader's filename and create the output
directory first. Change the output extension to `.spv` or `.dxil` to select
SPIR-V or DXIL.

## TODO

- [ ] Add a color code converter for hex and RGB colors to normalized RGBA values for `SDL_FColor`.

## Other platforms

The setup above covers macOS. Linux and Windows setup instructions still need
to be verified. A Windows shader script is provided at
`shaders/source/compile.bat`; it requires `shadercross` on `PATH` and the same
output directories.
