# Optrone
Optrone is a simple argument parser for Anstro Pleuton's programs. Despite this, it has pretty much all the features you would expect from an argument parser. And more!

# Features
- Long arguments (`--argument`, `--help`, etc.)
- Short arguments (`-a`, `-h`, etc.)
- Combined short arguments (`-ah` treated as `-a` and `-h`)
- Argument with values (`--argument value`, `-a value`, `--argument=value`, `-a=value`, etc.)
- Old Microsoft-style "switches" (`/SWITCH`, `/S`, `/SWITCH:ON`, etc.)

# Prerequisite
- Know to program in C++
- Know to program in C++23 (optional, recommended)
- Know to use third party C++ libraries
- Know to use CMake

# Usage
If you are not Anstro Pleuton, which you probably are, here is a quick and rough guide to use my library:

 1. Setup:
    - Clone this repository.
    ```bash
    git clone https://github.com/anstropleuton/optrone.git
    cd optrone
    ```
 2. Dependencies
    - Clone/update dependencies
    ```bash
    git submodule update --init --recursive
    ```
 3. Build the library:
    - Make build folder:
    ```bash
    mkdir build
    cd build
    ```
    - Build the project:
    ```bash
    cmake ..
    cmake --build .
    ```
 4. Install (optional, Unix/-like only)
    ```
    sudo cmake --install .
    ```
    Note: To uninstall, use `sudo cmake --build . --target uninstall`
 5. If you are using CMake, you can use `find_package`/`add_subdirectory` or `FetchContent_Declare`/`FetchContent_MakeAvailable`, or manually set `include` and `build` as include directories and link `build/liboptrone.a`.

# Documentations
Assuming you are in root directory of this project and built the project in build directory, generate documentation using:
```bash
cd build
doxygen
```

# Changelog
Refer to the [Changelog](Changelog.md) for info about changes in each version.

# Contributing
Check out [Guidelines](Guidelines.md) before you submit your code! Make sure to create a branch with one task with the name of the category of your task (i.e., "features/", "bugfix/", etc.) your task before you change the code.

# Credits
Thanks to [jothepro](https://github.com/jothepro) for the stylesheet [Doxygen Awesome](https://github.com/jothepro/doxygen-awesome-css) for Doxygen.

Thanks to Nuno Pinheiro (can't find link) for the background [Elarun](background.png). But I did find a [KDE store link](https://store.kde.org/p/1162360/) to the background image. I think you could consider that. If KDE developers are here, I would appreciate your help to find a proper link to the author.

Thanks to GLFW for some code in CMake.

# License
This project is licensed under the terms of MIT License, see [License](License.md).

# Junk folder?
That folder is where I put my useless features. Don't use them please.
