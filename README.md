# IGVC-Olympus Firmware &#9925;

The autonomous golf cart is a project at the Cal Poly Robotics Club focused on producing a golf cart that can drive around campus without human intervention. Olympus is the name of the electronic control board for the the golf cart, and this reposistory contains firmware for the microcontrollers on Olympus. To learn more about our project, click [here](https://www.calpolyrobotics.com/igvc/).

## Project Structure
This repository contains multiple directories, each pertaining to the different applications run on the microcontrollers living on Olympus:
- **apollo** - headlights, turn signals, and miscellaneous lighting on the golf cart
- **hephaestus** - motor controllers for steering and braking
- **hera** - hall-effect sensors, sonars, and steering potentiometer
- **janus** - forward, neutral, and reverse control
- **olympus** - handles communications between all devices and reports data back over USB
- **master_boot** - bootloader for main MCU (Olympus)
- **slave_boot** - bootloader for slave MCUs (Apollo, Hephaestus, Hera, Janus)
- **master_lib** - shared files between master applications
- **slave_lib** - shared files between slave applications

## Programming
### Bootloader
After connecting your computer to the USB port on Olympus, and changing into the root directory of the reposistory:
**To Program All MCUs:**
```sh
$ make flash
```

**To Program Specific MCU:**
```sh
$ make flash-<mcuFolderName>
```

### JLink
After connecting your computer to the Segger JLink, connecting the JLink to the MCU you wish to program, and changing into the root directory of the repository:
**To Program Specific MCU:**
```sh
$ cd <mcuFolderName>
$ make flash-jlink
```

## Debugging
To debug one of the microcontroller on Olympus, you must use a Segger Jlink.
1. Connect the Jlink to the MCU you wish to debug
2. Open a terminal and run the following (depends on the MCU you are debugging):
    **Olympus**
    ```sh
    JLinkGDBServer -Device stm32f205RG -if swd
    ```
    
    **Apollo, Hera, or Janus**
    ```sh
    JLinkGDBServer -Device stm32f042K6 -if swd
    ```
    
    **Hephaestus**
    ```sh
    JLinkGDBServer -Device stm32f042C6 -if swd
    ```
    
3. Open another terminal and run (from the directory of the mcu you are debugging):
    ```sh
    arm-none-eabi-gdb build/program.elf
    ```
4. You will now be able to use standard gdb commands to debug the microcontroller
