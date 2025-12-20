/**
 * @file main.cpp
 * @brief Entry point for the Backgammon application.
 *
 * This file contains the main function that initializes the Qt application
 * and displays the main game window.
 */

#include "BackgammonUI.hpp"
#include <QApplication>

/**
 * @brief Main entry point of the application.
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Application exit code
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    BackgammonUI window;
    window.show();
    return app.exec();
}
