# Introduction

GifII is an application that generates a ProDOS animation `.po` disk from a provided GIF. The generated disks can be run on real ProDOS-compatible Apple II machines as well as Apple II emulators.

For development background and example results, please visit [my blog post on the development of GifII.]()

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

The disks can also be run on real ProDOS-compatible Apple II hardware. You can see the [ProDOS hardware requirements here.](https://en.wikipedia.org/wiki/Apple_ProDOS#Requirements) Disks have been tested by me on an Apple IIgs running in compatibility mode, but I expect it should work on anything ProDOS-compatible.

To transfer your disk image to a real disk, you can use a tool such as [c2t](https://github.com/datajerk/c2t) to convert the disk into a sound file that can be sent over the cassette port and flashed to a real disk.

# Building from source

GifII uses CMake for building, and has been successfully compiled with MSVC via GitHub Actions and GCC via VSCode's CMake extension. CMake will automatically copy the `GIFII_TEMPLATE_DISK.po` disk from source into the build folder.

```
cmake -S . -B build
cmake --build build  
```

# Notes
- ProDOS only allows 50 files to be placed at the root of the disk image. 3 of these will be the necessary code to display the images, so a maximum of 47 frames of animation will be generated on the disk. GIFs with more than 47 frames will have some intermediate frames removed in the animation.
- GIFs with no animation / only one frame are also supported for generating still screensavers.
- No other file types or image formats are supported currently. 