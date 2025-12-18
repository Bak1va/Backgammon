cmake_minimum_required(VERSION 3.14)

enable_testing()

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

add_executable(BackgammonLogicTests ${TEST_SOURCES})

target_link_libraries(BackgammonLogicTests PRIVATE
        Backgammon::Lib
        gtest_main
        gmock
)

target_include_directories(BackgammonLogicTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME BackgammonLogicTests COMMAND BackgammonLogicTests)