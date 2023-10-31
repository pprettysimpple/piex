# Piex

This is an emulator of famous chip8 system. It is written in C++ and can be easily ported to any platform.
Core of emulator is bundled with several different system implementations.
Currently only SDL2 or ascii-art are available.

## Table of Contents

- [Build](#build)
- [Usage](#usage)
- [TODO](#todo)

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
ninja all
```

And to clean build directory after some builds:

```bash
ninja clean
```

## Usage

```bash
./build/piexapp <sdl|ascii> <ch8|sch|xoch> <path_to_rom>
```

First argument is platform implementation:
- sdl - use sdl2 implementation
- ascii - use ascii-art implementation, no keyboard support

Second argument is emulation-type:
- ch8 - chip8 type
- sch - superchip type
- xoch - xochip type

Third argument is path to rom file.

## TODO

- Add sound implementation in sdl build, currently it is stub with no sound
- Incapsulate quirks logic into core (move video memory to core, change interface to draw only per_pixel)
- Better deal with timers, only "display.wait" left for 100% quirks tests
- Implement the rest of opcodes
