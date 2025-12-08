# BackgammonUI CMake
cmake_minimum_required(VERSION 3.21)

project(BackgammonUI LANGUAGES CXX)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable Qt's MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Collect source files
file(GLOB UI_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)

file(GLOB UI_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp"
)

file(GLOB UI_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/UI/*.ui"
)

file(GLOB QRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/UI/*.qrc"
)

# Explicitly set headers that need MOC processing
set(MOC_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/Include/BackgammonUI.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Include/BoardWidget.hpp"
)

add_executable(BackgammonUI
        ${UI_SOURCES}
        ${MOC_HEADERS}
        ${UI_FILES}
        ${QRC_FILES}
)

target_link_libraries(BackgammonUI
        PRIVATE
        Qt6::Core
        Qt6::Widgets
        Backgammon::Lib
)

target_include_directories(BackgammonUI
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_compile_features(BackgammonUI PRIVATE cxx_std_17)

if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

    add_custom_command(TARGET BackgammonUI POST_BUILD
            COMMAND "${QT_BIN_DIR}/windeployqt.exe"
            --no-compiler-runtime
            --no-translations
            "$<TARGET_FILE:BackgammonUI>"
            COMMENT "Deploying Qt libraries..."
    )
endif()