#include "mainwindow.h"
#include "search_engine.h"

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
