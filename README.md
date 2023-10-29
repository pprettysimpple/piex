# Piex

This is an emulator of famous chip8 system. It is written in C++ and can be easily ported to any platform.
Core of emulator is bundled with several different system implementations.
Currently only SDL2 or ascii-art are available.

## Table of Contents

- [Build](#build)
- [Usage](#usage)

## Build

You will need to install SDL2 library to build this project. On Ubuntu you can do it with following command:

```bash
sudo apt-get install libsdl2-dev
```

Then you can build project with cmake:

```bash
mkdir build
cd build
cmake ..
ninja piexapp
```

And to clean build directory after some builds:

```bash
ninja clean
```

## Usage

```bash
./piexapp <path_to_rom>
```

This will run emulator with SDL2 or ascii-art system implementation. Depending on stupid `#if` in `demo/main.cpp` code.

SDL2 implementation is very buggy and not finished. It is not recommended to use it.
Actually, it is not recommended to use this emulator at all, cause it is not finished yet. `:)`
