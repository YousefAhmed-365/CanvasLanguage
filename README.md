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
  - C-Style Syntax (Mostly)
  - Dynamic Types
    ```python
    string = "Example";
    number = 18;
    arr = ["Laptop", 7, 8, "Apple", "Banana"];
    ```
  - Conditional Statements
    ```python
    score = 85;
    if(score >= 90){
      print("Excellent");
    }elif(score >= 50){
      print("Good");
    }else{
      print("Bad");
    }
    ```
  - Loops
    ```python
    # While loop
    while(1){
      print("Infinity...");
    }

    # Repeat loop
    repeat(7){
      print("Love me... Love me not...");
    }

    # Foreach loop
    items = [7, 8, 9];
    foreach(item in items){
      print(item);
    }

    # For loop is supported but being reworked on as it's support is limited by the parser.
    ```
  - Mathematical, Assignement, and Logical Operators
    `+ - * / % ^`, `+= -= *= /= %= ^=`, `&& ||`
  - User-Defined Functions & Variables
    ```python
    # Normal function
    def sum(x, y){
      ret x + y;
    }

    # Recursive Function
    def factorial(num){
      if(num <= 1){
        ret num;
      }

      ret num + factorial(num - 1);
    }

    # Calling functions dynamically or statically
    sum(3, 7)
    invoke(factorial, 6)

    # Lambda Function
    name = def(){
      print("This is a lambda 1");
    };
    
    invoke(name);
    invoke(def(){print("This is a lambda 2");});
    
    def addFive(function){
        ret invoke(function) + 5;
    }
    printf("Result: %s\n", addFive(def(){ret 4;}));
    ```
  - Basic Input & Output
    ```python
    _inp = input("What's your name?: ");
    printf("Your name is %s!\n", _inp);
    ```
  - Basic Type Conversion
    ```python
    to_str(3);
    to_num("5")
    ```
  - Basic Error Handling
    ```python
    error("Invalid Execution...");
    ```

- **Graphical features** (Not Implemented Yet/Being reimplemented from old code):
  - Drawable objects (Canvas/Sprite)
  - Color support (rgb/rgba/hsl/hsla/hex)
  - Functional drawing (pixel/line/rec...etc.)

<a id="section_3"></a>
## Documentations & Examples
All documentations and examples can be found in the `DOCS` Directory.

### Basic Examples
Note: The language doesn't have an official syntax highlighting extension or a linter yet. 

`hello_world.canvas`
```C
print("Hello, world!");
```

`cat_club.canvas`
``` C
def ask_YesNo(question){
    _inp = input(question + "? (Yes/No): ");
    while(_inp != "Yes" && _inp != "No"){
        _inp = input(question + "? (Yes/No): ");
    } 

    ret _inp;
}

answer = ask_YesNo("Do you like cats");

if(answer == "Yes"){
    printf("%s? Welcome to the cat club!!!", answer);
}else{
    print("Go away!");
}
```

<a id="section_4"></a>
## Usage
```bash
canvas <flags> -e <file>
```

#### example
```bash
canvas -e code.canvas
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
cmake -DCMAKE_BUILD_TYPE=RELEASE
# Use make instead of mingw32-make if you are on Linux.
mingw32-make
```
A new file will appear: **canvas**. Make sure to add it into your system PATH.

<a id="section_6"></a>
## Authors & Credits
- Developed and maintaned by Yousef Ahmed.
