#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication EmpireEarthZoomChanger(argc, argv);
    MainWindow window;
    window.show();

    return EmpireEarthZoomChanger.exec();
}
