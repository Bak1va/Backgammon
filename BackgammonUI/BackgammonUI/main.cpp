#include "BackgammonUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    BackgammonUI window;
    window.show();
    return app.exec();
}
