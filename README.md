![Status](https://img.shields.io/badge/Status-Alpha-orange) ![Version](https://img.shields.io/badge/Version-v0.1-orange) ![Contributions](https://img.shields.io/badge/Contributions-Welcome-brightgreen) ![License](https://img.shields.io/badge/License-MIT-blue)

# Canvas

## Table of content
- [Introduction](#section_1)
- [Features](#section_2)
- [Documentations & Examples](#section_3)
- [Usage](#section_4)
- [Building the source (Windows/Linux)](#section_5)
- [Authors & Credits](#section_6)

<a id="section_1"></a>
## Introduction
**Canvas** is a dynamically-typed functional programming language that focuses more on rendering graphics and experimenting more with dynamic typing with ease of use in mind.

<a id="section_2"></a>
## Features
**Note**: As the project is still in an alpha state, the language or the interpreter are unoptimized and missing a lot of features as everything is still just a proof of concept. More optimizations, bug fixes & features will come with future alpha realases and the 1.0 release version which will rework the entire project from scratch with a better code structure. Documentations & Examples are still being worked on.

- **Core features**:
  - Dynamic Types
  - Conditional Statements
  - Loops
  - User-Defined Functions & Variables
  - Basic Input & Output
  - Basic Type Conversion

- **Graphical features** (Not Implemented Yet):
  - Drawable objects (Canvas/Sprite)
  - Color support (rgb/rgba/hsl/hsla/hex)
  - Functional drawing (pixel/line/rec...etc.)

<a id="section_3"></a>
## Documentations & Examples
All of the documentations and examples can be found in the **EXAMPLES** and **DOCS** dir of the source.

<a id="section_4"></a>
## Usage
```bash
canvas <flags> -c <file>
```

#### example
```bash
canvas -c code.canvas
```
Use the -h or --help flag for more information:
```bash
canvas --help
```

<a id="section_5"></a>
## Building the source (Windows/Linux)
#### Requirements
- Mingw-W64 Tools
- CMake (version 3.0 or higher).
- Boost Regex Library.

#### Using CMake

Clone the repository:
```bash
git clone https://github.com/YousefAhmed-365/Canvas-Language.git
```

Open the terminal in the source directory and then use the following commands:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release
# Use make instead of mingw32-make if you are on Linux.
mingw32-make
```
A new file will appear: **canvas**. Make sure to add it into your system PATH.

<a id="section_6"></a>
## Authors & Credits
- Developed and maintaned by Yousef Ahmed.
