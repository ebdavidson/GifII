# Introduction

GifII is an application that generates a ProDOS animation `.po` disk from a provided GIF. The generated disks can be run on Apple II emulators, and should theoretically run on ProDOS-compatible Apple II hardware.

For development background and example results, please visit [my blog post on the development of GifII.](https://ebdavidson.com/blog/2026-11-16/)

# Usage

Currently, GifII is only built for Windows. Download the latest release from the Releases page and extract it. The release contains the GifII binary as well as the template disk used by the program to generate the animation disks.

You can create an animation by dragging a .GIF file onto the EXE in File Explorer, or via the command line with:
```
GifII.exe [GIF PATH]
```

If the application correctly builds a disk, it will place it in the same folder as the .EXE and print the disk name. Otherwise, it will print an error message.

GifII expects the `GIFII_TEMPLATE_DISK.po` disk to be placed in the same directory as the .EXE, as it is when downloaded from the release. If this disk is missing, the application will not function. 

# Booting the disks

The easiest way to load your generated disk is in an Apple II emulator, such as [AppleIIjs](https://www.scullinsteel.com/apple2/) or [AppleWin](https://github.com/AppleWin/AppleWin). Load the disk in Slot I and reboot the system.

The disks should hopefully work on real ProDOS-compatible Apple II hardware, though this hasn't been tested yet. You can see the [ProDOS hardware requirements here.](https://en.wikipedia.org/wiki/Apple_ProDOS#Requirements) If you run a disk on real hardware, please reach out to me via the email on [my website.](https://ebdavidson.com)

To transfer your disk image to a real disk, you can use a tool such as [c2t](https://github.com/datajerk/c2t) to convert the disk into a sound file that can be sent over the cassette port.

# Building from source

GifII uses CMake for building, and has been successfully compiled with MSVC via GitHub Actions and GCC via VSCode's CMake extension. The code requires building with C++17 or above. CMake will automatically copy the `GIFII_TEMPLATE_DISK.po` disk from source into the build folder. 

```
cmake -S . -B build
cmake --build build  
```
The `startup.bas` in the repo is included as source for the BASIC program that runs on the generated disks, but the disk is already prebuilt with this BASIC code, so any changes to the source version will not carry over to the template disk. You will need to manually load your updated BASIC code to the template disk using a tool such as [CiderPress 2](https://ciderpress2.com/).

# Notes
- ProDOS only allows 50 files to be placed at the root of the disk image. 3 of these will be the necessary code to display the images, so a maximum of 47 frames of animation will be generated on the disk. GIFs with more than 47 frames will have some intermediate frames removed in the animation.
- GIFs with no animation / only one frame are also supported for generating still screensavers.
- No other file types or image formats are supported currently. 
