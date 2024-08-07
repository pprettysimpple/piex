# Piex

This is an emulator of famous chip8 system. It is written in C++ as a pet-project.
Core of emulator is bundled with opt-in SDL2 implementation or ascii.
I want to spport all the quirks and all different versions of chip8. That is the goal, sort of.

## Table of Contents

- [Implementation](#implementation)
- [Build](#build)
- [Usage](#usage)
- [TODO](#todo)

## Implementation

- [Core](core/README.md)
- [Core/iface](core/iface/README.md)

## Build

You will need to install SDL2 library to build this project. On Ubuntu you can do it with following command:

```bash
sudo apt-get install libsdl2-dev
```

Then you can build project with cmake:

```bash
mkdir build
cd build
cmake .. -DENABLE_SDL=1 # or -DENABLE_SDL=0 for tests-only build
make
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
