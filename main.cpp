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


//    SearchEngine searchEngine;

//    QElapsedTimer timer;

//    timer.start();
//    searchEngine.makeIndex("D:/Education/University/3rdCSE/DataStructure/SearchEngineProject/questions");


//    qInfo() << "indexing took " << timer.restart() /1000.0 << "Sec\n\n\n";

//    QSet<QString> list = searchEngine.search("better");

//    qInfo() << "search(better) took " << timer.restart() << "ms" << "\n\n\n";

//    MainWindow w;
//    for(const auto &item: list)
//    {
//        w.addFile(item);
//    }

//    qInfo() << "women in " <<  searchEngine.search("women").size();


//    qInfo() << "search(women) took " << timer.restart() << "ms" << "\n\n\n";

//    qInfo() << searchEngine.search("khjskjhjkhlkjshkjk").size();

//    qInfo() << "khjskjhjkhlkjshkjk in " << "search(khjskjhjkhlkjshkjk) took" << timer.elapsed();



    MainWindow w;
    w.show();
    return a.exec();
}
