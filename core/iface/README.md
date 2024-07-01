# Peripherals

This folder contains the interfaces for the external parts, that VM needs to interact with:

## Video
Interface is really simple. You have to implement just one method, that will be called by VM to draw the screen.
It accepts the buffer of video memory. You can dump it to the console, or render it to the window, or do whatever you want.

There is an ascii implementation in `impl_basic` root folder.

## Keyboard
Keyboard interface is tricky.
TODO: Describe it.

There is a no-op implementation in `impl_basic` root folder.

## Sound
Sound interface as simple as it is in CHIP-8. Just one method to beep for a while.

There is a no-op implementation in `impl_basic` root folder.

## Timer
Timer interface allows you to speed up or slow down the VM, by changing the real time it sleeps for.

You have no need to write your own implementation, as there is two of them (no-op and real sleep) in `impl_basic` root folder.

## Random
Random interface is used to generate random numbers for the VM. You can plug-in some real random generator, or use something deterministic for testing and your own sanity!

There is a simple implementation, based on `rand()` in `impl_basic` root folder.

## Implementation hints
You can find some simple implementations in `impl_basic` root folder. Also, there is a `impl_sdl` folder, that contains SDL2-based implementations of all the peripherals.
