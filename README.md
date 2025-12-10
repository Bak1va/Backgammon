# Backgammon

## Overview


## Prerequisites
- CMake (recommended >= 3.20)
- MinGW (mingw-w64) and its tools available on `PATH`
- Qt 6.10.1 installed (example path: `D:/Qt/6.10.1/mingw_64`)

## Build steps
```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" -DQt6_DIR="<PATH_T0_QT_CMAKE_LIBS>" .. # Example -DQt6_DIR="D:/Qt/6.10.1/mingw_64/lib/cmake/Qt6"
cmake --build . --config Release