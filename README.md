# Phasor Space Modules

By Roger Jungemann (a.k.a. [Phasor Space](https://phasor.space))

## Introduction

This repo contains a series of modules for VCV Rack, for building rhythmic synth patches.

## Setup

1. Follow these [prerequisite instructions](https://vcvrack.com/manual/PluginDevelopmentTutorial#Prerequisites)
2. Clone this repository
3. `cd PhasorSpace-Modules`
4. `make`
5. `make install`
6. Open VCV Rack (on OS X, `open -a 'VCV Rack 2 Pro'`)
7. Verify modules work properly

## The Modules

<img width="1552" alt="image" src="https://github.com/rjungemann/PhasorSpace-Modules/assets/49277/cff8da7b-37b7-42a7-9fad-224a40890829">

### Stochastic Ramp Filter

TODO

### Swung Ramp Generator

TODO

### Phased Ramp Distorter

TODO

### Pitched Ramp Generator

TODO

### Analog Swizzling Register

TODO

## Adding a new module

1. Duplicate and modify an existing SVG file. Assuming your module is named `SomeModule`, you should create a `SomeModule.svg`
2. `name=SomeModule; $RACK_DIR/helper.py createmodule $name res/$name.svg src/$name.cpp` to generate C++ skeleton code from the SVG file
3. Update `plugin.cpp` and `plugin.hpp`
4. Verify `plugin.json` looks correct
5. Build as normal
