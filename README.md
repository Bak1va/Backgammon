# Backgammon

A small C++ Backgammon project that contains:
- `BackgammonLib` — core game logic (library)
- `BackgammonUI` — Qt6-based user interface
- `BackgammonTests` — unit tests (GoogleTest)

## Quick overview
This repository builds a library and a Qt-based UI. CMake is used as the build system; Qt6 (Widgets) is used for the UI. Doxygen support is available to generate API documentation for the library.

## Project layout
- BackgammonLib/ — core library (headers & sources)
- BackgammonUI/ — Qt UI sources, resources and CMake target
- BackgammonTests/ — unit tests

## Prerequisites
- CMake (recommended >= 3.20)
- A C++ toolchain (MinGW or MSVC)
- Qt 6.x (example: `D:/Qt/6.10.1/mingw_64`)
- Doxygen (optional, for API docs)
- Graphviz (optional, provides `dot` used by Doxygen to render graphs)

If using Qt, point CMake at Qt by passing `-DQt6_DIR="<path-to-Qt6-cmake>"` (for example: `-DQt6_DIR="D:/Qt/6.10.1/mingw_64/lib/cmake/Qt6"`).

## Build (examples)
Create a build directory and configure the project. Replace the `-G` generator and toolchain as appropriate for your setup.

- Example (Ninja + MinGW/Qt for MinGW):

```powershell
mkdir build
cd build
cmake -S .. -B . -G "Ninja" -DQt6_DIR="D:/Qt/6.10.1/mingw_64/lib/cmake/Qt6"
cmake --build . --config Debug
```

- Example (Visual Studio generator + Qt compiled for MSVC):

```powershell
mkdir build-vs
cmake -S .. -B build-vs -G "Visual Studio 17 2022" -A x64 -DQt6_DIR="C:/Path/To/Qt/msvc/lib/cmake/Qt6"
cmake --build build-vs --config Debug --target BackgammonUI
```

Notes:
- If CMake cannot find Qt6, set `-DQt6_DIR` to the Qt6 CMake directory (the directory that contains `Qt6Config.cmake`).
- If you mix MinGW-built Qt with MSVC generator (or vice-versa) you'll get toolchain/linker errors. Use the Qt build that matches your compiler.

## Doxygen (API documentation)
This project contains a root `Doxyfile` template and CMake integration that supports two CMake options:

- `-DBUILD_DOCS=ON` — add the `Backgammon-docs` target to the build (requires Doxygen).
- `-DGENERATE_DOCS_ON_CONFIG=ON` — run Doxygen during CMake configure so docs are regenerated automatically on CMake reload.

Where the docs go:
- Generated HTML is placed in the build directory under `docs` (e.g. `build/docs`).

How to use:
- Generate docs on demand (after configuring with `-DBUILD_DOCS=ON`):

```powershell
cmake -S .. -B build -DQt6_DIR="D:/Qt/6.10.1/mingw_64/lib/cmake/Qt6" -DBUILD_DOCS=ON -DGENERATE_DOCS_ON_CONFIG=OFF
cmake --build build --target Backgammon-docs
```

- Regenerate docs automatically on CMake configure (Doxygen runs during configure):

```powershell
cmake -S .. -B build -DQt6_DIR="D:/Qt/6.10.1/mingw_64/lib/cmake/Qt6" -DBUILD_DOCS=ON -DGENERATE_DOCS_ON_CONFIG=ON
```

Troubleshooting:
- If Doxygen is requested but not found, CMake will warn and the `Backgammon-docs` target won’t be added. Install Doxygen or pass `-DBUILD_DOCS=OFF`.
- If you want graphs in the Doxygen output, install Graphviz so `dot` is available.

## Notes & tips
- Keep Qt and the compiler/ABI consistent (MinGW Qt with MinGW builds; MSVC Qt with MSVC builds).
- If your UI executable builds but fails on startup, check that the correct Qt DLLs are found at runtime (for Windows: copy the required Qt DLLs next to the executable or ensure your PATH points to the Qt bin directory).