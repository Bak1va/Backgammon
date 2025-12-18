# BackgammonLib CMake
cmake_minimum_required(VERSION 3.21)

project(BackgammonLib LANGUAGES CXX)

file(GLOB LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)

file(GLOB LIB_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hPP"
)

add_library(BackgammonLib STATIC ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(BackgammonLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(BackgammonLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(Backgammon::Lib ALIAS BackgammonLib)



