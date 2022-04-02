# Custom operating system for Armv8-A

![armOS-logo](docs/rasberrypi.jpg)

![GitHub top language](https://img.shields.io/github/languages/top/GeorgePag4028/Custom-Arm-OS?color=blueviolet) ![GitHub repo size](https://img.shields.io/github/repo-size/thanoskoutr/armOS?color=informational&logo=GitHub) ![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/GeorgePag4028/Custom-Arm-OS?color=blue&logo=GitHub) ![GitHub](https://img.shields.io/github/license/GeorgePag4028/Custom-Arm-OS?color=brightgreen&logo=Open%20Source%20Initiative) 

![Board](https://img.shields.io/badge/board-Raspberry%20Pi%20Zero%20W-red?style=flat&logo=Raspberry%20Pi) ![Board](https://img.shields.io/badge/board-Raspberry%20Pi%204-red?style=flat&logo=Raspberry%20Pi&logoColor=white) ![chip](https://img.shields.io/badge/chip-BCM2835-orange?style=flat&logo=Broadcom) ![chip](https://img.shields.io/badge/chip-BCM2711-orange?style=flat&logo=Broadcom) ![arch](https://img.shields.io/badge/arch-armv6-lightblue?style=flat) ![arch](https://img.shields.io/badge/arch-armv8--a-lightblue?style=flat)


Simple ARM Operating System for the Raspberry Pi, supporting Aarch64 architecture. The board supported is Raspberry Pi 3 A+.

This project was made for the Embedded Systems course at NTUA 2021-2022.

## Technology used 
- Board : Rasberry Pi 3 A+
    - Peripherals module : [BCM2837-Broadcom](https://github.com/raspberrypi/documentation/files/1888662/BCM2837-ARM-Peripherals.-.Revised.-.V2-1.pdf)
    - Architecture : Armv8-A
    - Cpu : [Contex-A53](https://developer.arm.com/Processors/Cortex-A53#Technical-Specifications)
- Arduino Nano 
- Cables 
    - TTL to Usb
    - Usb power cables

## Functionalities 

### Peripherals 

#### Uart 1

We are using "miniUART" or "UART1" , because UART0(PL011) is connected to bluetooth and we want to keep this functionality open for development. 

- First we have to set GPIO pins 14,15 to their function TXD1 and RXD1 respectively.

- Then we disable flow control and interrupts just to make it easier to test. Also, baud rate is set to 115200 and the 8 bit ascii mode (extended) is selected . The above is all done via AUX registers for UART1 for which we have calculated their addresses according to the datasheet again.

- Note: according to [this](https://forums.raspberrypi.com/viewtopic.php?f=63&t=138162) , to avoid clock skewing error in uart we set the clock frequency to 250 MHz in the config file of the boot partition in the SD card of the Pi 3. 

- For testing, “screen” program was used along with a TTL cable. Terminal Command : sudo screen /dev/ttyUSB0 115200 . Baud Rate and setup is explained in code.

#### I2C 

Use of Broadcom Serial Controller (BSC).

We are gonna use BSC1.


Tip on debugging. Check error. We were getting the “ERR” return value from the status register. This has to do with no ACK of slave address. One common cause for this issue is incorrect voltage in GPIO pins.So, we took our old trusty multimeter and saw that the voltage on the pins was indeed incorrect (lower than voltage “HIGH”). Thus, it was mandatory to  recheck hardware gpio settings in the kernel. It turned out that we had set some bits wrong. :D


If you are getting CLKT timeout then it probably means that the slave is reading/writing slower than expected. This can be fixed by either increasing baudrate of slave in its serial IOs (that’s what we did), or if this is not possible increasing timeout window from master (raspberry). 



### Interrupts 
We are going to use 2 from 4 different types of interrupts :
-  Synchronous  Interrupts : Software interrupts called with the instruction `svc`.
-  IRQ : External , low priority , interrupts.

We used interrupts for 2 main tasks : 
- Timer: IRQ in specific time interval 
- System Call : Svc in EL0 state

### System calls 
We implement 7 system calls that helped us in the os : 
- Write : Takes a argument and prints it in the screen.
- Malloc : Allocate a new memory page(4Kb) for a user process.
- Clone : Creates new process. Takes the argument of a process code and it's arguments.
- Exit : Free up the memory page of a process and makes it ZOMBIE.
- Cat : Prints cats in the screen.
- Change_prior: Set the priority of a process.
- Get_Prior: Get the priority of a process.

### Tasks 
```
├── task_struct
│   ├── cpu_context: all the registers of this process
│   ├── state: RUNNING|ZOMBIE
│   └── Counter: How many times this process has not been scheduled
│   └── Preempt_Count: Check if this process is prerforming the scheduler
│   └── Priority: High|Middle|Low
```
#### Priority 

| Priority          | Scheduling Policy                             |
| ----------------  | -------------------------------------------   |
| High              | Non-Preemptive                                |
| Middle            | Preemptive                                    |
| Low               | Preemptive                                    |

**Non-Preemptive**: A process can't be scheduled out in the middle of its execution.
**Preemptive**: A process can be scheduled out in the middle of its execution. 

#### Task Code
We have created a basic task so we can understand the demo. 
```
int task(array,priority){
sys_change_priority(priority);
count = 0;
while(count<5){
    for (i=0;i<len(array);i++){
        sys_call_write(sys_call_get_priority());
        sys_call_write(array[i]);
        delay(_);
        }
    count++;
    }
sys_exit_process();
}
```
## Documentation
There is proper documentation available for the project in two different formats:

- In the [GitHub Pages](https://thanoskoutr.github.io/armOS/) of the project's GitHub repository.
- In the [Wiki section](https://github.com/thanoskoutr/armOS/wiki) of the project's GitHub repository.

### Doxygen Documentation
There is detailed documentation of all the source code, generated by [Doxygen](https://www.doxygen.nl), describing all the structures and functions of the project.

**NOTE:** The documentation is generated either for the Rasperry Pi 4 or the Raspberry Pi Zero / Zero W. They have some differences mainly regarding the register addresses, but they are mostly identical.

### Wiki Documentation
There are specific details about the implementation of most major parts of the OS, and guides for installing the dependencies and running the OS on a Pi.

## Project Structure
The project structure for the source code is the following:
```
├── arch
│   ├── armv6
│   ├── armv7-a
│   └── armv8-a
├── armstub
│   └── src
├── include
│   ├── armv6
│   ├── armv8-a
│   ├── common
│   ├── kernel
│   └── peripherals
└── src
    ├── common
    └── kernel
```
The `include/` directory contains definitions of structures and functions:

- `kernel/`: For the core features of the kernel.
- `armv6/` and `armv8-a/`: For architecture specific routines implemented in ARM assembly code.
- `common/`: For common libraries such as `string.h` and `stdlib.h` needed for the kernel.
- `peripherals/`: For the register's addresses for the peripherals of the devices, in order to provide basic drivers for them.

The `src/` directory contains the implementations of the functions defined in the `include/` directory.

The `arch/` directory contains the implementations of the architecture specific routines implemented in ARM assembly code, separated based on the ARM architecture.

The `armstub/` directory contains a simple bootloader for the ARMv-8 architecture, in order to boot the kernel in the proper [Exception Level](https://developer.arm.com/documentation/102412/0100/Privilege-and-Exception-levels).

## Features
The current features of the kernel are the following:

- Drivers for basic peripherals of the Pi boards:
    - [x] mini UART
    - [x] System Timers
    - [x] GPIO Pins
    - [x] LED
- Implementation of basic library functions:
    - [x] `string.h`
    - [x] `stdlib.h`
    - [x] `stdbool.h`
    - [x] `printk()`
- Exception Level handling for the ARMv-8 architecture:
    - [x] Setting kernel at EL 1.
- Handling Interrupts - Exceptions:
    - [x] Initialize vector table.
    - [x] Handles Timer and UART IRQs.
- Scheduler and processes:
    - [x] Creates and kills processes.
    - [x] Schedules them in order to run multiple processes concurrently.
    - [x] Performs Context Switching for the execution of different processes.
- Implementation of basic serial console:
    - [x] Communication with users through serial input and output.
    - [x] Available commands to execute.

Features to be implemented in the near feature:

- System Calls
- Proper Virtual memory management
- Video drivers

### Demo of kernel
A screenshot of the kernel in action, after booting up:

```
                  _____ _____
 ___ ___ _____   |     |   __|
| . |  _|     |  |  |  |__   |
|__,|_| |_|_|_|  |_____|_____|

armOS initializing...

        Board: Raspberry Pi 4
        Arch: aarch64


----- Exception level: EL1 -----
Initializing IRQs...Done
Enabling IRQ controllers...Done
Enabling IRQs...Done
Initializing LED...Done

This is a minimal console, type 'help' to see the available commands. (Maximum Input Length: 80)
root@pi-4#
```

A screencast where, we create 3 processes, run them and kill them:
![asciicast-gif](docs/images/armOS-procs.gif)

### Available Commands
Showing the available commands of the console, using the `help` command:
```
root@pi-4# help
Available commands:
    help:
        Prints available commands to the console.
    help_led:
        Prints available LED commands to the console.
    create_procs:
        Creates proc_num kernel processes.
    run_procs:
        Runs the created kernel processes concurrently.
    kill_procs:
        Kills all created kernel processes.
    halt:
        Halts the system.
```


## Requirements
In order to run this kernel in a Raspberry Pi you need the following:

### Boards
A Rasberry Pi board. One of the following:

- Raspberry Pi 4
- Raspberry Pi Zero or Zero W

### Tools
- A USB to TTL serial cable.
- An a SD Card.

### Software
- The [Pi Imager](https://www.raspberrypi.org/software/) program.
- A Linux/Unix development environment to build the project.
- A Serial Console emulator (suggested: `screen`).

### Toolchain
An ARM Cross-Compiler Toolchain:
- For building the 32-bit kernel: `arm-none-eabi-gcc`: [Download from ARM website - For Linux x86_64 Hosts](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- For building the 64-bit kernel: `aarch64-none-elf-gcc` [Download from ARM website - For Linux x86_64 Hosts](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads)


## Instructions

### Install Toolchain
For detailed instructions, see the [Install Toolchain](https://github.com/thanoskoutr/armOS/wiki/Install-Toolchain) wiki page.

### Serial Connection with Raspberry PI
For detailed instructions, see the [Serial Connection](https://github.com/thanoskoutr/armOS/wiki/Serial-Connection) wiki page.


### Running on Raspberry Pi
For detailed instructions, see the [Installation](https://github.com/thanoskoutr/armOS/wiki/Installation) wiki page.

Basic overview of the steps:

- Format the SD card, with the Raspberry Pi Imager.
- Set cross-compiler path on the Makefile.
- Clone the project.
- Build project using `Makefile`.
- Transfer `.img`, `config.txt` to SD Card.
- Unmount SD card.
- Put SD card on Pi.
- Connect USB-to-TTL serial cable to the Pi and Linux host.
- Open `screen`.
- Power on the Pi.

### Generating Documentation
In order to generate the Doxygen documentation locally, see the [Generate Docs](https://github.com/thanoskoutr/armOS/wiki/Generate-Docs) wiki page.

### Implementation Details
For information about the source code, see on [Wiki](https://github.com/thanoskoutr/armOS/wiki), the *Implementation* section.

For detailed documentation of all the source code, generated by Doxygen, describing all the structures and functions of the project, see the [GitHub Pages](https://thanoskoutr.github.io/armOS/) hosted site.

## Technologies Used
A complete list of all the technologies used in this project:

| Technology       | Selection                                   |
| ---------------- | ------------------------------------------- |
| Editor           | `Visual Studio Code`                        |
| Host OS          | `Ubuntu 20.04`                              |
| Cross-Compiler   | `arm-none-eabi-gcc`, `aarch64-none-elf-gcc` |
| Binutils         | `objdump`, `objcopy`, `size`, `nm`          |
| Building         | `Make`                                      |
| Scripts          | `Bash`                                      |
| Documentation    | `Doxygen`                                   |
| CI               | `Travis CI`                                 |
| Static Hosting   | `GitHub Pages`                              |
| Version Control  | `Git`, `GitHub`                             |
| Console Emulator | `screen`                                    |
| Boards           | Raspberry Pi 4, Raspberry Pi Zero W         |
| Hardware         | SD card, USB-to-TTL Serial cable            |

## Resources

### Manuals

- [BROADCOM BCM2835](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/README.md)
- [BROADCOM BCM2711](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2711/README.md)
- [AArch64 memory management](https://developer.arm.com/documentation/101811/latest)

### Wikis

- [OSDev.org - Raspberry Pi Bare Bones](https://wiki.osdev.org/ARM_RaspberryPi_Tutorial_C)
- [ARM Developer](https://developer.arm.com/)

### Repositories

- [Linux Kernel - /arch/arm/](https://github.com/torvalds/linux/tree/master/arch/arm)
- [Learning operating system development using Linux kernel and Raspberry Pi](https://github.com/s-matyukevich/raspberry-pi-os)
- [Building an Operating System for the Raspberry Pi](https://jsandler18.github.io/)
- [Raspberry Pi ARM based bare metal examples](https://github.com/dwelch67/raspberrypi)
- [Bare metal Raspberry Pi 3 tutorials](https://github.com/bztsrc/raspi3-tutorial)
- [Writing a "bare metal" operating system for Raspberry Pi 4](https://github.com/isometimes/rpi4-osdev)

### Book

- [The little book about OS development](http://littleosbook.github.io/)

## License
This project is licensed under the MIT license. See [LICENSE](LICENSE) for details.