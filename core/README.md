# Core

## Overview

Here is the core of the project. It contains the Virtual Machine implementation and instructions.

Instructions are baked into the VM, and should not change at all.

Folder iface contains the interfaces for the external parts, that VM needs to interact with:
- Video
- Keyboard
- Sound
- Timer
- Random

Each of these parts might be implemented independently, and the VM should be able to work with any of them.
Check out the `../impl_basic` folder for naiive implementation of peripherals.

## Quirks

I tried to make the VM as flexible as possible, and there is a way to control all the quirks of the original CHIP-8 implementation.
It is done by setting the `settings.emulator_type` field of the VM's settings struct.

Although, some of them are not implemented yet, and some of them are not tested. I have not even done with the original CHIP-8 quirks yet.
