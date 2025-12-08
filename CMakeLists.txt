cmake_minimum_required(VERSION 3.21)
project(Backgammon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(BackgammonLib)
add_subdirectory(BackgammonUI)
add_subdirectory(BackgammonLogicTests)
