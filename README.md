# fpvLiberator
Get Live Video from DJI HD FPV Goggles. C++ version!

## Compiling from source

Builds using [Cmake](https://cmake.org/download/) for easy compilation on any system. Requires libusb.

## Usage

```
./fpvLiberator | ffplay -i - -analyzeduration 1 -probesize 32 -sync ext
```
