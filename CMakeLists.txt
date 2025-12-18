cmake_minimum_required(VERSION 3.21)
project(Backgammon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(BackgammonLib)
add_subdirectory(BackgammonUI)
add_subdirectory(BackgammonTests)

find_package(Doxygen QUIET)
option(BUILD_DOCS "Build Doxygen documentation" ${DOXYGEN_FOUND})
option(GENERATE_DOCS_ON_CONFIG "Run Doxygen during CMake configure (regenerate docs on CMake reload)" ON)

if (BUILD_DOCS AND DOXYGEN_FOUND)
    set(DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/BackgammonLib/Include ${CMAKE_SOURCE_DIR}/BackgammonLib/Source")

    set(DOXYFILE_IN ${CMAKE_SOURCE_DIR}/Doxyfile)
    set(DOXYFILE_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    add_custom_target(Backgammon-docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating Backgammon project documentation with Doxygen"
        VERBATIM
    )

    message(STATUS "Backgammon docs: target 'Backgammon-docs' added (output: ${CMAKE_BINARY_DIR}/docs)")

    if (GENERATE_DOCS_ON_CONFIG)
        message(STATUS "GENERATE_DOCS_ON_CONFIG is ON: running Doxygen now (during configuration)")
        execute_process(COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                        RESULT_VARIABLE DOXY_RESULT
                        OUTPUT_VARIABLE DOXY_OUTPUT
                        ERROR_VARIABLE DOXY_ERROR
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
        if (NOT DOXY_RESULT EQUAL 0)
            message(WARNING "Doxygen execution failed during configure (code ${DOXY_RESULT}). Output:\n${DOXY_ERROR}${DOXY_OUTPUT}")
        else()
            message(STATUS "Doxygen ran successfully during configure")
        endif()
    endif()
elseif(BUILD_DOCS AND NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen requested (BUILD_DOCS=ON) but Doxygen not found. Install Doxygen or set BUILD_DOCS=OFF.")
else()
    message(STATUS "Doxygen support disabled or not found; no documentation target added.")
endif()
