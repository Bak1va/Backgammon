cmake_minimum_required(VERSION 3.21)
project(Backgammon LANGUAGES CXX)

# Build static libraries by default
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(BUILD_TESTING "Build tests" ON)
option(BUILD_UI "Build the graphical user interface" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Subprojects
add_subdirectory(BackgammonLib)

#[[if(BUILD_UI)
    add_subdirectory(BackgammonUI/BackgammonUI)
else()
    message(STATUS "BUILD_UI=OFF: Skipping GUI (BackgammonUI). Configure with -DBUILD_UI=ON to enable it.")
endif()]]

if(BUILD_TESTING AND EXISTS "${CMAKE_SOURCE_DIR}/BackgammonLogicTests")
    add_subdirectory(BackgammonLogicTests)
endif()
